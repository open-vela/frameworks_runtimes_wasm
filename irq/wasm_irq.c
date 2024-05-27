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
#include <nuttx/arch.h>
#include <nuttx/config.h>
#include <nuttx/irq.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/param.h>
#include <syslog.h>

#include "wasm_irq_manager.h"

static int __wasm_irq_hander(int irq, void* context, void* arg)
{
    wasm_irq_t* p = (wasm_irq_t*)arg;

    atomic_fetch_add_explicit(&p->refs, 1, memory_order_relaxed);
    if (p->irq_thread->irq_status == WASM_IRQ_STOP) {
        up_disable_irq(p->id);
        sem_post(&p->irq_thread->wasm_irq_sem);
    }

    return 0;
}

static int irq_attach_wrapper(wasm_exec_env_t exec_env, int irq, unsigned int index, unsigned int arg)
{
    int ret;
    wasm_irq_t* node;
    node = wasm_interrupt_register(irq, wasm_runtime_get_user_data(exec_env), index, arg);
    if (!node) {
        ret = get_errno();
        return ret;
    }

    ret = irq_attach(irq, __wasm_irq_hander, node);
    return ret;
}

static void irq_enable_wrapper(wasm_exec_env_t exec_env, int irq)
{
    up_enable_irq(irq);
}

static void irq_disable_wrapper(wasm_exec_env_t exec_env, int irq)
{
    up_disable_irq(irq);
}

static void irq_initialize_wrapper(wasm_exec_env_t exec_env)
{
    int ret;
    wasm_irq_thread* wasm_thread = NULL;

    wasm_thread = (wasm_irq_thread*)malloc(sizeof(wasm_irq_thread));
    if (wasm_thread == NULL) {
        syslog(LOG_ERR, "wasm_irq_thread malloc failed \n");
        return;
    }

    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    ret = wasm_irq_thread_create(module_inst, wasm_thread);
    if (ret != 0) {
        syslog(LOG_ERR, "create irq pthread err\n");
        free(wasm_thread);
    } else {
        wasm_runtime_set_user_data(exec_env, (void*)wasm_thread);
    }
}

static void irq_uninitialize_wrapper(wasm_exec_env_t exec_env)
{
    wasm_irq_thread_exit(wasm_runtime_get_user_data(exec_env));
    free(wasm_runtime_get_user_data(exec_env));
}

static NativeSymbol g_wasm_symbols_irq[] = {
    EXPORT_WASM_API_WITH_SIG2(irq_attach, "(iii)i"),
    EXPORT_WASM_API_WITH_SIG2(irq_enable, "(i)"),
    EXPORT_WASM_API_WITH_SIG2(irq_disable, "(i)"),
    EXPORT_WASM_API_WITH_SIG2(irq_initialize, "()"),
    EXPORT_WASM_API_WITH_SIG2(irq_uninitialize, "()"),
};

bool wamr_module_irq_register(void)
{
    return wasm_runtime_register_natives("env", g_wasm_symbols_irq,
        nitems(g_wasm_symbols_irq));
}
