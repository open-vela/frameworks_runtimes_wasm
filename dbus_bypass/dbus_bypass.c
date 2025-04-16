/*
 * Copyright (C) 2024 Xiaomi Corporation
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

#include <dbus/dbus.h>
#include <glib/glib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/types.h>

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static bool wasm_runtime_call_indirect_internal(wasm_exec_env_t exec_env,
    uint32_t element_index,
    uint32_t argc,
    uint32_t argv[])
{
    if (element_index == 0) {
        return false;
    }

    return wasm_runtime_call_indirect(exec_env, element_index, argc, argv);
}

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

static void** app_double_pointer_to_native(wasm_exec_env_t env,
    uintptr_t parm)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uint32_t i;
    void** ptr_arr = (void**)parm;
    void* ptr = NULL;

    for (i = 0; i == 0 || ptr != NULL; i++) {
        ptr = (void*)addr_app_to_native(ptr_arr[i]);
        ptr_arr[i] = ptr;
    }

    return ptr_arr;
}

static uintptr_t native_double_pointer_to_app(wasm_exec_env_t env,
    void** ptr_arr)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    ret = addr_native_to_app((uintptr_t)ptr_arr);
    while (ptr_arr != NULL) {
        *ptr_arr = addr_native_to_app((uintptr_t)*ptr_arr);
        ptr_arr++;
    }

    return ret;
}

static void*** app_three_pointer_to_native(wasm_exec_env_t env,
    uintptr_t parm)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uint32_t i;
    void*** p3 = (void***)parm;
    void** p2 = NULL;

    for (i = 0; i == 0 || p2 != NULL; i++) {
        p2 = (void**)addr_app_to_native(p3[i]);
        p2 = app_double_pointer_to_native(env, p2);
        p3[i] = p2;
    }

    return p3;
}

static uintptr_t native_three_pointer_to_app(wasm_exec_env_t env, void*** p3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    ret = addr_native_to_app((uintptr_t)p3);
    while (p3 != NULL) {
        *p3 = native_double_pointer_to_app(env, (uintptr_t)*p3);
        p3++;
    }

    return ret;
}

#ifndef GLUE_FUNCTION_dbus_connection_get_object_path_data
#define GLUE_FUNCTION_dbus_connection_get_object_path_data
uintptr_t glue_dbus_connection_get_object_path_data(wasm_exec_env_t env,
    uintptr_t parm1,
    uintptr_t parm2,
    uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void** data_p = (void**)parm3;

    *data_p = addr_app_to_native((uintptr_t)*data_p);
    ret = dbus_connection_get_object_path_data((void*)parm1,
        (const char*)parm2, data_p);
    *data_p = addr_native_to_app((uintptr_t)*data_p);

    return ret;
}
#endif /* GLUE_FUNCTION_dbus_connection_get_object_path_data */

#ifndef GLUE_FUNCTION_dbus_connection_list_registered
#define GLUE_FUNCTION_dbus_connection_list_registered
uintptr_t glue_dbus_connection_list_registered(wasm_exec_env_t env,
    uintptr_t parm1,
    uintptr_t parm2,
    uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void*** p3 = (void***)parm3;

    p3 = app_three_pointer_to_native(env, p3);
    ret = dbus_connection_list_registered((void*)parm1,
        (const char*)parm2, (char***)p3);
    native_three_pointer_to_app(env, p3);

    return ret;
}

#endif /* GLUE_FUNCTION_dbus_connection_list_registered */

#ifndef GLUE_FUNCTION_dbus_free_string_array
#define GLUE_FUNCTION_dbus_free_string_array
void glue_dbus_free_string_array(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void** p2 = (void**)parm1;

    p2 = app_double_pointer_to_native(env, p2);
    dbus_free_string_array((char**)p2);
    native_double_pointer_to_app(env, p2);
}

#endif /* GLUE_FUNCTION_dbus_free_string_array */

