/*
 * Copyright (C) 2024 Xiaomi Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <errno.h>
#include <nuttx/config.h>
#include <nuttx/pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <syslog.h>

#include "wasm_irq_manager.h"

void* wasm_irq_thread_cb(void* arg)
{
    wasm_irq_t* node;
    wasm_irq_t* temp;
    wasm_irq_thread* wasm_thread = (wasm_irq_thread*)arg;
    bool flag = false;

    if (!wasm_runtime_init_thread_env()) {
        return NULL;
    }

    while (!wasm_thread->irq_exit) {
        flag = false;
        if (!list_is_empty(&wasm_thread->irq_list)) {
            list_for_every_entry_safe(&wasm_thread->irq_list, node, temp, struct wasm_irq_s, list)
            {
                if (node->status == WASM_IRQ_STATUS_RELEASED) {
                    list_delete(&node->list);
                    free(node);
                    continue;
                }

                if (node->refs > 0) {
                    if (!wasm_runtime_call_indirect(wasm_thread->exec_env, node->index, 1, (uint32_t*)node->arg)) {
                        wasm_thread->irq_exit = true;
                        break;
                    }
                    atomic_fetch_sub_explicit(&node->refs, 1, memory_order_relaxed);
                    continue;
                }
            }

            list_for_every_entry(&wasm_thread->irq_list, node, struct wasm_irq_s, list)
            {
                if (node->refs > 0) {
                    flag = true;
                    break;
                }
            }
        }

        if (!flag && wasm_thread->irq_exit == false) {
            atomic_store_explicit(&wasm_thread->irq_status, WASM_IRQ_STOP, memory_order_relaxed);
            sem_wait(&wasm_thread->wasm_irq_sem);
            atomic_store_explicit(&wasm_thread->irq_status, WASM_IRQ_RUN, memory_order_relaxed);
        }
    }

    wasm_runtime_destroy_thread_env();
    return NULL;
}

static wasm_irq_t* wasm_interrupt_malloc(void)
{
    return malloc(sizeof(wasm_irq_t));
}

wasm_irq_t* wasm_interrupt_register(int irq, wasm_irq_thread* wasm_thread, unsigned int index, unsigned int arg)
{
    wasm_irq_t* node;
    if (!wasm_thread->is_irq_enable) {
        set_errno(-EINVAL);
        return NULL;
    }

    if (!list_is_empty(&wasm_thread->irq_list)) {
        list_for_every_entry(&wasm_thread->irq_list, node, struct wasm_irq_s, list)
        {
            if (node->status != WASM_IRQ_STATUS_RELEASED && node->id == irq) {
                set_errno(-EEXIST);
                return NULL;
            }
        }
    }

    node = wasm_interrupt_malloc();
    if (!node) {
        set_errno(-ENOMEM);
        return NULL;
    }

    node->status = 0;
    node->id = irq;
    node->refs = 0;
    node->index = index;
    node->arg[0] = arg;
    node->irq_thread = wasm_thread;

    list_add_tail(&wasm_thread->irq_list, &node->list);

    return node;
}

void wasm_interrupt_unregister(int irq, wasm_irq_thread* wasm_thread)
{
    wasm_irq_t* node;
    wasm_irq_t* temp;

    if (!wasm_thread->is_irq_enable) {
        return;
    }

    if (list_is_empty(&wasm_thread->irq_list)) {
        return;
    }
    syslog(LOG_INFO, "wasm_interrupt_unregister start!!");
    list_for_every_entry_safe(&wasm_thread->irq_list, node, temp, struct wasm_irq_s, list)
    {
        if (node->id == irq) {
            node->status = WASM_IRQ_STATUS_RELEASED;
            if (wasm_thread->irq_status == WASM_IRQ_STOP) {
                sem_post(&wasm_thread->wasm_irq_sem);
            }
        }
    }
}

int wasm_irq_thread_create(wasm_module_inst_t inst, wasm_irq_thread* wasm_thread)
{
    int ret = -EINVAL;
    pthread_attr_t attr = { 0 };

    wasm_thread->exec_env = wasm_runtime_create_exec_env(inst, 4);
    if (!wasm_thread->exec_env) {
        return ret;
    }

    wasm_thread->spawn_exec_env = wasm_runtime_spawn_exec_env(wasm_thread->exec_env);
    if (!wasm_thread->spawn_exec_env) {
        wasm_runtime_destroy_exec_env(wasm_thread->exec_env);
        return ret;
    }

    list_initialize(&wasm_thread->irq_list);
    sem_init(&wasm_thread->wasm_irq_sem, 1, 0);
    wasm_thread->irq_status = WASM_IRQ_STOP;
    wasm_thread->irq_exit = false;

    memcpy(&attr, &g_default_pthread_attr, sizeof(pthread_attr_t));
    attr.priority = CONFIG_WASM_IRQ_THREAD_PRIORITY;
    attr.stacksize = CONFIG_WASM_IRQ_THREAD_STACK_SIZE;

    ret = pthread_create(&wasm_thread->pt_irq, &attr, wasm_irq_thread_cb, (void*)wasm_thread);
    if (ret == 0) {
        pthread_setname_np(wasm_thread->pt_irq, "wasm_irq");
        pthread_attr_destroy(&attr);
        wasm_thread->is_irq_enable = true;
    } else {
        wasm_runtime_destroy_spawned_exec_env(wasm_thread->spawn_exec_env);
        wasm_runtime_destroy_exec_env(wasm_thread->exec_env);
        pthread_attr_destroy(&attr);
        list_clear_node(&wasm_thread->irq_list);
        sem_destroy(&wasm_thread->wasm_irq_sem);
    }

    return ret;
}

void wasm_irq_thread_exit(wasm_irq_thread* wasm_thread)
{
    if (wasm_thread->pt_irq < 0) {
        return;
    }

    wasm_thread->irq_exit = true;
    if (wasm_thread->irq_status == WASM_IRQ_STOP) {
        sem_post(&wasm_thread->wasm_irq_sem);
    }

    pthread_join(wasm_thread->pt_irq, NULL);

    wasm_thread->irq_status = WASM_IRQ_STOP;
    list_clear_node(&wasm_thread->irq_list);
    sem_destroy(&wasm_thread->wasm_irq_sem);
    wasm_runtime_destroy_spawned_exec_env(wasm_thread->spawn_exec_env);
    wasm_runtime_destroy_exec_env(wasm_thread->exec_env);
}
