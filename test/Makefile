#
# Copyright (C) 2024 Xiaomi Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

include $(APPDIR)/Make.defs

PRIORITY  = $(CONFIG_WASM_TEST_PRIORITY)
STACKSIZE = $(CONFIG_WASM_TEST_STACKSIZE)

ifneq ($(CONFIG_WASM_TEST_LIBC),)
PROGNAME += cmocka_wasm_test_libc
MAINSRC  += libc_test.c
endif

# If any test case enabled, then enable the test module

ifneq ($(PROGNAME),)
ifneq ($(MAINSRC),)
MODULE = y
endif
endif

generated:
	mkdir -p generated

# Rule to compile the libc proxy module

generated/libc.wasm: proxy/libc.c generated
	$(WASI_SDK_PATH)/bin/clang --target=wasm32 -O3 -flto --sysroot=$(APPDIR)/frameworks/runtimes/wasm/vela-sysroot -nostdlib -z stack-size=4096 -Wl,--initial-memory=65536 -o generated/libc.wasm proxy/libc.c -Wl,--no-entry -Wl,--export-all -Wl,--allow-undefined

generated/libc_wasm_array.c: generated/libc.wasm
	xxd -i $< > $@

context:: generated/libc_wasm_array.c

distclean::
	$(call DELDIR, generated)

include $(APPDIR)/Application.mk
