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

//! audio.h begin
REG_CHRE_NATIVE_FUNC(chreAudioGetSource, "(i*)i"),
REG_CHRE_NATIVE_FUNC(chreAudioConfigureSource, "(iiII)i"),
//! audio.h end

//! ble.h begin
REG_CHRE_NATIVE_FUNC(chreBleGetCapabilities, "()i"),
REG_CHRE_NATIVE_FUNC(chreBleGetFilterCapabilities, "()i"),
REG_CHRE_NATIVE_FUNC(chreBleStartScanAsync, "(ii*)i"),
REG_CHRE_NATIVE_FUNC(chreBleStopScanAsync, "()i"),
//! ble.h end

//event.h begin
REG_CHRE_NATIVE_FUNC(chreSendEvent, "(i*ii)i"),
REG_CHRE_NATIVE_FUNC(chreSendMessageToHost, "(*~ii)i"),
REG_CHRE_NATIVE_FUNC(chreSendMessageToHostEndpoint, "(*~iii)i"),
REG_CHRE_NATIVE_FUNC(chreSendMessageWithPermissions, "(*~iiii)i"),
REG_CHRE_NATIVE_FUNC(chreGetNanoappInfoByAppId, "(I*)i"),
REG_CHRE_NATIVE_FUNC(chreGetNanoappInfoByInstanceId, "(i*)i"),
REG_CHRE_NATIVE_FUNC(chreConfigureNanoappInfoEvents, "(i)"),
REG_CHRE_NATIVE_FUNC(chreConfigureHostSleepStateEvents, "(i)"),
REG_CHRE_NATIVE_FUNC(chreIsHostAwake, "()i"),
REG_CHRE_NATIVE_FUNC(chreConfigureDebugDumpEvent, "(i)"),
REG_CHRE_NATIVE_FUNC(chreConfigureHostEndpointNotifications, "(ii)i"),
REG_CHRE_NATIVE_FUNC(chrePublishRpcServices, "(*i)i"),
REG_CHRE_NATIVE_FUNC(chreGetHostEndpointInfo, "(i*)i"),
//! event.h end

//! gnss.h begin
REG_CHRE_NATIVE_FUNC(chreGnssGetCapabilities, "()i"),
REG_CHRE_NATIVE_FUNC(chreGnssLocationSessionStartAsync, "(ii*)i"),
REG_CHRE_NATIVE_FUNC(chreGnssLocationSessionStopAsync, "(*)i"),
REG_CHRE_NATIVE_FUNC(chreGnssMeasurementSessionStartAsync, "(i*)i"),
REG_CHRE_NATIVE_FUNC(chreGnssMeasurementSessionStopAsync, "(*)i"),
REG_CHRE_NATIVE_FUNC(chreGnssConfigurePassiveLocationListener, "(i)i"),
//! gnss.h end

/* re.h begin */
REG_CHRE_NATIVE_FUNC(chreGetAppId, "()I"),
REG_CHRE_NATIVE_FUNC(chreGetInstanceId, "()i"),
REG_CHRE_NATIVE_FUNC(chreLog, "(i$*)"),
REG_CHRE_NATIVE_FUNC(chreGetTime, "()I"),
REG_CHRE_NATIVE_FUNC(chreGetEstimatedHostTimeOffset, "()I"),
REG_CHRE_NATIVE_FUNC(chreGetEstimatedHostTime, "()I"),
REG_CHRE_NATIVE_FUNC(chreTimerSet, "(I*i)i"),
REG_CHRE_NATIVE_FUNC(chreTimerCancel, "(i)i"),
REG_CHRE_NATIVE_FUNC(chreAbort, "(i)"),
REG_CHRE_NATIVE_FUNC(chreHeapAlloc, "(i)i"),
REG_CHRE_NATIVE_FUNC(chreHeapFree, "(i)"),
REG_CHRE_NATIVE_FUNC(chreDebugDumpLog, "($*)"),
/* re.h end */

//! sensor.h begin
REG_CHRE_NATIVE_FUNC(chreSensorFindDefault, "(i*)i"),
REG_CHRE_NATIVE_FUNC(chreSensorFind, "(ii*)i"),
REG_CHRE_NATIVE_FUNC(chreGetSensorInfo, "(i*)i"),
REG_CHRE_NATIVE_FUNC(chreGetSensorSamplingStatus, "(i*)i"),
REG_CHRE_NATIVE_FUNC(chreSensorConfigure, "(iiII)i"),
REG_CHRE_NATIVE_FUNC(chreSensorConfigureBiasEvents, "(ii)i"),
REG_CHRE_NATIVE_FUNC(chreSensorGetThreeAxisBias, "(i*)i"),
REG_CHRE_NATIVE_FUNC(chreSensorFlushAsync, "(i*)i"),
//! sensor.h end

//! user_settings.h begin
REG_CHRE_NATIVE_FUNC(chreUserSettingGetState, "(i)i"),
REG_CHRE_NATIVE_FUNC(chreUserSettingConfigureEvents, "(ii)"),
//! user_settings.h end

/* version.h begin */
REG_CHRE_NATIVE_FUNC(chreGetApiVersion, "()i"),
REG_CHRE_NATIVE_FUNC(chreGetVersion, "()i"),
REG_CHRE_NATIVE_FUNC(chreGetPlatformId, "()I"),
/* version.h end */

//! wifi.h begin
REG_CHRE_NATIVE_FUNC(chreWifiGetCapabilities, "()i"),
REG_CHRE_NATIVE_FUNC(chreWifiNanGetCapabilities, "(*)i"),
REG_CHRE_NATIVE_FUNC(chreWifiConfigureScanMonitorAsync, "(i*)i"),
REG_CHRE_NATIVE_FUNC(chreWifiRequestScanAsync, "(i*)i"),
REG_CHRE_NATIVE_FUNC(chreWifiRequestRangingAsync, "(i*)i"),
REG_CHRE_NATIVE_FUNC(chreWifiNanSubscribe, "(**)i"),
REG_CHRE_NATIVE_FUNC(chreWifiNanSubscribeCancel, "(i)i"),
REG_CHRE_NATIVE_FUNC(chreWifiNanRequestRangingAsync, "(**)i"),
//! wifi.h end

//! wwan.h begin
REG_CHRE_NATIVE_FUNC(chreWwanGetCapabilities, "()i"),
REG_CHRE_NATIVE_FUNC(chreWwanGetCellInfoAsync, "(*)i"),
//! wwan.h end
