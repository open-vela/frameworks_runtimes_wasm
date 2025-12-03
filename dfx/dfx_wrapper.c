/*
 * Copyright (C) 2025 Xiaomi Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "wasm_export.h"
#include <sys/param.h>

#include "event.h"

static void
va_list_string2conv(wasm_exec_env_t exec_env, const char* format,
    va_list ap, bool to_native)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    char* pos = *((char**)&ap);

    if (pos == NULL) {
        return;
    }

    int long_ctr = 0;
    int might = 0;

    while (*format) {
        if (!might) {
            if (*format == '%') {
                might = 1;
                long_ctr = 0;
            }
        } else {
            switch (*format) {
            case '.':
            case '+':
            case '-':
            case ' ':
            case '#':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                goto still_might;

            case 't':
            case 'z':
                long_ctr = 1;
                goto still_might;

            case 'j':
                long_ctr = 2;
                goto still_might;

            case 'l':
                long_ctr++;
            case 'h':
                goto still_might;

            case 'o':
            case 'd':
            case 'i':
            case 'u':
            case 'p':
            case 'x':
            case 'X':
            case 'c': {
                if (long_ctr < 2) {
                    pos += sizeof(int32_t);
                } else {
                    pos += sizeof(int64_t);
                }
                break;
            }

            case 'e':
            case 'E':
            case 'g':
            case 'G':
            case 'f':
            case 'F': {
                pos += sizeof(double);
                break;
            }

            case 's': {
                if (to_native) {
                    *(uintptr_t*)pos = (uintptr_t)addr_app_to_native(*(uintptr_t*)pos);
                } else {
                    *(uintptr_t*)pos = (uintptr_t)addr_native_to_app(*(uintptr_t*)pos);
                }

                pos += sizeof(uintptr_t);
                break;
            }

            default:
                break;
            }

            might = 0;
        }

    still_might:
        ++format;
    }
}

#define va_list_string2native(exec_env, format, ap) \
    va_list_string2conv(exec_env, format, ap, true)

#define va_list_string2app(exec_env, format, ap) \
    va_list_string2conv(exec_env, format, ap, false)

#ifndef GLUE_FUNCTION_sendEventMisightF
#define GLUE_FUNCTION_sendEventMisightF

int glue_sendEventMisightF(wasm_exec_env_t env, uintptr_t parm1, uintptr_t format, va_list ap)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    va_list_string2native(env, (const char*)format, ap);
    sched_note_vprintf_ip(DFX_EVENT_TAG_DIAG, LOG_INFO, (uint32_t)parm1, (const char*)format, 0, &ap);
    va_list_string2app(env, (const char*)format, ap);

    return 0;
}
#endif /* GLUE_FUNCTION_sendEventMisightF */

#ifndef native_function
#define native_function(func_name, signature)         \
    {                                                 \
#func_name, glue_##func_name, signature, NULL \
    }
#endif

static NativeSymbol g_dfx_native_symbols[] = {
#ifndef GLUE_ENTRY_sendEventMisightF
#define GLUE_ENTRY_sendEventMisightF
    native_function(sendEventMisightF, "(i$*)i"),
#endif /* GLUE_ENTRY_sendEventMisightF */
};

bool wamr_module_dfx_wrapper_register(void)
{
    bool ret;

    /* Add extra init hook here */

    ret = wasm_runtime_register_natives("env", g_dfx_native_symbols, nitems(g_dfx_native_symbols));
    if (!ret) {
        return ret;
    }

    return ret;
}
