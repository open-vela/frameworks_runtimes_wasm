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
#include <sys/param.h>
#include <syslog.h>

#include "wasm_export.h"

#ifdef CONFIG_ARCH_CHIP_BL616

#define REGISTER_REGION_START 0x20000000
#define REGISTER_REGION_END 0x20072000

#else

#error "No register region defined for this chip"

#endif

#ifdef CONFIG_WASM_MEMORY_REGISTER_ACCESS

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
#endif

#ifdef CONFIG_WASM_MEMORY_PUTREG_GETREG

/* Provide the register access functions for WASM */

static void putreg8_wrapper(wasm_exec_env_t exec_env, uint8_t value, uint32_t addr)
{
    *((volatile uint8_t*)addr) = value;
}

static void putreg16_wrapper(wasm_exec_env_t exec_env, uint16_t value, uint32_t addr)
{
    *((volatile uint16_t*)addr) = value;
}

static void putreg32_wrapper(wasm_exec_env_t exec_env, uint32_t value, uint32_t addr)
{
    *((volatile uint32_t*)addr) = value;
}

static uint8_t getreg8_wrapper(wasm_exec_env_t exec_env, uint32_t addr)
{
    return *((volatile uint8_t*)addr);
}

static uint16_t getreg16_wrapper(wasm_exec_env_t exec_env, uint32_t addr)
{
    return *((volatile uint16_t*)addr);
}

static uint32_t getreg32_wrapper(wasm_exec_env_t exec_env, uint32_t addr)
{
    return *((volatile uint32_t*)addr);
}

/* Register access functions */

static NativeSymbol g_register_access_symbols[] = {
    EXPORT_WASM_API_WITH_SIG2(getreg8, "(i)i"),
    EXPORT_WASM_API_WITH_SIG2(getreg16, "(i)i"),
    EXPORT_WASM_API_WITH_SIG2(getreg32, "(i)i"),
    EXPORT_WASM_API_WITH_SIG2(putreg8, "(ii)"),
    EXPORT_WASM_API_WITH_SIG2(putreg16, "(ii)"),
    EXPORT_WASM_API_WITH_SIG2(putreg32, "(ii)"),
};

/* Register access module, this function is called by the WAMR runtime */

bool wamr_module_register_access_register(void)
{
    return wasm_runtime_register_natives(
        "env", g_register_access_symbols, nitems(g_register_access_symbols));
}

#endif
