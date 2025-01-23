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

#ifndef _EVENT_WRAPPER_H_
#define _EVENT_WRAPPER_H_

#include "chre/event.h"
#include "wasm_export.h"

/**
 * Enum declaring the various CHRE permissions that can be declared. Nanoapps
 * built against CHRE API v1.5+ must contain the respective permission for the
 * set of APIs they attempt to call. For example, CHRE_NANOAPP_USES_WIFI must
 * be declared by the nanoapp in order for it to make use of any WiFi APIs.
 *
 * The 8 most-significant bits (MSBs) are reserved for vendor use and must be
 * used if a vendor API allows access to privacy sensitive information that is
 * guarded by a permission on the Android side (e.g. location).
 *
 * This should keep same with chre/util/include/chre/util/system/napp_permissions.h
 */

extern "C" {

enum NanoappPermissions : uint32_t {
    CHRE_PERMS_NONE = 0,
    CHRE_PERMS_AUDIO = 1,
    CHRE_PERMS_GNSS = 1 << 1,
    CHRE_PERMS_WIFI = 1 << 2,
    CHRE_PERMS_WWAN = 1 << 3,
    CHRE_PERMS_BLE = 1 << 4,
    CHRE_PERMS_ALL = 0xffffffff,
};

bool chreSendEventWrapper(wasm_exec_env_t execEnv, uint16_t eventType,
    void* eventData, uint32_t funcOffset,
    uint32_t targetInstanceId);

bool chreSendMessageToHostWrapper(wasm_exec_env_t execEnv, void* message,
    uint32_t messageSize, uint32_t messageType,
    uint32_t funcOffset);

bool chreSendMessageToHostEndpointWrapper(wasm_exec_env_t execEnv, void* message, uint32_t messageSize,
    uint32_t messageType, uint16_t hostEndpoint,
    uint32_t funcOffset);

bool chreSendMessageWithPermissionsWrapper(wasm_exec_env_t execEnv, void* message, uint32_t messageSize,
    uint32_t messageType, uint16_t hostEndpoint,
    uint32_t messagePermissions,
    uint32_t funcOffset);

bool chreGetNanoappInfoByAppIdWrapper(wasm_exec_env_t execEnv, uint64_t appId, chreNanoappInfo* info);

bool chreGetNanoappInfoByInstanceIdWrapper(wasm_exec_env_t execEnv, uint32_t instanceId, chreNanoappInfo* info);

void chreConfigureNanoappInfoEventsWrapper(wasm_exec_env_t execEnv, bool enable);

void chreConfigureHostSleepStateEventsWrapper(wasm_exec_env_t execEnv, bool enable);

bool chreIsHostAwakeWrapper(wasm_exec_env_t execEnv);

void chreConfigureDebugDumpEventWrapper(wasm_exec_env_t execEnv, bool enable);

bool chreConfigureHostEndpointNotificationsWrapper(wasm_exec_env_t execEnv, uint16_t hostEndpointId, bool enable);

bool chrePublishRpcServicesWrapper(wasm_exec_env_t execEnv, chreNanoappRpcService* services, size_t numServices);

bool chreGetHostEndpointInfoWrapper(wasm_exec_env_t execEnv, uint16_t hostEndpointId, chreHostEndpointInfo* info);
}

#endif
