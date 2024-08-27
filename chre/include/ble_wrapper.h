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

#ifndef _BLE_WRAPPER_H_
#define _BLE_WRAPPER_H_

#include "chre/ble.h"
#include "wasm_export.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t chreBleGetCapabilitiesWrapper(wasm_exec_env_t execEnv);

uint32_t chreBleGetFilterCapabilitiesWrapper(wasm_exec_env_t execEnv);

bool chreBleStartScanAsyncWrapper(wasm_exec_env_t execEnv, enum chreBleScanMode mode,
    uint32_t reportDelayMs, const chreBleScanFilter* filter);

bool chreBleStopScanAsyncWrapper(wasm_exec_env_t execEnv);

#ifdef __cplusplus
}
#endif

#endif
