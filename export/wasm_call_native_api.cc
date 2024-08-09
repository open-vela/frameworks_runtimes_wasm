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

#include "wasm_call_native_api.h"

#ifdef CONFIG_CHRE_WASM
#include "./../chre/include/chre_wrapper.h"
#endif

#define REG_NATIVE_FUNC(func_name, signature)                 \
{                                                             \
    #func_name, (void*)(func_name##_wrapper), signature, NULL \
}

#define REG_CHRE_NATIVE_FUNC(func_name, signature)            \
{                                                             \
    #func_name, (void*)(func_name##Wrapper), signature, NULL  \
}

static NativeSymbol extended_native_symbol_defs[] = {
#ifdef CONFIG_CHRE_WASM
#include "./../chre/include/chre.inl"
#endif
};

int get_ext_export_apis(NativeSymbol **p_ext_apis) {
    *p_ext_apis = extended_native_symbol_defs;
    return sizeof(extended_native_symbol_defs) / sizeof(NativeSymbol);
}
