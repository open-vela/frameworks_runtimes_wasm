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

#include "include/sensor_wrapper.h"

extern "C" {

bool chreSensorFindDefaultWrapper(wasm_exec_env_t execEnv, uint8_t sensorType, uint32_t* handle)
{
    (void)execEnv;
    return chreSensorFindDefault(sensorType, handle);
}

bool chreSensorFindWrapper(wasm_exec_env_t execEnv, uint8_t sensorType, uint8_t sensorIndex, uint32_t* handle)
{
    (void)execEnv;
    return chreSensorFind(sensorType, sensorIndex, handle);
}

bool chreGetSensorInfoWrapper(wasm_exec_env_t execEnv, uint32_t sensorHandle, struct chreSensorInfo* info)
{
    uint64_t offsetSensorName = 0;
    uint64_t lengthSensorName = 0;
    char* pointerSensorName = NULL;

    wasm_module_inst_t wasmModuleInst = get_module_inst(execEnv);
    if (!wasm_runtime_validate_native_addr(wasmModuleInst, info, (uint64_t)sizeof(struct chreSensorInfo)))
        return 0;

    bool success = chreGetSensorInfo(sensorHandle, info);
    if (success) {
        lengthSensorName = strlen(info->sensorName) + 1;
        offsetSensorName = wasm_runtime_module_malloc(wasmModuleInst, lengthSensorName + 1,
            reinterpret_cast<void**>(&pointerSensorName));
        success = static_cast<bool>(offsetSensorName);
        if (success) {
            memcpy(pointerSensorName, info->sensorName, lengthSensorName);
            info->sensorName = reinterpret_cast<const char*>(offsetSensorName);
        }
    }
    return success;
}

bool chreGetSensorSamplingStatusWrapper(wasm_exec_env_t execEnv, uint32_t sensorHandle,
    struct chreSensorSamplingStatus* status)
{
    (void)execEnv;
    return chreGetSensorSamplingStatus(sensorHandle, status);
}

bool chreSensorConfigureWrapper(wasm_exec_env_t execEnv, uint32_t sensorHandle,
    enum chreSensorConfigureMode mode,
    uint64_t interval, uint64_t latency)
{
    (void)execEnv;
    return chreSensorConfigure(sensorHandle, mode, interval, latency);
}

bool chreSensorConfigureBiasEventsWrapper(wasm_exec_env_t execEnv, uint32_t sensorHandle, bool enable)
{
    (void)execEnv;
    return chreSensorConfigureBiasEvents(sensorHandle, enable);
}

bool chreSensorGetThreeAxisBiasWrapper(wasm_exec_env_t execEnv, uint32_t sensorHandle,
    struct chreSensorThreeAxisData* bias)
{
    (void)execEnv;
    return chreSensorGetThreeAxisBias(sensorHandle, bias);
}

bool chreSensorFlushAsyncWrapper(wasm_exec_env_t execEnv, uint32_t sensorHandle, const void* cookie)
{
    (void)execEnv;
    return chreSensorFlushAsync(sensorHandle, cookie);
}
}
