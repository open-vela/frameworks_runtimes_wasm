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
#include "include/event_wrapper.h"
#include <map>

extern "C" {
typedef struct mapInternalEventData {
    wasm_exec_env_t execEnv;
    uint32_t originFuncOffset;
} mapInternalEventData;
static std::map<void*, mapInternalEventData> eventDataMap;
void freeFunc(uint16_t eventType, void* eventData)
{
    auto item = eventDataMap.find(eventData);
    uint64_t offset = 0;
    uint32_t argv[2];
    if (item == eventDataMap.end() || !item->second.originFuncOffset) {
        return;
    }
    wasm_module_inst_t wasmModuleInst = get_module_inst(item->second.execEnv);
    offset = wasm_runtime_addr_native_to_app(wasmModuleInst, eventData);
    argv[0] = (uint32_t)eventType;
    argv[1] = (uint32_t)offset;
    if (!wasm_runtime_call_indirect(item->second.execEnv, item->second.originFuncOffset, 2, argv)) {
        printf("Calling callback function in WASM for freeFunc failed! Error: %s", wasm_runtime_get_exception(wasmModuleInst));
    }
    eventDataMap.erase(item);
}
bool chreSendEventWrapper(wasm_exec_env_t execEnv, uint16_t eventType,
    void* eventData,
    uint32_t funcOffset,
    uint32_t targetInstanceId)
{
    // mapping data
    eventDataMap[eventData] = {
        execEnv,
        funcOffset,
    };
    return chreSendEvent(eventType, eventData, freeFunc, targetInstanceId);
}
bool chreSendMessageToHostWrapper(wasm_exec_env_t execEnv, void* message,
    uint32_t messageSize, uint32_t messageType,
    uint32_t funcOffset)
{
    return chreSendMessageWithPermissionsWrapper(execEnv, message, messageSize, messageType,
        CHRE_HOST_ENDPOINT_BROADCAST, static_cast<uint32_t>(NanoappPermissions::CHRE_PERMS_NONE), funcOffset);
}
bool chreSendMessageToHostEndpointWrapper(wasm_exec_env_t execEnv, void* message, uint32_t messageSize,
    uint32_t messageType, uint16_t hostEndpoint,
    uint32_t funcOffset)
{
    return chreSendMessageWithPermissionsWrapper(execEnv, message, messageSize, messageType, hostEndpoint,
        static_cast<uint32_t>(NanoappPermissions::CHRE_PERMS_NONE),
        funcOffset);
}
typedef struct mapInternalMessageData {
    wasm_exec_env_t execEnv;
    uint32_t originFuncOffset;
} mapInternalMessageData;
static std::map<void*, mapInternalMessageData> messageDataMap;
static void messageFreeFunc(void* message, size_t messageSize)
{
    auto item = messageDataMap.find(message);
    uint64_t offset = 0;
    uint32_t argv[2];
    if (messageDataMap.end() == item || !item->second.originFuncOffset) {
        return;
    }
    wasm_module_inst_t wasmModuleInst = get_module_inst(item->second.execEnv);
    offset = wasm_runtime_addr_native_to_app(wasmModuleInst, message);
    argv[0] = (uint32_t)offset;
    argv[1] = (uint32_t)messageSize;
    if (!wasm_runtime_call_indirect(item->second.execEnv, item->second.originFuncOffset, 2, argv)) {
        printf("Calling callback function in WASM for message failed! Error: %s", wasm_runtime_get_exception(wasmModuleInst));
    }
    messageDataMap.erase(item);
}
bool chreSendMessageWithPermissionsWrapper(wasm_exec_env_t execEnv, void* message, uint32_t messageSize,
    uint32_t messageType, uint16_t hostEndpoint,
    uint32_t messagePermissions,
    uint32_t funcOffset)
{
    messageDataMap[message] = {
        execEnv,
        funcOffset
    };
    return chreSendMessageWithPermissions(message, messageSize, messageType, hostEndpoint,
        messagePermissions, messageFreeFunc);
}
bool chreGetNanoappInfoByAppIdWrapper(wasm_exec_env_t execEnv, uint64_t appId,
    chreNanoappInfo* info)
{
    (void)execEnv;
    return chreGetNanoappInfoByAppId(appId, static_cast<chreNanoappInfo*>(info));
}
bool chreGetNanoappInfoByInstanceIdWrapper(wasm_exec_env_t execEnv, uint32_t instanceId,
    chreNanoappInfo* info)
{
    (void)execEnv;
    return chreGetNanoappInfoByInstanceId(instanceId, info);
}
void chreConfigureNanoappInfoEventsWrapper(wasm_exec_env_t execEnv, bool enable)
{
    (void)execEnv;
    return chreConfigureNanoappInfoEvents(enable);
}
void chreConfigureHostSleepStateEventsWrapper(wasm_exec_env_t execEnv, bool enable)
{
    (void)execEnv;
    return chreConfigureHostSleepStateEvents(enable);
}
bool chreIsHostAwakeWrapper(wasm_exec_env_t execEnv)
{
    (void)execEnv;
    return chreIsHostAwake();
}
void chreConfigureDebugDumpEventWrapper(wasm_exec_env_t execEnv, bool enable)
{
    (void)execEnv;
    return chreConfigureDebugDumpEvent(enable);
}
bool chreConfigureHostEndpointNotificationsWrapper(wasm_exec_env_t execEnv, uint16_t hostEndpointId,
    bool enable)
{
    (void)execEnv;
    return chreConfigureHostEndpointNotifications(hostEndpointId, enable);
}
bool chrePublishRpcServicesWrapper(wasm_exec_env_t execEnv, chreNanoappRpcService* services,
    size_t numServices)
{
    (void)execEnv;
    return chrePublishRpcServices(services, numServices);
}
bool chreGetHostEndpointInfoWrapper(wasm_exec_env_t execEnv, uint16_t hostEndpointId,
    chreHostEndpointInfo* info)
{
    (void)execEnv;
    return chreGetHostEndpointInfo(hostEndpointId, info);
}
}
