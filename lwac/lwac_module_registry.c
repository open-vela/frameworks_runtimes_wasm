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

#include "lwac_module_registry.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Head of the linked list
static wasm_module_info_t* g_module_list_head = NULL;

// Read-write lock for thread-safe access to the module list
static pthread_rwlock_t g_module_list_rwlock = PTHREAD_RWLOCK_INITIALIZER;

// Module ID counter for unique identification
static uint32_t g_module_id_counter = 0;

/**
 * Register a new running Wasm module in the tracking system
 */
wasm_module_info_t* lwac__register_wasm_module(const char* name,
    int stack_size,
    int heap_size,
    wasm_module_t module,
    wasm_module_inst_t module_inst,
    wasm_exec_env_t exec_env)
{
    wasm_module_info_t* module_info = NULL;

    // Allocate memory for the module info using system malloc
    module_info = (wasm_module_info_t*)malloc(sizeof(wasm_module_info_t));
    if (!module_info) {
        printf("Error: Failed to allocate memory for module info\n");
        return NULL;
    }

    // Initialize the structure
    memset(module_info, 0, sizeof(wasm_module_info_t));

    // Copy name
    strlcpy(module_info->name, name, sizeof(module_info->name));

    // Set other parameters
    module_info->stack_size = stack_size;
    module_info->heap_size = heap_size;
    module_info->start_time = time(NULL);
    module_info->module = module;
    module_info->module_inst = module_inst;
    module_info->exec_env = exec_env;

    // Add to the linked list with write lock
    pthread_rwlock_wrlock(&g_module_list_rwlock);
    // Set the module ID
    module_info->module_id = ++g_module_id_counter;
    module_info->next = g_module_list_head;
    g_module_list_head = module_info;
    pthread_rwlock_unlock(&g_module_list_rwlock);

    return module_info;
}

/**
 * Unregister a Wasm module from the tracking system
 */
int lwac__unregister_wasm_module(wasm_module_inst_t module_inst)
{
    wasm_module_info_t* current = NULL;
    wasm_module_info_t* prev = NULL;
    int result = -1;

    // Acquire write lock for modification
    pthread_rwlock_wrlock(&g_module_list_rwlock);

    // Find the module in the list
    current = g_module_list_head;
    while (current) {
        if (current->module_inst == module_inst) {
            // Remove from the list
            if (prev) {
                prev->next = current->next;
            } else {
                g_module_list_head = current->next;
            }

            // Free the memory using system free
            free(current);
            result = 0;
            break;
        }

        prev = current;
        current = current->next;
    }

    pthread_rwlock_unlock(&g_module_list_rwlock);
    return result; // -1 if not found, 0 if successfully removed
}

/**
 * Find module info by module ID
 */
wasm_module_info_t* lwac__find_wasm_module_by_id(uint32_t module_id)
{
    wasm_module_info_t* current = NULL;

    // Acquire read lock for searching
    pthread_rwlock_rdlock(&g_module_list_rwlock);

    current = g_module_list_head;
    while (current) {
        if (current->module_id == module_id) {
            pthread_rwlock_unlock(&g_module_list_rwlock);
            return current;
        }
        current = current->next;
    }

    pthread_rwlock_unlock(&g_module_list_rwlock);
    return NULL;
}

/**
 * Find module info by module name
 */
int lwac__find_wasm_module_by_name(const char* name, wasm_module_info_t* module_info)
{
    wasm_module_info_t* current = NULL;

    if (!name || !module_info) {
        return -EINVAL;
    }

    // Acquire read lock for searching
    pthread_rwlock_rdlock(&g_module_list_rwlock);

    current = g_module_list_head;
    while (current) {
        if (strncmp(current->name, name, sizeof(current->name)) == 0) {
            // Copy the module info data
            memcpy(module_info, current, sizeof(wasm_module_info_t));
            // Clear the next pointer to avoid exposing internal structure
            module_info->next = NULL;
            pthread_rwlock_unlock(&g_module_list_rwlock);
            return 0;
        }
        current = current->next;
    }

    pthread_rwlock_unlock(&g_module_list_rwlock);
    return -ENOENT;
}

