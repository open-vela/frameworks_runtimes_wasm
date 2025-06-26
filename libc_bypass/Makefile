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

CSRCS += libc_bypass.c

#Filter compilation warnings for syscall library
CFLAGS += -Wno-unused-variable -Wno-int-conversion

WAMR_MODULE_NAME = libc_bypass


GLUECSRCS = syscall_glue.c libc_glue.c libm_glue.c
CSVSRCS = $(TOPDIR)/syscall/syscall.csv $(TOPDIR)/libs/libc/libc.csv $(TOPDIR)/libs/libm/libm.csv

$(GLUECSRCS): $(CSVSRCS) ../tools/mkwamrglue.py
	$(Q) ../tools/mkwamrglue.py -i $(TOPDIR)/syscall/syscall.csv -o syscall_glue.c
	$(Q) ../tools/mkwamrglue.py -i $(TOPDIR)/libs/libc/libc.csv -o libc_glue.c
	$(Q) ../tools/mkwamrglue.py -i $(TOPDIR)/libs/libm/libm.csv -o libm_glue.c
	gcc -shared -fPIC -o signature_libc.so signature_libc_glue.c -I$(IWASM_ROOT)
	gcc -shared -fPIC -o signature_libm.so signature_libm_glue.c -I$(IWASM_ROOT)
	gcc -shared -fPIC -o signature_syscall.so signature_syscall_glue.c -I$(IWASM_ROOT)

depend:: $(GLUECSRCS)

distclean::
	$(call DELFILE, syscall_glue.c)
	$(call DELFILE, libc_glue.c)
	$(call DELFILE, libm_glue.c)
	$(call DELFILE, signature_*.so)
	$(call DELFILE, signature_*.c)

include $(APPDIR)/interpreters/wamr/Module.mk
include $(APPDIR)/Application.mk
