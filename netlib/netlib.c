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

#include <netlib.h>

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: netlib_event_stream_extract
 *
 * Description:
 *   Extract the next event from the stream.
 *
 ****************************************************************************/
#ifndef GLUE_FUNCTION_netlib_ifup
#define GLUE_FUNCTION_netlib_ifup
uintptr_t glue_netlib_ifup(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    /* Compatible with the netlib interface */
    void* ifname = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == ifname)
        parm1 = (uintptr_t)NULL;

    ret = netlib_ifup((const char*)parm1);

    return ret;
}

#endif /* GLUE_FUNCTION_netlib_ifup */

#ifndef GLUE_FUNCTION_netlib_set_ipv4dnsaddr
#define GLUE_FUNCTION_netlib_set_ipv4dnsaddr
uintptr_t glue_netlib_set_ipv4dnsaddr(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    /* Compatible with the netlib interface */
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    ret = netlib_set_ipv4dnsaddr((FAR const struct in_addr*)parm1);

    return ret;
}

#endif /* GLUE_FUNCTION_netlib_set_ipv4dnsaddr */

#ifndef native_function
#define native_function(func_name, signature)         \
    {                                                 \
#func_name, glue_##func_name, signature, NULL \
    }

#endif
static NativeSymbol g_netlib_native_symbols[] = {
#ifndef GLUE_ENTRY_netlib_ifup
#define GLUE_ENTRY_netlib_ifup
    native_function(netlib_ifup, "($)i"),
#endif /* GLUE_ENTRY_netlib_ifup */

#ifndef GLUE_ENTRY_netlib_set_ipv4dnsaddr
#define GLUE_ENTRY_netlib_set_ipv4dnsaddr
    native_function(netlib_set_ipv4dnsaddr, "(*)i"),
#endif /* GLUE_ENTRY_netlib_set_ipv4dnsaddr */
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

bool wamr_module_netlib_register(void)
{
    bool ret;

    /* Add extra init hook here */

    ret = wasm_runtime_register_natives("env", g_netlib_native_symbols, nitems(g_netlib_native_symbols));
    if (!ret) {
        return ret;
    }

    return ret;
}