/**
 * Get the count of running modules
 */
int lwac__get_running_module_count(void)
{
    wasm_module_info_t* current = NULL;
    int count = 0;

    // Acquire read lock for counting
    pthread_rwlock_rdlock(&g_module_list_rwlock);

    current = g_module_list_head;
    while (current) {
        count++;
        current = current->next;
    }

    pthread_rwlock_unlock(&g_module_list_rwlock);
    return count;
}

/**
 * Print information about all running modules
 */
void lwac__print_running_modules(void)
{
    wasm_module_info_t* current = NULL;
    time_t current_time;
#ifdef CONFIG_INTERPRETERS_WAMR_MEMORY_PROFILING
    wasm_aux_stack_info_t aux_stack_info;
#endif
    mem_alloc_info_t heap_info;

    pthread_rwlock_rdlock(&g_module_list_rwlock);

    current_time = time(NULL);

    printf("Running Wasm Modules:\n");
    printf("ID  %-40s %-8s %-12s %-12s %-12s %-12s %s\n", "Name", "Stack", "Heap Total", "Heap Used", "Heap Free", "Aux Used", "Uptime");
    printf("--- %-40s %-8s %-12s %-12s %-12s %-12s %s\n", "----", "-----", "-----------", "---------", "---------", "--------", "--------");

    current = g_module_list_head;
    while (current) {
        time_t elapsed_seconds = current_time - current->start_time;
        int hours = elapsed_seconds / 3600;
        int minutes = (elapsed_seconds % 3600) / 60;
        int seconds = elapsed_seconds % 60;

        bool heap_info_available = wasm_runtime_get_mem_alloc_info(&heap_info);

        if (!heap_info_available) {
            printf("%-3lu %-40s %-8d %-12s %-12s %-12s %-12s %02d:%02d:%02d\n",
                (unsigned long)current->module_id,
                current->name,
                current->stack_size,
                "N/A",
                "N/A",
                "N/A",
                "N/A",
                hours, minutes, seconds);
            current = current->next;
            continue;
        }

#ifdef CONFIG_INTERPRETERS_WAMR_MEMORY_PROFILING
        bool aux_stack_available = wasm_runtime_get_aux_stack_info(current->exec_env, &aux_stack_info);

        if (aux_stack_available) {
            printf("%-3lu %-40s %-8d %-12lu %-12lu %-12lu %-12lu %02d:%02d:%02d\n",
                (unsigned long)current->module_id,
                current->name,
                current->stack_size,
                (unsigned long)heap_info.total_size,
                (unsigned long)heap_info.used_size,
                (unsigned long)heap_info.free_size,
                (unsigned long)aux_stack_info.current_aux_stack_used,
                hours, minutes, seconds);
        } else {
            printf("%-3lu %-40s %-8d %-12lu %-12lu %-12lu %-12s %02d:%02d:%02d\n",
                (unsigned long)current->module_id,
                current->name,
                current->stack_size,
                (unsigned long)heap_info.total_size,
                (unsigned long)heap_info.used_size,
                (unsigned long)heap_info.free_size,
                "N/A",
                hours, minutes, seconds);
        }
#else
        printf("%-3lu %-40s %-8d %-12lu %-12lu %-12lu %-12s %02d:%02d:%02d\n",
            (unsigned long)current->module_id,
            current->name,
            current->stack_size,
            (unsigned long)heap_info.total_size,
            (unsigned long)(heap_info.total_size - heap_info.total_free_size),
            (unsigned long)heap_info.total_free_size,
            "N/A",
            hours, minutes, seconds);
#endif

        current = current->next;
    }

    pthread_rwlock_unlock(&g_module_list_rwlock);
}