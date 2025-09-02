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

IWASM_ROOT := $(APPDIR)/interpreters/wamr

CFLAGS += -I$(APPDIR)/external/glib/glib
CFLAGS += -I$(APPDIR)/external/dbus/dbus/dbus
CFLAGS += -I$(APPDIR)/external/dbus/dbus
CFLAGS += -I$(APPDIR)/frameworks/system/utils/gdbus

CSRCS += dbus.c

CFLAGS += -Wno-unused-variable -Wno-int-conversion

WAMR_MODULE_NAME = dbus

GLUECSRCS = dbus_glue.c
CSVSRCS += $(APPDIR)/external/dbus/dbus.csv

$(GLUECSRCS): $(CSVSRCS) ../tools/mkwamrglue.py
	$(Q) ../tools/mkwamrglue.py -i $(APPDIR)/external/dbus/dbus.csv -o dbus_glue.c
	gcc -shared -fPIC -o signature_dbus.so signature_dbus_glue.c -I$(IWASM_ROOT)

depend:: $(GLUECSRCS)

distclean::
	$(call DELFILE, dbus_glue.c)
	$(call DELFILE, signature_dbus.so)
	$(call DELFILE, signature_dbus_glue.c)

include $(APPDIR)/interpreters/wamr/Module.mk
include $(APPDIR)/Application.mk
