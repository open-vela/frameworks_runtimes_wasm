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

#include <errno.h>
#include <math.h>
#include <netinet/arp.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/types.h>
#include <cJSON.h>

#include <nuttx/wireless/wireless.h>

#include "wireless/wapi.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/
#define PTR_TO_NATIVE(ptr, field)                                                  \
    do {                                                                           \
        if ((ptr)->field) {                                                        \
            (ptr)->field = (uintptr_t)addr_app_to_native((uintptr_t)(ptr)->field); \
        }                                                                          \
    } while (0)

#define PTR_TO_APP(ptr, field)                                                     \
    do {                                                                           \
        if ((ptr)->field) {                                                        \
            (ptr)->field = (uintptr_t)addr_native_to_app((uintptr_t)(ptr)->field); \
        }                                                                          \
    } while (0)

static void wconfig_ptrs_to_native(wasm_module_inst_t module_inst, struct wpa_wconfig_s* wconfig)
{
    PTR_TO_NATIVE(wconfig, ifname);
    PTR_TO_NATIVE(wconfig, ssid);
    PTR_TO_NATIVE(wconfig, bssid);
    PTR_TO_NATIVE(wconfig, passphrase);
}

static void wconfig_ptrs_to_app(wasm_module_inst_t module_inst, struct wpa_wconfig_s* wconfig)
{
    PTR_TO_APP(wconfig, ifname);
    PTR_TO_APP(wconfig, ssid);
    PTR_TO_APP(wconfig, bssid);
    PTR_TO_APP(wconfig, passphrase);
}

/****************************************************************************
 * Name: wapi_event_stream_extract
 *
 * Description:
 *   Extract the next event from the stream.
 *
 ****************************************************************************/

#ifndef GLUE_FUNCTION_wapi_scan_coll
#define GLUE_FUNCTION_wapi_scan_coll
uintptr_t glue_wapi_scan_coll(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2, uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);

    FAR struct wapi_scan_info_s* info;
    FAR struct wapi_scan_info_s* tmp;
    FAR struct wapi_list_s* list;

    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm2 == addr_app) {
        parm2 = (uintptr_t)NULL;
    }

    if ((void*)parm3 == addr_app) {
        parm3 = (uintptr_t)NULL;
        return -EFAULT;
    }

    list = (struct wapi_list_s*)parm3;
    list->head.scan = (uintptr_t)addr_app_to_native((uintptr_t)list->head.scan);
    info = list->head.scan;

    while (info) {
        if (info->next) {
            info->next = (uintptr_t)addr_app_to_native((uintptr_t)info->next);
        } else {
            info->next = NULL;
            break;
        }
        info = info->next;
    }

    ret = wapi_scan_coll((int)parm1, (FAR const char*)parm2, (FAR struct wapi_list_s*)parm3);

    info = list->head.scan;
    list->head.scan = addr_native_to_app(list->head.scan);

    while (info) {
        tmp = info->next;
        if (info->next) {
            info->next = (uintptr_t)addr_native_to_app((uintptr_t)info->next);
        } else {
            info->next = NULL;
            break;
        }
        info = tmp;
    }

    return ret;
}

#endif /* GLUE_FUNCTION_wapi_scan_coll */

#ifndef GLUE_FUNCTION_wapi_scan_coll_free
#define GLUE_FUNCTION_wapi_scan_coll_free
void glue_wapi_scan_coll_free(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);

    FAR struct wapi_scan_info_s* info;
    FAR struct wapi_list_s* list;

    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app) {
        return;
    }

    list = (struct wapi_list_s*)parm1;
    info = (uintptr_t)addr_app_to_native((uintptr_t)list->head.scan);

    while (info) {
        if (info->next) {
            info->next = (uintptr_t)addr_app_to_native((uintptr_t)info->next);
        } else {
            break;
        }
        info = info->next;
    }

    list->head.scan = info;

    wapi_scan_coll_free((FAR struct wapi_list_s*)list);

    list->head.scan = NULL;
}

#endif /* GLUE_FUNCTION_wapi_scan_coll_free */

