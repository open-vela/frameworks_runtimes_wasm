# ##############################################################################
# frameworks/runtimes/wasm/test/CMakeLists.txt
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

if(CONFIG_WASM_TEST_LIBC)

  list(APPEND WASI_SDK_PATH $ENV{WASI_SDK_PATH})
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)

  add_custom_target(
    build_libc_wasm
    COMMAND ${WASI_SDK_PATH}/bin/clang
       --target=wasm32 -O3 -flto
       --sysroot=${CMAKE_CURRENT_LIST_DIR}/../vela-sysroot
       -nostdlib -z stack-size=4096 -Wl,--initial-memory=65536
       -o generated/libc.wasm ${CMAKE_CURRENT_LIST_DIR}/proxy/libc.c
       -Wl,--no-entry -Wl,--export-all -Wl,--allow-undefined
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/proxy/libc.c
    COMMENT "Building libc.wasm")

  add_custom_target(
    build_libc_wasm_array
    COMMAND xxd -i generated/libc.wasm > generated/libc_wasm_array.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS build_libc_wasm
    COMMENT "Converting libc.wasm to C array")

  nuttx_add_application(
      NAME
      cmocka_wasm_test_libc
      PRIORITY
      ${CONFIG_WASM_TEST_PRIORITY}
      STACKSIZE
      ${CONFIG_WASM_TEST_STACKSIZE}
      MODULE
      ${CONFIG_WASM_TEST_LIBC}
      SRCS
      libc_test.c
      INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_BINARY_DIR}
      DEPENDS
      cmocka
      build_libc_wasm_array)

endif()
