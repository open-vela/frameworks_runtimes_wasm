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

#include "include/wifi_wrapper.h"

extern "C" {

uint32_t chreWifiGetCapabilitiesWrapper(wasm_exec_env_t execEnv)
{
    (void)execEnv;
    return chreWifiGetCapabilities();
}

bool chreWifiNanGetCapabilitiesWrapper(wasm_exec_env_t execEnv, struct chreWifiNanCapabilities* capabilities)
{
    (void)execEnv;
    (void)capabilities;
    return false;
}

bool chreWifiConfigureScanMonitorAsyncWrapper(wasm_exec_env_t execEnv, bool enable, const void* cookie)
{
    (void)execEnv;
    return chreWifiConfigureScanMonitorAsync(enable, cookie);
}

bool chreWifiRequestScanAsyncWrapper(wasm_exec_env_t execEnv, uint32_t offset_params, const void* cookie)
{
    struct chreWifiScanParams nativeParams;
    wasm_module_inst_t wasmModuleInst = get_module_inst(execEnv);
    if (!wasm_runtime_validate_app_addr(wasmModuleInst, offset_params, sizeof(struct chreWifiScanParams))) {
        return false;
    }
    nativeParams = *(reinterpret_cast<struct chreWifiScanParams*>(
        wasm_runtime_addr_app_to_native(wasmModuleInst, offset_params)));
    if (!wasm_runtime_validate_app_addr(wasmModuleInst, reinterpret_cast<uint64_t>(nativeParams.frequencyList),
            nativeParams.frequencyListLen * sizeof(*nativeParams.frequencyList))) {
        return false;
    }
    if (!wasm_runtime_validate_app_addr(wasmModuleInst, reinterpret_cast<uint64_t>(nativeParams.ssidList),
            nativeParams.ssidListLen * sizeof(*nativeParams.ssidList))) {
        return false;
    }
    nativeParams.frequencyList = reinterpret_cast<const uint32_t*>(
        wasm_runtime_addr_app_to_native(wasmModuleInst, reinterpret_cast<uint64_t>(nativeParams.frequencyList)));
    nativeParams.ssidList = reinterpret_cast<const struct chreWifiSsidListItem*>(
        wasm_runtime_addr_app_to_native(wasmModuleInst, reinterpret_cast<uint64_t>(nativeParams.ssidList)));
    return chreWifiRequestScanAsync(&nativeParams, cookie);
}

bool chreWifiRequestRangingAsyncWrapper(wasm_exec_env_t execEnv, uint32_t offset_params,
    const void* cookie)
{
    struct chreWifiRangingParams nativeParams;
    wasm_module_inst_t wasmModuleInst = get_module_inst(execEnv);
    if (!wasm_runtime_validate_app_addr(wasmModuleInst, offset_params, sizeof(struct chreWifiRangingParams))) {
        return false;
    }
    nativeParams = *(reinterpret_cast<struct chreWifiRangingParams*>(wasm_runtime_addr_app_to_native(wasmModuleInst, offset_params)));
    if (!wasm_runtime_validate_app_addr(wasmModuleInst, reinterpret_cast<uint64_t>(nativeParams.targetList),
            nativeParams.targetListLen * sizeof(*nativeParams.targetList))) {
        return false;
    }
    nativeParams.targetList = reinterpret_cast<const struct chreWifiRangingTarget*>(
        wasm_runtime_addr_app_to_native(wasmModuleInst, reinterpret_cast<uint64_t>(nativeParams.targetList)));
    return chreWifiRequestRangingAsync(&nativeParams, cookie);
}

bool chreWifiNanSubscribeWrapper(wasm_exec_env_t execEnv, struct chreWifiNanSubscribeConfig* config,
    const void* cookie)
{
    char* service = NULL;
    uint8_t* serviceSpecificInfo = NULL;
    uint8_t* matchFilter = NULL;

    uint64_t serviceOffset = 0;
    uint64_t serviceSpecificInfoOffset = 0;
    uint64_t matchFilterOffset = 0;
    uint64_t serviceLength = 0;

    wasm_module_inst_t wasmModuleInst = get_module_inst(execEnv);
    if (!wasm_runtime_validate_native_addr(wasmModuleInst, config, (uint64_t)sizeof(struct chreWifiNanSubscribeConfig)))
        return 0;

    bool success = chreWifiNanSubscribe(config, cookie);

    if (success) {
        serviceLength = strlen(config->service) + 1;
        serviceOffset = wasm_runtime_module_malloc(wasmModuleInst, serviceLength + 1,
            reinterpret_cast<void**>(&service));
        success = static_cast<bool>(serviceOffset);
        if (success) {
            memcpy(service, config->service, serviceLength);
            config->service = reinterpret_cast<const char*>(serviceOffset);
        }

        serviceSpecificInfoOffset = wasm_runtime_module_malloc(wasmModuleInst,
            config->serviceSpecificInfoSize,
            reinterpret_cast<void**>(&serviceSpecificInfo));
        success = static_cast<bool>(serviceSpecificInfoOffset);
        if (success) {
            memcpy(serviceSpecificInfo, config->serviceSpecificInfo, config->serviceSpecificInfoSize);
            config->serviceSpecificInfo = reinterpret_cast<const uint8_t*>(serviceSpecificInfoOffset);
        }

        matchFilterOffset = wasm_runtime_module_malloc(wasmModuleInst,
            config->matchFilterLength,
            reinterpret_cast<void**>(&matchFilter));
        success = static_cast<bool>(matchFilterOffset);
        if (success) {
            memcpy(matchFilter, config->matchFilter, config->matchFilterLength);
            config->matchFilter = reinterpret_cast<const uint8_t*>(matchFilterOffset);
        }
    }
    return success;
}

bool chreWifiNanSubscribeCancelWrapper(wasm_exec_env_t execEnv, uint32_t subscriptionID)
{
    (void)execEnv;
    return chreWifiNanSubscribeCancel(subscriptionID);
}

bool chreWifiNanRequestRangingAsyncWrapper(wasm_exec_env_t execEnv, const struct chreWifiNanRangingParams* params,
    const void* cookie)
{
    (void)execEnv;
    return chreWifiNanRequestRangingAsync(params, cookie);
}
}
