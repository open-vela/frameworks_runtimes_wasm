# ##############################################################################
# apps/frameworks/chre/CMakeLists.txt
#
# Licensed to the Apache Software Foundation (ASF) under one or more contributor
# license agreements.  See the NOTICE file distributed with this work for
# additional information regarding copyright ownership.  The ASF licenses this
# file to you under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations under
# the License.
#
# ##############################################################################

if (CONFIG_CHRE_WASM)
  set(CXXSRCS
     audio_wrapper.cc
     ble_wrapper.cc
     chre.cc
     event_wrapper.cc
     gnss_wrapper.cc
     re_wrapper.cc
     sensor_wrapper.cc
     user_settings_wrapper.cc
     version_wrapper.cc
     wifi_wrapper.cc
     wwan_wrapper.cc)

  target_sources(apps PRIVATE ${CXXSRCS})
  target_include_directories(apps PRIVATE ${NUTTX_APPS_DIR}/external/android/system/chre/chre/chre_api/include/chre_api)
  target_compile_options(apps PRIVATE -DCHRE_MESSAGE_TO_HOST_MAX_SIZE=${CONFIG_CHRE_MESSAGE_TO_HOST_MAX_SIZE})
  # register WAMR mod
  nuttx_add_wamrmod(MODS chre)
endif()