#ifndef GLUE_FUNCTION_wpa_driver_wext_associate
#define GLUE_FUNCTION_wpa_driver_wext_associate
uintptr_t glue_wpa_driver_wext_associate(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    struct wpa_wconfig_s* wconfig;

    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app) {
        parm1 = (uintptr_t)NULL;
        return -EFAULT;
    }

    wconfig = (struct wpa_wconfig_s*)parm1;
    wconfig_ptrs_to_native(module_inst, wconfig);

    ret = wpa_driver_wext_associate(wconfig);

    wconfig_ptrs_to_app(module_inst, wconfig);

    return ret;
}

#endif /* GLUE_FUNCTION_wpa_driver_wext_associate */

#if defined(CONFIG_WIRELESS_WAPI_INITCONF)

#ifndef GLUE_FUNCTION_wapi_unload_config
#define GLUE_FUNCTION_wapi_unload_config
void glue_wapi_unload_config(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    cJSON* item;
    uintptr_t ret;

    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app) {
        parm1 = (uintptr_t)NULL;
        return;
    }

    item = (cJSON*)parm1;
    item->next = (uintptr_t)addr_app_to_native((uintptr_t)item->next);

    wapi_unload_config((FAR void*)parm1);

    item->next = (uintptr_t)addr_native_to_app((uintptr_t)item->next);
}

#endif /* GLUE_FUNCTION_wapi_unload_config */
#endif /* defined(CONFIG_WIRELESS_WAPI_INITCONF) */

#if defined(CONFIG_WIRELESS_WAPI_INITCONF)

#ifndef GLUE_FUNCTION_wapi_load_config
#define GLUE_FUNCTION_wapi_load_config
uintptr_t glue_wapi_load_config(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2, uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    struct wpa_wconfig_s* conf;

    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app) {
        parm1 = (uintptr_t)NULL;
        return NULL;
    }

    if ((void*)parm2 == addr_app) {
        parm2 = (uintptr_t)NULL;
    }

    if ((void*)parm3 == addr_app) {
        parm3 = (uintptr_t)NULL;
        return NULL;
    }

    conf = (struct wpa_wconfig_s*)parm3;
    wconfig_ptrs_to_native(module_inst, conf);

    ret = addr_native_to_app((uintptr_t)wapi_load_config((FAR const char*)parm1, (FAR const char*)parm2, (FAR struct wpa_wconfig_s*)parm3));

    wconfig_ptrs_to_app(module_inst, conf);

    return ret;
}

#endif /* GLUE_FUNCTION_wapi_load_config */
#endif /* defined(CONFIG_WIRELESS_WAPI_INITCONF) */

#if defined(CONFIG_WIRELESS_WAPI_INITCONF)

#ifndef GLUE_FUNCTION_wapi_save_config
#define GLUE_FUNCTION_wapi_save_config
uintptr_t glue_wapi_save_config(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2, uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    struct wpa_wconfig_s* conf;

    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app) {
        parm1 = (uintptr_t)NULL;
        return NULL;
    }

    if ((void*)parm2 == addr_app) {
        parm2 = (uintptr_t)NULL;
    }

    if ((void*)parm3 == addr_app) {
        parm3 = (uintptr_t)NULL;
        return NULL;
    }

    conf = (struct wpa_wconfig_s*)parm3;
    wconfig_ptrs_to_native(module_inst, conf);

    ret = wapi_save_config((FAR const char*)parm1, (FAR const char*)parm2, (FAR const struct wpa_wconfig_s*)parm3);

    wconfig_ptrs_to_app(module_inst, conf);

    return ret;
}

#endif /* GLUE_FUNCTION_wapi_save_config */
#endif /* defined(CONFIG_WIRELESS_WAPI_INITCONF) */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "wapi_glue.c"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

bool wamr_module_wapi_register(void)
{
    bool ret;

    /* Add extra init hook here */

    ret = wasm_runtime_register_natives("env", g_wapi_native_symbols, nitems(g_wapi_native_symbols));
    if (!ret) {
        return ret;
    }

    return ret;
}
