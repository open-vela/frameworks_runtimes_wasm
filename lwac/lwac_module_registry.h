/*
 * Copyright (C) 2025 Xiaomi Corporation
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

#pragma once

#include <time.h>
#include <wasm_export.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Module info structure for registered Wasm modules
 */
typedef struct wasm_module_info_s {
    uint32_t module_id; /**< Unique identifier for the module */
    char name[64]; /**< Module name (derived from file name) */
    int stack_size; /**< Stack size in bytes */
    int heap_size; /**< Heap size in bytes */
    time_t start_time; /**< Module start timestamp */
    wasm_module_t module; /**< WAMR module handle */
    wasm_module_inst_t module_inst; /**< WAMR module instance handle */
    wasm_exec_env_t exec_env; /**< Execution environment handle */
    struct wasm_module_info_s* next; /**< Pointer to next module in list */
} wasm_module_info_t;

/**
 * Register a new running Wasm module in the info system
 *
 * @param name Module name (derived from file name)
 * @param stack_size Stack size in bytes
 * @param heap_size Heap size in bytes
 * @param module WAMR module handle
 * @param module_inst WAMR module instance handle
 * @param exec_env Execution environment handle
 * @return Pointer to the newly created module info structure, or NULL on failure
 */
wasm_module_info_t* lwac__register_wasm_module(const char* name,
    int stack_size,
    int heap_size,
    wasm_module_t module,
    wasm_module_inst_t module_inst,
    wasm_exec_env_t exec_env);

/**
 * Unregister a Wasm module from the info system
 *
 * @param module_inst WAMR module instance handle to unregister
 * @return 0 on success, -1 if module not found
 */
int lwac__unregister_wasm_module(wasm_module_inst_t module_inst);

/**
 * Find module info by module ID
 *
 * @param module_id Module ID to search for
 * @return Pointer to module info structure if found, NULL otherwise
 */
wasm_module_info_t* lwac__find_wasm_module_by_id(uint32_t module_id);

/**
 * Find module info by module name
 *
 * @param name Module name to search for
 * @param module_info Pointer to module info structure to copy data to
 * @return 0 on success (module found and data copied),
 *         -EINVAL if name or module_info is NULL,
 *         -ENOENT if module with specified name is not found
 */
int lwac__find_wasm_module_by_name(const char* name, wasm_module_info_t* module_info);

/**
 * Get the count of running modules
 *
 * @return Number of currently running Wasm modules
 */
int lwac__get_running_module_count(void);

/**
 * Print information about all running modules
 *
 * Prints a formatted table showing ID, name, stack size, heap size,
 * and runtime duration for all currently registered Wasm modules.
 * Runtime is shown as hh:mm:ss format indicating how long the module
 * has been running.
 *
 * Sample output:
 * Running Wasm Modules:
 * ID  Name                                     Stack    Heap     Runtime
 * --- ----                                     -----    ----     --------
 * 1   /inc/inc_demo.wasm.aot                   2048     4096     00:01:40
 */
void lwac__print_running_modules(void);

#ifdef __cplusplus
}
#endif
