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

#include "include/ble_wrapper.h"

uint32_t chreBleGetCapabilitiesWrapper(wasm_exec_env_t execEnv)
{
    return chreBleGetCapabilities();
}

uint32_t chreBleGetFilterCapabilitiesWrapper(wasm_exec_env_t execEnv)
{
    return chreBleGetFilterCapabilities();
}

bool chreBleStartScanAsyncWrapper(wasm_exec_env_t execEnv, enum chreBleScanMode mode,
    uint32_t reportDelayMs, const struct chreBleScanFilter* filter)
{
    struct chreBleScanFilter nativeFilter;
    if (!filter) {
        return chreBleStartScanAsync(mode, reportDelayMs, nullptr);
    }
    nativeFilter.rssiThreshold = filter->rssiThreshold;
    nativeFilter.scanFilterCount = filter->scanFilterCount;
    wasm_module_inst_t wasmModuleInst = wasm_runtime_get_module_inst(execEnv);
    if (!wasmModuleInst) {
        goto fail;
    }
    nativeFilter.scanFilters = static_cast<chreBleGenericFilter*>(
        wasm_runtime_addr_app_to_native(wasmModuleInst, reinterpret_cast<uint64_t>(filter->scanFilters)));
    return chreBleStartScanAsync(mode, reportDelayMs, &nativeFilter);
fail:
    return false;
}

bool chreBleStopScanAsyncWrapper(wasm_exec_env_t execEnv)
{
    return chreBleStopScanAsync();
}
