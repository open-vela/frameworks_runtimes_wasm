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

#ifndef __WASM_IRQ_MANAGER__
#define __WASM_IRQ_MANAGER__

#include <nuttx/list.h>
#include <nuttx/pthread.h>
#include <stdatomic.h>
#include <stdint.h>

#include "wasm_export.h"

#define WASM_IRQ_RUN 1
#define WASM_IRQ_STOP 0

/*
 * This interrupt needs to be released
 */
#define WASM_IRQ_STATUS_RELEASED 1

/**
 * @struct wasm_irq_thread
 * @brief Represents a WebAssembly (Wasm) IRQ handling thread structure.
 *
 * This structure is used to manage the state and behavior of a thread dedicated to
 * handling interrupt requests (IRQs) in a WebAssembly runtime environment. It includes
 * information about the thread's execution state, interrupt status, and synchronization
 * mechanisms necessary for efficient IRQ processing. Each instance of this structure
 * corresponds to a specific thread that handles one or more IRQs.
 *
 * Members:
 * - irq_exit: A boolean flag indicating whether the IRQ thread should exit.
 *   When true, it signals that the thread is requested to terminate its operations.
 * - is_irq_enable: A boolean flag that determines if interrupts are currently enabled.
 *   If true, the thread is allowed to process incoming IRQs; if false, it will ignore them.
 * - pt_irq: A POSIX thread identifier for the IRQ handling thread.
 *   This ID is utilized for thread management functions, allowing for operations
 *   such as joining or canceling the thread.
 * - exec_env: The execution environment for the current thread.
 *   This structure holds context information needed for executing WebAssembly code.
 * - spawn_exec_env: The execution environment for any threads that are spawned.
 *   It allows for the management of separate execution contexts for newly created threads,
 *   facilitating concurrency in the runtime.
 * - irq_list: A linked list node used to manage IRQ-related items.
 *   This enables efficient storage and organization of interrupt requests that
 *   the thread may need to handle.
 * - irq_status: An atomic integer representing the current status of interrupts.
 *   This variable allows for thread-safe updates and reads of the IRQ state,
 *   preventing race conditions during concurrent access.
 * - wasm_irq_sem: A semaphore for signaling between threads regarding IRQ events.
 *   The IRQ thread uses this semaphore to wait for events and to be notified when
 *   it needs to process an IRQ.
 */

typedef struct wasm_thread_s {
    bool irq_exit;
    bool is_irq_enable;
    pthread_t pt_irq;
    wasm_exec_env_t exec_env;
    wasm_exec_env_t spawn_exec_env;
    struct list_node irq_list;
    atomic_int irq_status;
    sem_t wasm_irq_sem;
} wasm_irq_thread;

/**
 * @struct wasm_irq_s
 * @brief Represents a WebAssembly (Wasm) interrupt request (IRQ) structure.
 *
 * This structure is used to manage interrupt requests in a WebAssembly runtime environment.
 * It holds information related to the interrupt status, reference counting, callback handling,
 * and associated thread control. Each instance of this structure corresponds to a specific
 * IRQ identified by its unique ID.
 *
 * Members:
 * - list: A linked list node to facilitate the storage and management of multiple
 *   wasm_irq_t instances within a collection.
 * - status: An integer flag indicating the current status of the interrupt,
 *   allowing for the release or acknowledgment of the interrupt.
 * - id: A unique identifier for the IRQ, representing its number within the system.
 * - refs: An atomic integer used for managing the reference count of the structure,
 *   ensuring safe concurrent access in multi-threaded environments.
 * - index: An index used to specify the callback address for the Wasm IRQ handler.
 * - arg: An array of function parameters (with variable length) to be passed to the
 *   IRQ callback function, allowing for flexible argument handling.
 * - irq_thread: A pointer to the control structure for the thread associated
 *   with handling this interrupt, enabling proper synchronization and execution flow.
 */

typedef struct wasm_irq_s {
    struct list_node list;
    int status;
    unsigned int id;
    atomic_int refs;
    unsigned int index;
    unsigned int arg[1];
    wasm_irq_thread* irq_thread;
} wasm_irq_t;

wasm_irq_t* wasm_interrupt_register(int irq, wasm_irq_thread* wasm_thread, unsigned int index, unsigned int arg);
void wasm_interrupt_unregister(int irq, wasm_irq_thread* wasm_thread);
void* wasm_irq_thread_cb(void* arg);
int wasm_irq_thread_create(wasm_module_inst_t inst, wasm_irq_thread* wasm_thread);
void wasm_irq_thread_exit(wasm_irq_thread* wasm_thread);

#endif