static bool dbus_message_arg_type_is_number(int type)
{
    return (type == DBUS_TYPE_INT32 || type == DBUS_TYPE_INT32 || type == DBUS_TYPE_UINT16 || type == DBUS_TYPE_BYTE || type == DBUS_TYPE_BOOLEAN || type == DBUS_TYPE_UNIX_FD);
}

static bool dbus_message_arg_type_is_string(int type)
{
    return (type == DBUS_TYPE_STRING || type == DBUS_TYPE_OBJECT_PATH || type == DBUS_TYPE_SIGNATURE);
}

/* The va_list is a pointer to the raw data of wasm arguments. */

static void dbus_message_args_conv(wasm_exec_env_t env, int first_arg_type,
    va_list ap, bool to_native)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    int spec_type;
    int msg_type;
    int i;
    int j;
    char* pos = *((char**)&ap);

    spec_type = first_arg_type;
    while (spec_type != DBUS_TYPE_INVALID) {
        if (dbus_message_arg_type_is_string(spec_type)) {
            if (to_native) {
                *(uintptr_t*)pos = (uintptr_t)addr_app_to_native(*(uintptr_t*)pos);
                (*(uintptr_t*)(*(uintptr_t*)pos)) = (uintptr_t)addr_app_to_native(
                    *(uintptr_t*)(*(uintptr_t*)pos));
            } else {
                (*(uintptr_t*)(*(uintptr_t*)pos)) = (uintptr_t)addr_native_to_app(
                    *(uintptr_t*)(*(uintptr_t*)pos));
                *(uintptr_t*)pos = (uintptr_t)addr_native_to_app(*(uintptr_t*)pos);
            }

            pos += sizeof(uintptr_t);
        }

        else if (spec_type == DBUS_TYPE_ARRAY) {
            uintptr_t** array_p;
            int n_elements;

            if (to_native) {
                pos += sizeof(int);
                *(uintptr_t*)pos = addr_app_to_native(*(uintptr_t*)pos);
                array_p = addr_app_to_native(*(uintptr_t*)pos);
                pos += sizeof(uintptr_t*);

                *(uintptr_t*)pos = (uintptr_t)addr_app_to_native(*(uintptr_t*)pos);
                n_elements = *(int*)pos;
                pos += sizeof(int*);

                for (int i = 0; i < n_elements; i++) {
                    array_p[i] = addr_app_to_native(array_p[i]);
                }
            }

            else {
                pos += sizeof(int);

                array_p = *(uintptr_t*)pos;
                *(uintptr_t*)pos = addr_native_to_app(*(uintptr_t*)pos);
                pos += sizeof(uintptr_t*);

                n_elements = *(int*)pos;
                *(uintptr_t*)pos = (uintptr_t)addr_native_to_app(*(uintptr_t*)pos);
                pos += sizeof(uintptr_t*);

                for (int i = 0; i < n_elements; i++) {
                    array_p[i] = addr_native_to_app(array_p[i]);
                }
            }
        }

        else if (dbus_message_arg_type_is_number(spec_type)) {
            if (to_native) {
                *(uintptr_t*)pos = (uintptr_t)addr_app_to_native(*(uintptr_t*)pos);
            } else {
                *(uintptr_t*)pos = (uintptr_t)addr_native_to_app(*(uintptr_t*)pos);
            }

            pos += sizeof(uintptr_t);
        }

        spec_type = *((int*)pos);
        pos += sizeof(int);
    }
}

#ifndef GLUE_FUNCTION_dbus_message_append_args_valist
#define GLUE_FUNCTION_dbus_message_append_args_valist
uintptr_t glue_dbus_message_append_args_valist(wasm_exec_env_t env,
    uintptr_t parm1,
    uintptr_t format, va_list ap)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    dbus_message_args_conv(env, (int)format, ap, true);
    ret = dbus_message_append_args_valist((void*)parm1, (int)format, ap);
    dbus_message_args_conv(env, (int)format, ap, false);

    return ret;
}

#endif /* GLUE_FUNCTION_dbus_message_append_args_valist */

