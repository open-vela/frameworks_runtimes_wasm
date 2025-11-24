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

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdint.h>
#include <syslog.h>
#include "feature_exports.h"
#include "feature_context.h"
#include "feature_qjs_exports.h"

#include "wasm_export.h"

static void
va_list_string2conv(wasm_exec_env_t exec_env, const char* format,
    va_list ap, bool to_native)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    char* pos = *((char**)&ap);
    if (pos == NULL) {
        return;
    }
    int long_ctr = 0;
    int might = 0;
    while (*format) {
        if (!might) {
            if (*format == '%') {
                might = 1;
                long_ctr = 0;
            }
        } else {
            switch (*format) {
            case '.':
            case '+':
            case '-':
            case ' ':
            case '#':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                goto still_might;
            case 't':
            case 'z':
                long_ctr = 1;
                goto still_might;
            case 'j':
                long_ctr = 2;
                goto still_might;
            case 'l':
                long_ctr++;
            case 'h':
                goto still_might;
            case 'o':
            case 'd':
            case 'i':
            case 'u':
            case 'p':
            case 'x':
            case 'X':
            case 'c': {
                if (long_ctr < 2) {
                    pos += sizeof(int32_t);
                } else {
                    pos += sizeof(int64_t);
                }
                break;
            }
            case 'e':
            case 'E':
            case 'g':
            case 'G':
            case 'f':
            case 'F': {
                pos += sizeof(double);
                break;
            }
            case 's': {
                if (to_native) {
                    *(uintptr_t*)pos = (uintptr_t)addr_app_to_native(*(uintptr_t*)pos);
                } else {
                    *(uintptr_t*)pos = (uintptr_t)addr_native_to_app(*(uintptr_t*)pos);
                }

                pos += sizeof(uintptr_t);
                break;
            }
            default:
                break;
            }
            might = 0;
        }
    still_might:
        ++format;
    }
}

#define va_list_string2native(exec_env, format, ap) \
    va_list_string2conv(exec_env, format, ap, true)
#define va_list_string2app(exec_env, format, ap) \
    va_list_string2conv(exec_env, format, ap, false)

#ifndef GLUE_FUNCTION_FeatureMalloc
#define GLUE_FUNCTION_FeatureMalloc
uintptr_t glue_FeatureMalloc(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    ret = addr_native_to_app((void*)FeatureMalloc((size_t)parm1, (FeatureType)parm2));
    return ret;
}
#endif /* GLUE_FUNCTION_FeatureMalloc */

#ifndef GLUE_FUNCTION_FeatureDupValue
#define GLUE_FUNCTION_FeatureDupValue
uintptr_t glue_FeatureDupValue(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    ret = addr_native_to_app((void*)FeatureDupValue((void*)parm1));
    return ret;
}
#endif /* GLUE_FUNCTION_FeatureDupValue */

#ifndef GLUE_FUNCTION_FeatureFreeValue
#define GLUE_FUNCTION_FeatureFreeValue
void glue_FeatureFreeValue(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    FeatureFreeValue((void*)parm1);
}
#endif /* GLUE_FUNCTION_FeatureFreeValue */

#ifndef GLUE_FUNCTION_FeatureGetProtoHandle
#define GLUE_FUNCTION_FeatureGetProtoHandle
uintptr_t glue_FeatureGetProtoHandle(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    ret = addr_native_to_app((void*)FeatureGetProtoHandle((void*)parm1));
    return ret;
}
#endif /* GLUE_FUNCTION_FeatureGetProtoHandle */

#ifndef GLUE_FUNCTION_FeatureGetProtoData
#define GLUE_FUNCTION_FeatureGetProtoData
uintptr_t glue_FeatureGetProtoData(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    ret = addr_native_to_app((void*)FeatureGetProtoData((void*)parm1));
    return ret;
}
#endif /* GLUE_FUNCTION_FeatureGetProtoData */

#ifndef GLUE_FUNCTION_FeatureSetProtoData
#define GLUE_FUNCTION_FeatureSetProtoData
void glue_FeatureSetProtoData(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    if ((void*)parm2 == addr_app)
        parm2 = (uintptr_t)NULL;

    FeatureSetProtoData((void*)parm1, (void*)parm2);
}
#endif /* GLUE_FUNCTION_FeatureSetProtoData */

