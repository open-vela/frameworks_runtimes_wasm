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

#include <stdint.h>
#include <sys/param.h>
#include <sys/types.h>
#include <time.h>

#include "wasm_export.h"

/**
 * @struct wasm_timespec
 * @brief Structure to represent time in seconds and nanoseconds for WebAssembly.
 *
 * This structure is designed to mimic the POSIX timespec structure but is tailored
 * for WebAssembly environments. It includes fields for seconds and nanoseconds,
 * with the nanoseconds field being 32-bit for WASM32 and potentially 64-bit for WASM64.
 *
 * @var wasm_timespec::tv_sec
 * Seconds part of the time representation.
 *
 * @var wasm_timespec::tv_nsec
 * Nanoseconds part of the time representation. Note that this field is 32-bit,
 * but it should be 64-bit if WASM64 is enabled.
 */

struct wasm_timespec {
    int64_t tv_sec; /* seconds */
    uint32_t tv_nsec; /* nanoseconds, it should be uint64_t if WASM64 is enabled */
};

/**
 * @brief Wrapper function for nanosleep system call.
 *
 * This function takes a `wasm_timespec` structure for the requested sleep duration
 * and an optional pointer to a `wasm_timespec` structure to store the remaining time
 * if the sleep is interrupted. It converts the `wasm_timespec` to a `timespec` structure
 * used by the standard `nanosleep` function and calls it. If the sleep is interrupted
 * and a remainder pointer is provided, it updates the remainder with the remaining time.
 *
 * @param req Pointer to the `wasm_timespec` structure specifying the requested sleep duration.
 * @param rem Optional pointer to a `wasm_timespec` structure to store the remaining time if the sleep is interrupted.
 * @return 0 on success, or -1 on failure (with errno set to indicate the error).
 */

int nanosleep_wrapper(const struct wasm_timespec* req, struct wasm_timespec* rem)
{
    int ret;
    struct timespec nreq, nrem;
    nreq.tv_sec = req->tv_sec;
    nreq.tv_nsec = req->tv_nsec;

    ret = nanosleep(&nreq, &nrem);

    if (ret == -1 && rem != NULL) {
        rem->tv_sec = nrem.tv_sec;
        rem->tv_nsec = nrem.tv_nsec;
    }
    return ret;
}

/**
 * @brief Array of NativeSymbol structures for exporting WASM APIs.
 *
 * This array defines the native symbols that are exported for use in WebAssembly environments.
 * Each entry in the array represents a function that can be called from WASM code.
 * The EXPORT_WASM_API_WITH_SIG2 macro is used to define the function signature for the exported API.
 *
 * @var g_libc_symbols
 * Array of NativeSymbol structures containing the exported WASM APIs.
 */

static NativeSymbol g_libc_symbols[] = {
    EXPORT_WASM_API_WITH_SIG2(nanosleep, "(**)i"),
};

/* Register module, this function is called by the WAMR runtime */

bool wamr_module_libc_register(void)
{
    return wasm_runtime_register_natives(
        "env", g_libc_symbols, nitems(g_libc_symbols));
}
