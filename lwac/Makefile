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

# LWAC built-in application info

PROGNAME  = lwac
PRIORITY  = $(CONFIG_WASM_LWAC_PRIORITY)
STACKSIZE = $(CONFIG_WASM_LWAC_STACKSIZE)
MODULE    = $(CONFIG_WASM_LWAC)

# LWAC

MAINSRC = lwac.c

CSRCS = lwac_module_registry.c

CFLAGS += -I$(APPDIR)/interpreters/wamr

ifneq ($(CONFIG_WASM_LWAC_TEST),)

PROGNAME += lwac_test
PRIORITY += $(CONFIG_WASM_LWAC_TEST_PRIORITY)
STACKSIZE += $(CONFIG_WASM_LWAC_TEST_STACKSIZE)
MODULE += $(CONFIG_WASM_LWAC_TEST)

MAINSRC += tests/lwac_test.c
endif

include $(APPDIR)/Application.mk
