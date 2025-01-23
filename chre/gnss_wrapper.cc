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

#include "include/gnss_wrapper.h"

extern "C" {

uint32_t chreGnssGetCapabilitiesWrapper(wasm_exec_env_t execEnv)
{
    (void)execEnv;
    return chreGnssGetCapabilities();
}

bool chreGnssLocationSessionStartAsyncWrapper(wasm_exec_env_t execEnv, uint32_t minIntervalMs,
    uint32_t minTimeToNextFixMs, const void* cookie)
{
    (void)execEnv;
    return chreGnssLocationSessionStartAsync(minIntervalMs, minTimeToNextFixMs, cookie);
}

bool chreGnssLocationSessionStopAsyncWrapper(wasm_exec_env_t execEnv, const void* cookie)
{
    (void)execEnv;
    return chreGnssLocationSessionStopAsync(cookie);
}

bool chreGnssMeasurementSessionStartAsyncWrapper(wasm_exec_env_t execEnv, uint32_t minIntervalMs,
    const void* cookie)
{
    (void)execEnv;
    return chreGnssMeasurementSessionStartAsync(minIntervalMs, cookie);
}

bool chreGnssMeasurementSessionStopAsyncWrapper(wasm_exec_env_t execEnv, const void* cookie)
{
    (void)execEnv;
    return chreGnssMeasurementSessionStopAsync(cookie);
}

bool chreGnssConfigurePassiveLocationListenerWrapper(wasm_exec_env_t execEnv, bool enable)
{
    (void)execEnv;
    return chreGnssConfigurePassiveLocationListener(enable);
}
}