#ifndef GLUE_FUNCTION_dbus_message_append_args
#define GLUE_FUNCTION_dbus_message_append_args
uintptr_t glue_dbus_message_append_args(wasm_exec_env_t env, uintptr_t parm1,
    uintptr_t parm2, va_list ap)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    dbus_message_args_conv(env, (int)parm2, ap, true);
    ret = dbus_message_append_args_valist((void*)parm1, (int)parm2, ap);
    dbus_message_args_conv(env, (int)parm2, ap, false);
    return ret;
}

#endif /* GLUE_FUNCTION_dbus_message_append_args */

#ifndef GLUE_FUNCTION_dbus_message_get_args
#define GLUE_FUNCTION_dbus_message_get_args
uintptr_t glue_dbus_message_get_args(wasm_exec_env_t env, uintptr_t parm1,
    uintptr_t parm2, uintptr_t parm3,
    va_list ap)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    if ((void*)parm2 == addr_app_to_native((uintptr_t)NULL)) {
        parm2 = NULL;
    }

    dbus_message_args_conv(env, parm3, ap, true);
    ret = dbus_message_get_args_valist((void*)parm1, (void*)parm2,
        (int)parm3, ap);
    dbus_message_args_conv(env, parm3, ap, false);
    return ret;
}

#endif /* GLUE_FUNCTION_dbus_message_get_args */

#ifndef GLUE_FUNCTION_dbus_set_error
#define GLUE_FUNCTION_dbus_set_error
void glue_dbus_set_error(wasm_exec_env_t env, uintptr_t parm1,
    uintptr_t parm2, uintptr_t parm3, va_list ap)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    va_list_string2native(env, parm3, ap);
    dbus_set_error((void*)parm1, (const char*)parm2,
        (const char*)parm3, ap);
    va_list_string2app(env, parm3, ap);
}

#endif /* GLUE_FUNCTION_dbus_set_error */

static wasm_exec_env_t dbus_env;
static int dbus_handle_message_func_idx;
static int dbus_free_func_idx;

DBusHandlerResult dbus_handle_message_func_proxy(DBusConnection* conn,
    DBusMessage* msg,
    void* data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[3];
    DBusHandlerResult ret;

    argv[0] = addr_native_to_app((void*)conn);
    argv[1] = addr_native_to_app((void*)msg);
    argv[2] = addr_native_to_app((void*)data);

    ret = wasm_runtime_call_indirect_internal(
              dbus_env, dbus_handle_message_func_idx,
              3, argv)
        ? argv[0]
        : 0;
    return ret;
}

void dbus_free_func_proxy(void* mem)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[1];

    argv[0] = addr_native_to_app((void*)mem);

    wasm_runtime_call_indirect_internal(dbus_env, dbus_free_func_idx, 1, argv);
}

#ifndef GLUE_FUNCTION_dbus_connection_add_filter
#define GLUE_FUNCTION_dbus_connection_add_filter

uintptr_t glue_dbus_connection_add_filter(wasm_exec_env_t env,
    uintptr_t parm1,
    uintptr_t parm2, uintptr_t parm3,
    uintptr_t parm4)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    dbus_env = env;
    dbus_handle_message_func_idx = parm2;
    dbus_free_func_idx = parm4;

    ret = dbus_connection_add_filter((void*)parm1,
        dbus_handle_message_func_proxy,
        (void*)parm3, dbus_free_func_proxy);
    return ret;
}

#endif /* GLUE_FUNCTION_dbus_connection_add_filter */

#ifndef GLUE_FUNCTION_dbus_connection_register_object_path
#define GLUE_FUNCTION_dbus_connection_register_object_path
static int generic_unregister_idx;
static int generic_message_idx;

static DBusHandlerResult generic_message_proxy(DBusConnection* connection,
    DBusMessage* message,
    void* user_data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[3];
    DBusHandlerResult ret;

    argv[0] = addr_native_to_app((void*)connection);
    argv[1] = addr_native_to_app((void*)message);
    argv[2] = addr_native_to_app((void*)user_data);

    ret = wasm_runtime_call_indirect_internal(dbus_env,
              generic_message_idx,
              3,
              argv)
        ? argv[0]
        : 1;

    return ret;
}

