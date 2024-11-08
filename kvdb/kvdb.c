/*
 * Copyright (C) 2024 Xiaomi Corperation
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

#include <stdint.h>
#include <sys/param.h>

#include "kvdb.h"
#include "wasm_export.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int property_set_buffer_wrapper(wasm_exec_env_t env,
    const char* key,
    const void* value,
    size_t size)
{
    return property_set_buffer(key, value, size);
}

static NativeSymbol g_kvdb_symbols[] = {
    EXPORT_WASM_API_WITH_SIG2(property_set_buffer, "($*i)i"),
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

bool wamr_module_kvdb_register(void)
{
    /* Add frameworks section init hook here */

    return wasm_runtime_register_natives("env", g_kvdb_symbols,
        nitems(g_kvdb_symbols));
}
