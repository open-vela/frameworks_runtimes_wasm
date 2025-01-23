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

#ifndef _SENSOR_WRAPPER_H_
#define _SENSOR_WRAPPER_H_

#include "chre/sensor.h"
#include "chre/sensor_types.h"
#include "wasm_export.h"

extern "C" {

bool chreSensorFindDefaultWrapper(wasm_exec_env_t execEnv, uint8_t sensorType, uint32_t* handle);

bool chreSensorFindWrapper(wasm_exec_env_t execEnv, uint8_t sensorType, uint8_t sensorIndex, uint32_t* handle);

bool chreGetSensorInfoWrapper(wasm_exec_env_t execEnv, uint32_t sensorHandle, struct chreSensorInfo* info);

bool chreGetSensorSamplingStatusWrapper(wasm_exec_env_t execEnv, uint32_t sensorHandle,
    struct chreSensorSamplingStatus* status);

bool chreSensorConfigureWrapper(wasm_exec_env_t execEnv, uint32_t sensorHandle,
    enum chreSensorConfigureMode mode,
    uint64_t interval, uint64_t latency);

bool chreSensorConfigureBiasEventsWrapper(wasm_exec_env_t execEnv, uint32_t sensorHandle, bool enable);

bool chreSensorGetThreeAxisBiasWrapper(wasm_exec_env_t execEnv, uint32_t sensorHandle,
    struct chreSensorThreeAxisData* bias);

bool chreSensorFlushAsyncWrapper(wasm_exec_env_t execEnv, uint32_t sensorHandle, const void* cookie);
}

#endif
