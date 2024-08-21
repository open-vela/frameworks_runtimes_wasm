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

#include <stddef.h>
#include <stdint.h>
#include <syslog.h>

#define REGISTER_REGION_START CONFIG_WASM_MEMORY_REGISTER_REGION_START
#define REGISTER_REGION_END CONFIG_WASM_MEMORY_REGISTER_REGION_END

/* Forward declarations of WASMModuleInstance struct that only used once in this
 * file
 */
struct WASMModuleInstance;

/* Override the weak function in aot_runtime.c:
 * uint8 * aot_bounds_check(AOTModuleInstance *module_inst, size_t offset,
 * uint32 bytes) to provide bounds checking for WASM register access
 */

uint8_t* aot_bounds_check(struct WASMModuleInstance* module_inst, size_t offset,
    uint32_t bytes)
{

    if (offset + bytes > REGISTER_REGION_END) {
        syslog(LOG_ERR, "WASM register access out of bounds");
        return NULL;
    }

    if (offset < REGISTER_REGION_START) {
        syslog(LOG_ERR, "Wasm register access before start of region");
        return NULL;
    }

    return (uint8_t*)offset;
}
