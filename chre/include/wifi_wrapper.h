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

#ifndef _WIFI_WRAPPER_H_
#define _WIFI_WRAPPER_H_

#include "chre/wifi.h"
#include "wasm_export.h"

extern "C" {

uint32_t chreWifiGetCapabilitiesWrapper(wasm_exec_env_t execEnv);

bool chreWifiNanGetCapabilitiesWrapper(wasm_exec_env_t execEnv, struct chreWifiNanCapabilities* capabilities);

bool chreWifiConfigureScanMonitorAsyncWrapper(wasm_exec_env_t execEnv, bool enable, const void* cookie);

bool chreWifiRequestScanAsyncWrapper(wasm_exec_env_t execEnv, uint32_t params, const void* cookie);

bool chreWifiRequestRangingAsyncWrapper(wasm_exec_env_t execEnv, uint32_t params, const void* cookie);

bool chreWifiNanSubscribeWrapper(wasm_exec_env_t execEnv, struct chreWifiNanSubscribeConfig* config,
    const void* cookie);

bool chreWifiNanSubscribeCancelWrapper(wasm_exec_env_t execEnv, uint32_t subscriptionID);

bool chreWifiNanRequestRangingAsyncWrapper(wasm_exec_env_t execEnv, const struct chreWifiNanRangingParams* params,
    const void* cookie);
}

#endif
