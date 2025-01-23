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

#include <cstring>

#include "include/audio_wrapper.h"

extern "C" {

bool chreAudioGetSourceWrapper(wasm_exec_env_t execEnv, uint32_t handle,
    struct chreAudioSource* audioSource)
{
    uint64_t nameOffset = 0;
    char* name = NULL;
    uint64_t nameLength = 0;

    wasm_module_inst_t wasmModuleInst = get_module_inst(execEnv);
    if (!wasm_runtime_validate_native_addr(wasmModuleInst, audioSource,
            (uint64_t)sizeof(struct chreAudioSource)))
        return 0;

    bool success = chreAudioGetSource(handle, audioSource);
    if (success) {
        nameLength = strlen(audioSource->name) + 1;
        nameOffset = wasm_runtime_module_malloc(wasmModuleInst, nameLength + 1,
            reinterpret_cast<void**>(&name));
        success = static_cast<bool>(nameOffset);
        if (success) {
            memcpy(name, audioSource->name, nameLength);
            audioSource->name = reinterpret_cast<const char*>(nameOffset);
        }
    }
    return success;
}

bool chreAudioConfigureSourceWrapper(wasm_exec_env_t execEnv,
    uint32_t handle, bool enable,
    uint64_t bufferDuration,
    uint64_t deliveryInterval)
{
    (void)execEnv;
    return chreAudioConfigureSource(handle, enable, bufferDuration, deliveryInterval);
}

bool chreAudioGetStatusWrapper(wasm_exec_env_t execEnv, uint32_t handle,
    chreAudioSourceStatus* status)
{
    (void)execEnv;
    (void)handle;
    (void)status;
    return false;
}
}