static void generic_unregister_proxy(DBusConnection* connection,
    void* user_data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[2];

    argv[0] = addr_native_to_app((void*)connection);
    argv[1] = addr_native_to_app((void*)user_data);

    wasm_runtime_call_indirect_internal(dbus_env,
        generic_unregister_idx,
        2,
        argv);
}

static DBusObjectPathVTable generic_table = {
    .unregister_function = generic_unregister_proxy,
    .message_function = generic_message_proxy,
};

uintptr_t glue_dbus_connection_register_object_path(wasm_exec_env_t env,
    uintptr_t parm1,
    uintptr_t parm2,
    uintptr_t parm3,
    uintptr_t parm4)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    const DBusObjectPathVTable* table = (const DBusObjectPathVTable*)parm3;

    generic_unregister_idx = table->unregister_function;
    generic_message_idx = table->message_function;
    dbus_env = env;

    ret = dbus_connection_register_object_path((DBusConnection*)parm1,
        (const char*)parm2,
        &generic_table, (void*)parm4);
    return ret;
}

#endif /* GLUE_FUNCTION_dbus_connection_register_object_path */

#ifndef GLUE_FUNCTION_dbus_connection_remove_filter
#define GLUE_FUNCTION_dbus_connection_remove_filter
void glue_dbus_connection_remove_filter(wasm_exec_env_t env, uintptr_t parm1,
    uintptr_t parm2, uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    dbus_env = env;
    dbus_handle_message_func_idx = parm2;

    dbus_connection_remove_filter((DBusConnection*)parm1,
        dbus_handle_message_func_proxy,
        (void*)parm3);
}

#endif /* GLUE_FUNCTION_dbus_connection_remove_filter */

#ifndef GLUE_FUNCTION_dbus_connection_set_watch_functions
#define GLUE_FUNCTION_dbus_connection_set_watch_functions

static int dbus_add_watch_func_idx;
static int dbus_remove_watch_func_idx;
static int dbus_watch_toggled_func_idx;
static int dbus_connection_set_watch_free_data_idx;

dbus_bool_t dbus_add_watch_func_proxy(DBusWatch* watch, void* data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[2];

    argv[0] = addr_native_to_app((void*)watch);
    argv[1] = addr_native_to_app((void*)data);

    return wasm_runtime_call_indirect_internal(dbus_env,
               dbus_add_watch_func_idx,
               2, argv)
        ? argv[0]
        : 0;
}

void dbus_remove_watch_func_proxy(DBusWatch* watch, void* data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[2];

    argv[0] = addr_native_to_app((void*)watch);
    argv[1] = addr_native_to_app((void*)data);

    wasm_runtime_call_indirect_internal(dbus_env,
        dbus_remove_watch_func_idx,
        2,
        argv);
}

void dbus_watch_toggled_func_proxy(DBusWatch* watch, void* data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[2];

    argv[0] = addr_native_to_app((void*)watch);
    argv[1] = addr_native_to_app((void*)data);

    wasm_runtime_call_indirect_internal(dbus_env,
        dbus_watch_toggled_func_idx,
        2,
        argv);
}

void dbus_connection_set_watch_free_data_proxy(void* data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[1];

    argv[0] = addr_native_to_app((void*)data);

    wasm_runtime_call_indirect_internal(
        dbus_env,
        dbus_connection_set_watch_free_data_idx,
        1,
        argv);
}

