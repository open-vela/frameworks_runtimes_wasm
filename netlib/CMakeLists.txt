# ##############################################################################
# frameworks/runtimes/wasm/dbus_bypass/CMakeLists.txt
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

if(CONFIG_WASM_NETLIB)
  set(INCDIR)
  list(APPEND INCDIR ${NUTTX_APPS_DIR}/include/netutils)

  if(NOT EXISTS {CMAKE_CURRENT_BINARY_DIR}/netlib.c)
    file(
      COPY ${NUTTX_APPS_DIR}/frameworks/runtimes/wasm/netlib/netlib.c
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  endif()

  # register WAMR mod
  target_sources(wamr PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/netlib.c)
  target_include_directories(wamr PRIVATE ${INCDIR})
  nuttx_add_wamrmod(MODS netlib)

endif()
