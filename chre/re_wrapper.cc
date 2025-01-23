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
#include "include/re_wrapper.h"

extern "C" {

uint64_t chreGetAppIdWrapper(wasm_exec_env_t execEnv)
{
    (void)execEnv;
    return chreGetAppId();
}
uint32_t chreGetInstanceIdWrapper(wasm_exec_env_t execEnv)
{
    (void)execEnv;
    return chreGetInstanceId();
}
void chreLogWrapper(wasm_exec_env_t execEnv, enum chreLogLevel level, const char* formatStr, const char* data)
{
    (void)execEnv;
    chreLog(level, formatStr, data);
}
uint64_t chreGetTimeWrapper(wasm_exec_env_t execEnv)
{
    (void)execEnv;
    return chreGetTime();
}
uint64_t chreGetEstimatedHostTimeWrapper(wasm_exec_env_t execEnv)
{
    (void)execEnv;
    return chreGetEstimatedHostTime();
}
int64_t chreGetEstimatedHostTimeOffsetWrapper(wasm_exec_env_t execEnv)
{
    (void)execEnv;
    return chreGetEstimatedHostTimeOffset();
}
uint32_t chreTimerSetWrapper(wasm_exec_env_t execEnv, uint64_t duration, const void* cookie, bool oneShot)
{
    (void)execEnv;
    return chreTimerSet(duration, cookie, oneShot);
}
bool chreTimerCancelWrapper(wasm_exec_env_t execEnv, uint32_t timerId)
{
    (void)execEnv;
    return chreTimerCancel(timerId);
}
void chreAbortWrapper(wasm_exec_env_t execEnv, uint32_t abortCode)
{
    (void)execEnv;
    chreAbort(abortCode);
}
uint64_t chreHeapAllocWrapper(wasm_exec_env_t execEnv, uint32_t bytes)
{
    wasm_module_inst_t wasmModuleInst = get_module_inst(execEnv);
    return wasm_runtime_module_malloc(wasmModuleInst, (uint64_t)bytes, NULL);
}
void chreHeapFreeWrapper(wasm_exec_env_t execEnv, uint64_t ptr)
{
    wasm_module_inst_t wasmModuleInst = get_module_inst(execEnv);
    wasm_runtime_module_free(wasmModuleInst, ptr);
}
void chreDebugDumpLogWrapper(wasm_exec_env_t execEnv, const char* formatStr, _va_list va_args)
{
    // TODO
}
}