uintptr_t glue_dbus_connection_set_watch_functions(
    wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2,
    uintptr_t parm3, uintptr_t parm4, uintptr_t parm5, uintptr_t parm6)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    dbus_env = env;
    dbus_add_watch_func_idx = parm2;
    dbus_remove_watch_func_idx = parm3;
    dbus_watch_toggled_func_idx = parm4;
    dbus_connection_set_watch_free_data_idx = parm6;

    ret = dbus_connection_set_watch_functions(
        (DBusConnection*)parm1,
        dbus_add_watch_func_proxy,
        dbus_remove_watch_func_proxy,
        dbus_watch_toggled_func_proxy,
        (void*)parm5,
        dbus_free_func_proxy);
    return ret;
}

#endif /* GLUE_FUNCTION_dbus_connection_set_watch_functions */

#ifndef GLUE_FUNCTION_dbus_connection_set_timeout_functions
#define GLUE_FUNCTION_dbus_connection_set_timeout_functions
static int dbus_add_timeout_idx;
static int dbus_remove_timeout_idx;
static int dbus_timeout_toggled_idx;
typedef dbus_bool_t (*DBusTimeoutHandler)(void* data);
typedef void (*DBusFreeFunction)(void* memory);

struct DBusTimeout {
    int refcount;
    int interval;

    DBusTimeoutHandler handler;
    void* handler_data;
    DBusFreeFunction free_handler_data_function;

    void* data;
    DBusFreeFunction free_data_function;
    unsigned int enabled : 1;
    unsigned int needs_restart : 1;
};

dbus_bool_t dbus_add_timeout_proxy(DBusTimeout* timeout, void* data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[2];
    dbus_bool_t ret;

    argv[0] = addr_native_to_app((void*)timeout);
    argv[1] = addr_native_to_app((void*)data);
    ret = wasm_runtime_call_indirect_internal(
              dbus_env,
              dbus_add_timeout_idx,
              2,
              argv)
        ? argv[0]
        : 0;
    return ret;
}

void dbus_remove_timeout_proxy(DBusTimeout* timeout, void* data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[2];

    argv[0] = addr_native_to_app((void*)timeout);
    argv[1] = addr_native_to_app((void*)data);

    wasm_runtime_call_indirect_internal(
        dbus_env,
        dbus_remove_timeout_idx,
        2,
        argv)
        ? argv[0]
        : 0;
}

void dbus_timeout_toggled_proxy(DBusTimeout* timeout, void* data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[2];

    argv[0] = addr_native_to_app((void*)timeout);
    argv[1] = addr_native_to_app((void*)data);

    wasm_runtime_call_indirect_internal(
        dbus_env,
        dbus_timeout_toggled_idx,
        2,
        argv)
        ? argv[0]
        : 0;
}

uintptr_t glue_dbus_connection_set_timeout_functions(
    wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2, uintptr_t parm3,
    uintptr_t parm4, uintptr_t parm5, uintptr_t parm6)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    dbus_env = env;
    dbus_add_timeout_idx = parm2;
    dbus_remove_timeout_idx = parm3;
    dbus_timeout_toggled_idx = parm4;
    dbus_free_func_idx = parm6;
    ret = dbus_connection_set_timeout_functions(
        (void*)parm1, dbus_add_timeout_proxy,
        dbus_remove_timeout_proxy, dbus_timeout_toggled_proxy, (void*)parm5,
        dbus_free_func_proxy);
    return ret;
}

#endif /* GLUE_FUNCTION_dbus_connection_set_timeout_functions */

#ifndef GLUE_FUNCTION_dbus_timeout_handle
#define GLUE_FUNCTION_dbus_timeout_handle
uintptr_t glue_dbus_timeout_handle(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    ret = dbus_timeout_handle((DBusTimeout*)parm1);
    return ret;
}

#endif /* GLUE_FUNCTION_dbus_timeout_handle */

