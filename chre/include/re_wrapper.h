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

#ifndef _RE_WRAPPER_H_
#define _RE_WRAPPER_H_

#include "chre/re.h"
#include "wasm_export.h"

extern "C" {

typedef char* _va_list;

uint64_t chreGetAppIdWrapper(wasm_exec_env_t execEnv);

uint32_t chreGetInstanceIdWrapper(wasm_exec_env_t execEnv);

void chreLogWrapper(wasm_exec_env_t execEnv, enum chreLogLevel level, const char* formatStr, const char* data);

uint64_t chreGetTimeWrapper(wasm_exec_env_t execEnv);

uint64_t chreGetEstimatedHostTimeWrapper(wasm_exec_env_t execEnv);

int64_t chreGetEstimatedHostTimeOffsetWrapper(wasm_exec_env_t execEnv);

uint32_t chreTimerSetWrapper(wasm_exec_env_t execEnv, uint64_t duration, const void* cookie, bool oneShot);

bool chreTimerCancelWrapper(wasm_exec_env_t execEnv, uint32_t timerId);

void chreAbortWrapper(wasm_exec_env_t execEnv, uint32_t abortCode);

uint64_t chreHeapAllocWrapper(wasm_exec_env_t execEnv, uint32_t bytes);

void chreHeapFreeWrapper(wasm_exec_env_t execEnv, uint64_t ptr);

void chreDebugDumpLogWrapper(wasm_exec_env_t execEnv, const char* formatStr, _va_list va_args);
}

#endif
