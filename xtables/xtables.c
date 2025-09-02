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

#include "netutils/xtables.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: xtables_event_stream_extract
 *
 * Description:
 *   Extract the next event from the stream.
 *
 ****************************************************************************/
#ifndef GLUE_FUNCTION_xtables_find_match
#define GLUE_FUNCTION_xtables_find_match
uintptr_t glue_xtables_find_match(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2, uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    /* Compatible with the xtables interface */
    ret = addr_native_to_app((void*)0);

    return ret;
}

#endif /* GLUE_FUNCTION_xtables_find_match */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "xtables_glue.c"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

bool wamr_module_xtables_register(void)
{
    bool ret;

    /* Add extra init hook here */

    ret = wasm_runtime_register_natives("env", g_connman_native_symbols, nitems(g_connman_native_symbols));
    if (!ret) {
        return ret;
    }

    return ret;
}