#ifndef GLUE_FUNCTION_dbus_message_iter_append_basic
#define GLUE_FUNCTION_dbus_message_iter_append_basic
uintptr_t glue_dbus_message_iter_append_basic(wasm_exec_env_t env,
    uintptr_t parm1,
    uintptr_t parm2,
    uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    int type = (int)parm2;
    char** string_p = NULL;

    switch (type) {
    case DBUS_TYPE_STRING:
    case DBUS_TYPE_OBJECT_PATH:
    case DBUS_TYPE_SIGNATURE:
        string_p = (char**)parm3;
        *string_p = (char*)addr_app_to_native(*string_p);
        break;
    }

    ret = dbus_message_iter_append_basic((DBusMessageIter*)parm1, (int)parm2,
        (const void*)parm3);
    switch (type) {
    case DBUS_TYPE_STRING:
    case DBUS_TYPE_OBJECT_PATH:
    case DBUS_TYPE_SIGNATURE:
        if (string_p) {
            *string_p = (char*)(uintptr_t)addr_native_to_app(*string_p);
        }

        break;
    }

    return ret;
}

#endif /* GLUE_FUNCTION_dbus_message_iter_append_basic */

#ifndef GLUE_FUNCTION_dbus_connection_set_dispatch_status_function
#define GLUE_FUNCTION_dbus_connection_set_dispatch_status_function

static int dbus_dispatch_status_func_idx;
static int dbus_connection_set_dispatch_status_free_data_idx;

void dbus_dispatch_status_proxy(DBusConnection* connection,
    DBusDispatchStatus new_status, void* data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[3];

    argv[0] = addr_native_to_app((void*)connection);
    argv[1] = new_status;
    argv[2] = addr_native_to_app((void*)data);
    wasm_runtime_call_indirect_internal(dbus_env,
        dbus_dispatch_status_func_idx, 3, argv);
}

void dbus_connection_set_dispatch_status_free_data(void* data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[1];

    argv[0] = addr_native_to_app((void*)data);

    wasm_runtime_call_indirect_internal(
        dbus_env,
        dbus_connection_set_dispatch_status_free_data_idx,
        1,
        argv);
}

void glue_dbus_connection_set_dispatch_status_function(wasm_exec_env_t env,
    uintptr_t parm1,
    uintptr_t parm2,
    uintptr_t parm3,
    uintptr_t parm4)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    dbus_env = env;
    dbus_dispatch_status_func_idx = parm2;
    dbus_connection_set_dispatch_status_free_data_idx = parm4;

    dbus_connection_set_dispatch_status_function(
        (DBusConnection*)parm1,
        dbus_dispatch_status_proxy,
        (void*)parm3,
        dbus_connection_set_dispatch_status_free_data);
}

#endif /* GLUE_FUNCTION_dbus_connection_set_dispatch_status_function */

typedef struct dbus_free_data_idx {
    int idx_new;
    int idx_old;
    void* data;
} dbus_free_data_idx;

#define DBUS_FREE_DATA_IDX_MAX 64
dbus_free_data_idx free_data_idx[DBUS_FREE_DATA_IDX_MAX] = {
    0
};

#ifndef GLUE_FUNCTION_dbus_watch_set_data
#define GLUE_FUNCTION_dbus_watch_set_data

typedef struct watch_info {
    guint id;
    DBusWatch* watch;
    DBusConnection* conn;
} watch_info;

void dbus_watch_set_data_free_data(void* w_info)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    watch_info* info = (watch_info*)w_info;
    DBusWatch* watch;
    uint32_t argv[1];
    int i;

    if (info == NULL) {
        return;
    }

    watch = (DBusWatch*)addr_app_to_native(info->watch);
    for (i = 0; i < DBUS_FREE_DATA_IDX_MAX; i++) {
        if (free_data_idx[i].data == watch) {
            break;
        }
    }

    if (i == DBUS_FREE_DATA_IDX_MAX) {
        return;
    }

    argv[0] = addr_native_to_app((void*)w_info);

    wasm_runtime_call_indirect(dbus_env, free_data_idx[i].idx_old, 1, argv);

    if (free_data_idx[i].idx_old == 0) {
        free_data_idx[i].data = NULL;
    }
}

