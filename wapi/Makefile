#
# Copyright (C) 2025 Xiaomi Corporation
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

IWASM_ROOT := $(APPDIR)/interpreters/wamr

CFLAGS += -I$(APPDIR)/include/wireless

CSRCS += wapi.c

CFLAGS += -Wno-unused-variable -Wno-int-conversion

WAMR_MODULE_NAME = wapi

GLUECSRCS = wapi_glue.c
CSVSRCS += $(APPDIR)/wireless/wapi/wapi.csv

$(GLUECSRCS): $(CSVSRCS) $(APPDIR)/../frameworks/runtimes/wasm/tools/mkwamrglue.py
	$(Q)  $(APPDIR)/../frameworks/runtimes/wasm/tools/mkwamrglue.py \
	-i $(APPDIR)/wireless/wapi/wapi.csv -o wapi_glue.c
	gcc -shared -fPIC -o signature_wapi.so signature_wapi_glue.c -I$(IWASM_ROOT)

depend:: $(GLUECSRCS)

distclean::
	$(call DELFILE, wapi_glue.c)

include $(APPDIR)/interpreters/wamr/Module.mk
include $(APPDIR)/Application.mk