#ifndef GLUE_FUNCTION_FeatureGetObjectData
#define GLUE_FUNCTION_FeatureGetObjectData
uintptr_t glue_FeatureGetObjectData(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    ret = addr_native_to_app((void*)FeatureGetObjectData((void*)parm1));
    return ret;
}
#endif /* GLUE_FUNCTION_FeatureGetObjectData */

#ifndef GLUE_FUNCTION_FeatureSetObjectData
#define GLUE_FUNCTION_FeatureSetObjectData
void glue_FeatureSetObjectData(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    if ((void*)parm2 == addr_app)
        parm2 = (uintptr_t)NULL;

    FeatureSetObjectData((void*)parm1, (void*)parm2);
}
#endif /* GLUE_FUNCTION_FeatureSetObjectData */

#ifndef GLUE_FUNCTION_FeatureRemoveCallback
#define GLUE_FUNCTION_FeatureRemoveCallback
uintptr_t glue_FeatureRemoveCallback(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    ret = FeatureRemoveCallback((void*)parm1, (FtCallbackId)parm2);
    return ret;
}
#endif /* GLUE_FUNCTION_FeatureRemoveCallback */

#ifndef GLUE_FUNCTION_FeaturePromiseReject
#define GLUE_FUNCTION_FeaturePromiseReject
uintptr_t glue_FeaturePromiseReject(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2, uintptr_t parm3, uintptr_t parm4)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    if ((void*)parm4 == addr_app)
        parm4 = (uintptr_t)NULL;

    ret = FeaturePromiseReject((void*)parm1, (FtPromiseId)parm2, (int)parm3, (const char*)parm4);
    return ret;
}
#endif /* GLUE_FUNCTION_FeaturePromiseReject */

#ifndef GLUE_FUNCTION_FeatureGetUVLoop
#define GLUE_FUNCTION_FeatureGetUVLoop
uintptr_t glue_FeatureGetUVLoop(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    ret = addr_native_to_app((void*)FeatureGetUVLoop((void*)parm1));
    return ret;
}
#endif /* GLUE_FUNCTION_FeatureGetUVLoop */

#ifndef GLUE_FUNCTION_FeatureGetManagerHandleFromProto
#define GLUE_FUNCTION_FeatureGetManagerHandleFromProto
uintptr_t glue_FeatureGetManagerHandleFromProto(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    ret = addr_native_to_app((void*)FeatureGetManagerHandleFromProto((void*)parm1));
    return ret;
}
#endif /* GLUE_FUNCTION_FeatureGetManagerHandleFromProto */

#ifndef GLUE_FUNCTION_FeatureDupInstanceHandle
#define GLUE_FUNCTION_FeatureDupInstanceHandle
uintptr_t glue_FeatureDupInstanceHandle(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    ret = addr_native_to_app((void*)FeatureDupInstanceHandle((void*)parm1));
    return ret;
}
#endif /* GLUE_FUNCTION_FeatureDupInstanceHandle */

#ifndef GLUE_FUNCTION_FeatureFreeInstanceHandle
#define GLUE_FUNCTION_FeatureFreeInstanceHandle
void glue_FeatureFreeInstanceHandle(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    FeatureFreeInstanceHandle((void*)parm1);
}
#endif /* GLUE_FUNCTION_FeatureFreeInstanceHandle */

#ifndef GLUE_FUNCTION_FeatureInstanceIsDetached
#define GLUE_FUNCTION_FeatureInstanceIsDetached
uintptr_t glue_FeatureInstanceIsDetached(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    ret = FeatureInstanceIsDetached((void*)parm1);
    return ret;
}
#endif /* GLUE_FUNCTION_FeatureInstanceIsDetached */

#ifndef GLUE_FUNCTION_FeatureGetJsonString
#define GLUE_FUNCTION_FeatureGetJsonString
uintptr_t glue_FeatureGetJsonString(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    ret = addr_native_to_app((void*)FeatureGetJsonString((struct FtJSONObject*)parm1));
    return ret;
}
#endif /* GLUE_FUNCTION_FeatureGetJsonString */

#ifndef native_function
#define native_function(func_name, signature)         \
    {                                                 \
#func_name, glue_##func_name, signature, NULL \
    }
#endif

