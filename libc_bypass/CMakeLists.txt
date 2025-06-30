# ##############################################################################
# frameworks/runtimes/wasm/libc_bypass/CMakeLists.txt
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

if(CONFIG_WASM_LIBC_BYPASS)
  add_custom_target(
    gen_glue_sources
    COMMAND ${NUTTX_APPS_DIR}/../frameworks/runtimes/wasm/tools/mkwamrglue.py -i
            ${NUTTX_DIR}/syscall/syscall.csv -o syscall_glue.c
    COMMAND ${NUTTX_APPS_DIR}/../frameworks/runtimes/wasm/tools/mkwamrglue.py -i
            ${NUTTX_DIR}/libs/libc/libc.csv -o libc_glue.c
    COMMAND ${NUTTX_APPS_DIR}/../frameworks/runtimes/wasm/tools/mkwamrglue.py
            -i ${NUTTX_DIR}/libs/libm/libm.csv -o libm_glue.c)

  add_dependencies(wamr gen_glue_sources)

  if(NOT EXISTS {CMAKE_CURRENT_BINARY_DIR}/libc_bypass.c)
    file(
      COPY ${NUTTX_APPS_DIR}/frameworks/runtimes/wasm/libc_bypass/libc_bypass.c
      DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
  endif()

  # register WAMR mod
  target_sources(wamr PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libc_bypass.c)
  nuttx_add_wamrmod(MODS libc_bypass)

  # generate .so files
  set(WAMR_ROOT ${NUTTX_APPS_DIR}/interpreters/wamr)
  include_directories(${WAMR_ROOT})
  add_custom_target(
    signature_libc
    COMMAND gcc -shared -fPIC -o signature_libc.so
            ${CMAKE_CURRENT_BINARY_DIR}/signature_libc_glue.c -I${WAMR_ROOT}
    DEPENDS gen_glue_sources
    COMMENT "Build signature_libc.so")

  add_custom_target(
    signature_libm
    COMMAND gcc -shared -fPIC -o signature_libm.so
            ${CMAKE_CURRENT_BINARY_DIR}/signature_libm_glue.c -I${WAMR_ROOT}
    DEPENDS gen_glue_sources
    COMMENT "Build signature_libm.so")

  add_custom_target(
    signature_syscall
    COMMAND gcc -shared -fPIC -o signature_syscall.so
            ${CMAKE_CURRENT_BINARY_DIR}/signature_syscall_glue.c -I${WAMR_ROOT}
    DEPENDS gen_glue_sources
    COMMENT "Build signature_syscall.so")

  wasm_collect_aot_flag("--native-lib=${CMAKE_CURRENT_BINARY_DIR}/signature_libc.so")

  add_dependencies(wamr signature_libc)
  add_dependencies(wamr signature_libm)
  add_dependencies(wamr signature_syscall)

endif()
