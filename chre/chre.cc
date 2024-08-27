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

#include <sys/param.h>

#include "./include/chre_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REG_CHRE_NATIVE_FUNC(func_name, signature)               \
    {                                                            \
#func_name, (void*)(func_name##Wrapper), signature, NULL \
    }

static NativeSymbol g_chre_symbols[] = {
#include "./include/chre.inl"
};

int wamr_module_chre_register(void)
{
    return wasm_runtime_register_natives("chre", g_chre_symbols,
        nitems(g_chre_symbols));
}

#ifdef __cplusplus
}
#endif