static NativeSymbol g_feature_native_symbols[] = {
#ifndef GLUE_ENTRY_FeatureMalloc
#define GLUE_ENTRY_FeatureMalloc
    native_function(FeatureMalloc, "(ii)i"),
#endif /* GLUE_ENTRY_FeatureMalloc */
#ifndef GLUE_ENTRY_FeatureDupValue
#define GLUE_ENTRY_FeatureDupValue
    native_function(FeatureDupValue, "(*)i"),
#endif /* GLUE_ENTRY_FeatureDupValue */
#ifndef GLUE_ENTRY_FeatureFreeValue
#define GLUE_ENTRY_FeatureFreeValue
    native_function(FeatureFreeValue, "(*)"),
#endif /* GLUE_ENTRY_FeatureFreeValue */
#ifndef GLUE_ENTRY_FeatureGetProtoHandle
#define GLUE_ENTRY_FeatureGetProtoHandle
    native_function(FeatureGetProtoHandle, "(*)i"),
#endif /* GLUE_ENTRY_FeatureGetProtoHandle */
#ifndef GLUE_ENTRY_FeatureGetProtoData
#define GLUE_ENTRY_FeatureGetProtoData
    native_function(FeatureGetProtoData, "(*)i"),
#endif /* GLUE_ENTRY_FeatureGetProtoData */
#ifndef GLUE_ENTRY_FeatureSetProtoData
#define GLUE_ENTRY_FeatureSetProtoData
    native_function(FeatureSetProtoData, "(**)"),
#endif /* GLUE_ENTRY_FeatureSetProtoData */
#ifndef GLUE_ENTRY_FeatureGetObjectData
#define GLUE_ENTRY_FeatureGetObjectData
    native_function(FeatureGetObjectData, "(*)i"),
#endif /* GLUE_ENTRY_FeatureGetObjectData */
#ifndef GLUE_ENTRY_FeatureSetObjectData
#define GLUE_ENTRY_FeatureSetObjectData
    native_function(FeatureSetObjectData, "(**)"),
#endif /* GLUE_ENTRY_FeatureSetObjectData */
#ifndef GLUE_ENTRY_FeatureRemoveCallback
#define GLUE_ENTRY_FeatureRemoveCallback
    native_function(FeatureRemoveCallback, "(*i)i"),
#endif /* GLUE_ENTRY_FeatureRemoveCallback */
#ifndef GLUE_ENTRY_FeaturePromiseReject
#define GLUE_ENTRY_FeaturePromiseReject
    native_function(FeaturePromiseReject, "(*ii$)i"),
#endif /* GLUE_ENTRY_FeaturePromiseReject */
#ifndef GLUE_ENTRY_FeatureGetUVLoop
#define GLUE_ENTRY_FeatureGetUVLoop
    native_function(FeatureGetUVLoop, "(*)i"),
#endif /* GLUE_ENTRY_FeatureGetUVLoop */
#ifndef GLUE_ENTRY_FeatureGetManagerHandleFromProto
#define GLUE_ENTRY_FeatureGetManagerHandleFromProto
    native_function(FeatureGetManagerHandleFromProto, "(*)i"),
#endif /* GLUE_ENTRY_FeatureGetManagerHandleFromProto */
#ifndef GLUE_ENTRY_FeatureDupInstanceHandle
#define GLUE_ENTRY_FeatureDupInstanceHandle
    native_function(FeatureDupInstanceHandle, "(*)i"),
#endif /* GLUE_ENTRY_FeatureDupInstanceHandle */
#ifndef GLUE_ENTRY_FeatureFreeInstanceHandle
#define GLUE_ENTRY_FeatureFreeInstanceHandle
    native_function(FeatureFreeInstanceHandle, "(*)"),
#endif /* GLUE_ENTRY_FeatureFreeInstanceHandle */
#ifndef GLUE_ENTRY_FeatureInstanceIsDetached
#define GLUE_ENTRY_FeatureInstanceIsDetached
    native_function(FeatureInstanceIsDetached, "(*)i"),
#endif /* GLUE_ENTRY_FeatureInstanceIsDetached */
#ifndef GLUE_ENTRY_FeatureGetJsonString
#define GLUE_ENTRY_FeatureGetJsonString
    native_function(FeatureGetJsonString, "(*)i"),
#endif /* GLUE_ENTRY_FeatureGetJsonString */
};

bool wamr_module_feature_wrapper_register(void)
{
    bool ret;

    /* Add extra init hook here */

    ret = wasm_runtime_register_natives("env", g_feature_native_symbols, nitems(g_feature_native_symbols));
    if (!ret) {
        return ret;
    }

    return ret;
}
