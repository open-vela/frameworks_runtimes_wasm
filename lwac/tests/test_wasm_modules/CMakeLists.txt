# ##############################################################################
# frameworks/runtimes/wasm/lwac/tests/test_wasm_modules/CMakeLists.txt
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

# Add simple_test module
wasm_add_application(
  NAME
  simple_test
  SRCS
  simple_test.c
  STACK_SIZE
  2048
  INITIAL_MEMORY_SIZE
  65536
  INSTALL_NAME
  ${CONFIG_WASM_LWAC_TEST_MODULE_PATH}/simple_test.wasm)

# Add entry_point_test module
wasm_add_application(
  NAME
  entry_point_test
  SRCS
  entry_point_test.c
  STACK_SIZE
  2048
  INITIAL_MEMORY_SIZE
  65536
  INSTALL_NAME
  ${CONFIG_WASM_LWAC_TEST_MODULE_PATH}/entry_point_test.wasm)