void glue_dbus_watch_set_data(wasm_exec_env_t env, uintptr_t parm1,
    uintptr_t parm2, uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    DBusWatch* watch = (DBusWatch*)parm1;
    void* data = (void*)parm2;
    uintptr_t ret;
    int i;

    for (i = 0; i < DBUS_FREE_DATA_IDX_MAX; i++) {
        if (free_data_idx[i].data == watch) {
            free_data_idx[i].idx_old = free_data_idx[i].idx_new;
            free_data_idx[i].idx_new = parm3;
            break;
        }

        if (free_data_idx[i].data == NULL) {
            free_data_idx[i].data = watch;
            free_data_idx[i].idx_new = parm3;
            break;
        }
    }

    dbus_env = env;
    dbus_watch_set_data(
        watch,
        data,
        dbus_watch_set_data_free_data);
}

#endif /* GLUE_FUNCTION_dbus_watch_set_data */

#ifndef GLUE_FUNCTION_dbus_timeout_set_data
#define GLUE_FUNCTION_dbus_timeout_set_data

struct timeout_handler {
    guint id;
    DBusTimeout* timeout;
};

void dbus_timeout_set_data_free_data(void* data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    struct timeout_handler* handler = data;
    DBusTimeout* timeout;
    uint32_t argv[1];
    uint32_t i = 0;

    if (!data) {
        return;
    }

    timeout = (DBusTimeout*)addr_app_to_native(handler->timeout);

    for (i = 0; i < DBUS_FREE_DATA_IDX_MAX; i++) {
        if (free_data_idx[i].data == timeout) {
            break;
        }
    }

    if (i == DBUS_FREE_DATA_IDX_MAX) {
        return;
    }

    argv[0] = addr_native_to_app((void*)data);

    wasm_runtime_call_indirect_internal(
        dbus_env,
        free_data_idx[i].idx_old,
        1,
        argv);

    if (free_data_idx[i].idx_old == 0) {
        free_data_idx[i].data = NULL;
    }
}

void glue_dbus_timeout_set_data(wasm_exec_env_t env, uintptr_t parm1,
    uintptr_t parm2, uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    DBusTimeout* timeout = (DBusTimeout*)parm1;
    void* data = (void*)parm2;
    uint32_t i = 0;

    dbus_env = env;

    for (i = 0; i < DBUS_FREE_DATA_IDX_MAX; i++) {
        if (free_data_idx[i].data == timeout) {
            free_data_idx[i].idx_old = free_data_idx[i].idx_new;
            free_data_idx[i].idx_new = parm3;
            break;
        }

        if (free_data_idx[i].data == NULL) {
            free_data_idx[i].data = timeout;
            free_data_idx[i].idx_new = parm3;
            break;
        }
    }

    dbus_timeout_set_data(
        timeout,
        data,
        dbus_timeout_set_data_free_data);
}

#endif /* GLUE_FUNCTION_dbus_timeout_set_data */

#ifndef GLUE_FUNCTION_dbus_pending_call_set_notify
#define GLUE_FUNCTION_dbus_pending_call_set_notify
static int dbus_pending_call_notify_idx;
static int dbus_pending_call_set_notify_free_data_idx;

void dbus_pending_call_notify_proxy(DBusPendingCall* pending,
    void* user_data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[2];

    argv[0] = addr_native_to_app((void*)pending);
    argv[1] = addr_native_to_app((void*)user_data);

    wasm_runtime_call_indirect_internal(
        dbus_env,
        dbus_pending_call_notify_idx,
        2,
        argv);
}

static void dbus_pending_call_set_notify_free_data(void* data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[1];

    argv[0] = addr_native_to_app((void*)data);

    wasm_runtime_call_indirect_internal(
        dbus_env,
        dbus_pending_call_set_notify_free_data_idx,
        1,
        argv);
}

uintptr_t glue_dbus_pending_call_set_notify(wasm_exec_env_t env,
    uintptr_t parm1, uintptr_t parm2,
    uintptr_t parm3, uintptr_t parm4)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    DBusPendingCall* pending = (DBusPendingCall*)parm1;
    void* data = (void*)parm3;
    uintptr_t ret;

    dbus_env = env;
    dbus_pending_call_notify_idx = parm2;
    dbus_pending_call_set_notify_free_data_idx = parm4;
    ret = dbus_pending_call_set_notify(
        pending,
        dbus_pending_call_notify_proxy,
        data,
        dbus_pending_call_set_notify_free_data);
    return ret;
}

#endif /* GLUE_FUNCTION_dbus_pending_call_set_notify */

#ifndef GLUE_FUNCTION_dbus_set_disconnect_function
#define GLUE_FUNCTION_dbus_set_disconnect_function
static int dbus_watch_func_idx;
static int dbus_set_disconnect_function_free_data_idx;

void dbus_watch_func_proxy(DBusConnection* connection, void* user_data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[2];

    argv[0] = addr_native_to_app((void*)connection);
    argv[1] = addr_native_to_app((void*)user_data);

    wasm_runtime_call_indirect_internal(
        dbus_env,
        dbus_watch_func_idx,
        2,
        argv);
}

static void dbus_set_disconnect_function_free_data(void* data)
{
    wasm_module_inst_t module_inst = get_module_inst(dbus_env);
    uint32_t argv[1];

    argv[0] = addr_native_to_app((void*)data);

    wasm_runtime_call_indirect_internal(
        dbus_env,
        dbus_set_disconnect_function_free_data_idx,
        1,
        argv);
}

uintptr_t glue_dbus_set_disconnect_function(wasm_exec_env_t env,
    uintptr_t parm1, uintptr_t parm2,
    uintptr_t parm3, uintptr_t parm4)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    dbus_env = env;
    dbus_watch_func_idx = parm2;
    dbus_set_disconnect_function_free_data_idx = parm4;
    ret = dbus_set_disconnect_function(
        (void*)parm1,
        dbus_watch_func_proxy,
        (void*)parm3,
        dbus_set_disconnect_function_free_data);
    return ret;
}

#endif /* GLUE_FUNCTION_dbus_set_disconnect_function */

#ifndef GLUE_FUNCTION_dbus_message_iter_get_fixed_array
#define GLUE_FUNCTION_dbus_message_iter_get_fixed_array
void glue_dbus_message_iter_get_fixed_array(wasm_exec_env_t env,
    uintptr_t parm1, uintptr_t parm2,
    uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    DBusMessageIter* iter = (DBusMessageIter*)parm1;
    void* value = (void*)parm2;
    int* n_elements = (int*)parm3;

    dbus_message_iter_get_fixed_array(iter, value, n_elements);
    *(uintptr_t*)value = addr_native_to_app(*(uintptr_t*)value);
}

#endif /* GLUE_FUNCTION_dbus_message_iter_get_fixed_array */

#ifndef GLUE_FUNCTION_dbus_message_iter_get_basic
#define GLUE_FUNCTION_dbus_message_iter_get_basic
void glue_dbus_message_iter_get_basic(wasm_exec_env_t env, uintptr_t parm1,
    uintptr_t parm2)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    DBusMessageIter* iter = (DBusMessageIter*)parm1;
    void* value = (void*)parm2;
    int type = dbus_message_iter_get_arg_type(iter);

    dbus_message_iter_get_basic(iter, value);
    if (type == DBUS_TYPE_STRING || type == DBUS_TYPE_OBJECT_PATH || type == DBUS_TYPE_SIGNATURE || type == DBUS_TYPE_ARRAY || type == DBUS_TYPE_UNIX_FD) {
        *(uintptr_t*)value = addr_native_to_app(*(uintptr_t*)value);
    }
}

#endif /* GLUE_FUNCTION_dbus_message_iter_get_basic */
/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "dbus_glue.c"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

bool wamr_module_dbus_bypass_register(void)
{
    bool ret;

    /* Add extra init hook here */

    ret = wasm_runtime_register_natives("env", g_dbus_native_symbols, nitems(g_dbus_native_symbols));
    if (!ret) {
        return ret;
    }

    return ret;
}
