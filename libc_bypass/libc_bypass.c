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

#include <iconv.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <math.h>
#include <mqueue.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <pthread.h>
#include <unistd.h>

#include "wasm_export.h"

/****************************************************************************
 * Private Data
 ****************************************************************************/

#define validate_native_addr(addr, size) \
    wasm_runtime_validate_native_addr(module_inst, addr, size)

typedef int (*out_func_t)(int c, void* ctx);

typedef char* _va_list;
#define _INTSIZEOF(n) (((uint32_t)sizeof(n) + 3) & (uint32_t)~3)
#define _va_arg(ap, t) (*(t*)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)))

static void
print_err(out_func_t out, void* ctx)
{
    out('E', ctx);
    out('R', ctx);
    out('R', ctx);
}

static int
glue_memcpy_s(void* s1, unsigned int s1max, const void* s2, unsigned int n)
{
    char* dest = (char*)s1;
    char* src = (char*)s2;
    if (n == 0) {
        return 0;
    }

    if (s1 == NULL) {
        return -1;
    }
    if (s2 == NULL || n > s1max) {
        memset(dest, 0, s1max);
        return -1;
    }
    memcpy(dest, src, n);
    return 0;
}

#define bh_memcpy_s(dest, dlen, src, slen)               \
    do {                                                 \
        int _ret = glue_memcpy_s(dest, dlen, src, slen); \
        (void)_ret;                                      \
        assert(_ret == 0);                               \
    } while (0)

#define CHECK_VA_ARG(ap, t)                                   \
    do {                                                      \
        if ((uint8_t*)ap + _INTSIZEOF(t) > native_end_addr) { \
            if (fmt_buf != temp_fmt) {                        \
                free(fmt_buf);                                \
            }                                                 \
            goto fail;                                        \
        }                                                     \
    } while (0)

/* clang-format off */
#define PREPARE_TEMP_FORMAT()                                \
    char temp_fmt[32], *s, *fmt_buf = temp_fmt;              \
    uint32_t fmt_buf_len = (uint32_t)sizeof(temp_fmt);           \
    int32_t n;                                                 \
                                                             \
    /* additional 2 bytes: one is the format char,           \
       the other is `\0` */                                  \
    if ((uint32_t)(fmt - fmt_start_addr + 2) >= fmt_buf_len) { \
        assert((uint32_t)(fmt - fmt_start_addr) <=          \
                  UINT32_MAX - 2);                           \
        fmt_buf_len = (uint32_t)(fmt - fmt_start_addr + 2);    \
        if (!(fmt_buf = malloc(fmt_buf_len))) { \
            print_err(out, ctx);                             \
            break;                                           \
        }                                                    \
    }                                                        \
                                                             \
    memset(fmt_buf, 0, fmt_buf_len);                         \
    glue_memcpy_s(fmt_buf, fmt_buf_len, fmt_start_addr,        \
                (uint32_t)(fmt - fmt_start_addr + 1));
/* clang-format on */

#define OUTPUT_TEMP_FORMAT()           \
    do {                               \
        if (n > 0) {                   \
            s = buf;                   \
            while (*s)                 \
                out((int)(*s++), ctx); \
        }                              \
                                       \
        if (fmt_buf != temp_fmt) {     \
            free(fmt_buf);             \
        }                              \
    } while (0)

static bool
_vprintf_wa(out_func_t out, void* ctx, const char* fmt, _va_list ap,
    wasm_module_inst_t module_inst)
{
    int might_format = 0; /* 1 if encountered a '%' */
    int long_ctr = 0;
    const char* fmt_start_addr = NULL;

    uint8_t* native_end_addr;

    if (!wasm_runtime_get_native_addr_range(module_inst, (uint8_t*)ap, NULL,
            &native_end_addr))
        goto fail;

    /* fmt has already been adjusted if needed */

    while (*fmt) {
        if (!might_format) {
            if (*fmt != '%') {
                out((int)*fmt, ctx);
            } else {
                might_format = 1;
                long_ctr = 0;
                fmt_start_addr = fmt;
            }
        } else {
            switch (*fmt) {
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
                goto still_might_format;

            case 't': /* ptrdiff_t */
            case 'z': /* size_t (32bit on wasm) */
                long_ctr = 1;
                goto still_might_format;

            case 'j':
                /* intmax_t/uintmax_t */
                long_ctr = 2;
                goto still_might_format;

            case 'l':
                long_ctr++;
                /* Fall through */
            case 'h':
                /* FIXME: do nothing for these modifiers */
                goto still_might_format;

            case 'o':
            case 'd':
            case 'i':
            case 'u':
            case 'p':
            case 'x':
            case 'X':
            case 'c': {
                char buf[64];
                PREPARE_TEMP_FORMAT();

                if (long_ctr < 2) {
                    int32_t d;

                    CHECK_VA_ARG(ap, uint32_t);
                    d = _va_arg(ap, int32_t);

                    if (long_ctr == 1) {
                        uint32_t fmt_end_idx = (uint32_t)(fmt - fmt_start_addr);

                        if (fmt_buf[fmt_end_idx - 1] == 'l'
                            || fmt_buf[fmt_end_idx - 1] == 'z'
                            || fmt_buf[fmt_end_idx - 1] == 't') {
                            /* The %ld, %zd and %td should be treated as
                             * 32bit integer in wasm */
                            fmt_buf[fmt_end_idx - 1] = fmt_buf[fmt_end_idx];
                            fmt_buf[fmt_end_idx] = '\0';
                        }
                    }

                    n = snprintf(buf, sizeof(buf), fmt_buf, d);
                } else {
                    int64_t lld;

                    /* Make 8-byte aligned */
                    ap = (_va_list)(((uintptr_t)ap + 7) & ~(uintptr_t)7);
                    CHECK_VA_ARG(ap, uint64_t);
                    lld = _va_arg(ap, int64_t);
                    n = snprintf(buf, sizeof(buf), fmt_buf, lld);
                }

                OUTPUT_TEMP_FORMAT();
                break;
            }

            case 's': {
                char buf_tmp[128], *buf = buf_tmp;
                char* start;
                uint32_t s_offset, str_len, buf_len;

                PREPARE_TEMP_FORMAT();

                CHECK_VA_ARG(ap, int32_t);
                s_offset = _va_arg(ap, uint32_t);

                if (!wasm_runtime_validate_app_str_addr(module_inst, s_offset)) {
                    if (fmt_buf != temp_fmt) {
                        free(fmt_buf);
                    }
                    return false;
                }

                s = start = addr_app_to_native((uint64_t)s_offset);

                str_len = (uint32_t)strlen(start);
                if (str_len >= UINT32_MAX - 64) {
                    print_err(out, ctx);
                    if (fmt_buf != temp_fmt) {
                        free(fmt_buf);
                    }
                    break;
                }

                /* reserve 64 more bytes as there may be width description
                 * in the fmt */
                buf_len = str_len + 64;

                if (buf_len > (uint32_t)sizeof(buf_tmp)) {
                    if (!(buf = malloc(buf_len))) {
                        print_err(out, ctx);
                        if (fmt_buf != temp_fmt) {
                            free(fmt_buf);
                        }
                        break;
                    }
                }

                n = snprintf(buf, buf_len, fmt_buf,
                    (s_offset == 0 && str_len == 0) ? NULL
                                                    : start);

                OUTPUT_TEMP_FORMAT();

                if (buf != buf_tmp) {
                    free(buf);
                }

                break;
            }

            case '%': {
                out((int)'%', ctx);
                break;
            }

            case 'e':
            case 'E':
            case 'g':
            case 'G':
            case 'f':
            case 'F': {
                double f64;
                char buf[64];
                PREPARE_TEMP_FORMAT();

                /* Make 8-byte aligned */
                ap = (_va_list)(((uintptr_t)ap + 7) & ~(uintptr_t)7);
                CHECK_VA_ARG(ap, double);
                f64 = _va_arg(ap, double);
                n = snprintf(buf, sizeof(buf), fmt_buf, f64);

                OUTPUT_TEMP_FORMAT();
                break;
            }

            case 'n':
                /* print nothing */
                break;

            default:
                out((int)'%', ctx);
                out((int)*fmt, ctx);
                break;
            }

            might_format = 0;
        }

    still_might_format:
        ++fmt;
    }
    return true;

fail:
    wasm_runtime_set_exception(module_inst, "out of bounds memory access");
    return false;
}

#ifndef BUILTIN_LIBC_BUFFERED_PRINTF
#define BUILTIN_LIBC_BUFFERED_PRINTF 0
#endif

#ifndef BUILTIN_LIBC_BUFFERED_PRINT_SIZE
#define BUILTIN_LIBC_BUFFERED_PRINT_SIZE 128
#endif

struct str_context {
    char* str;
    uint32_t max;
    uint32_t count;
};

static int
sprintf_out(int c, struct str_context* ctx)
{
    if (!ctx->str || ctx->count >= ctx->max) {
        ctx->count++;
        return c;
    }

    if (ctx->count == ctx->max - 1) {
        ctx->str[ctx->count++] = '\0';
    } else {
        ctx->str[ctx->count++] = (char)c;
    }

    return c;
}

static sem_t g_aligned_memory_map_sem = SEM_INITIALIZER(1);
static uintptr_t g_aligned_memory_map
    [CONFIG_LIBC_BYPASS_POSIXMEMALIGN_MAP_SIZE][2]
    = {
          0
      };

static bool
add_to_aligned_map(uintptr_t mapped, uintptr_t raw)
{
    int i;
    bool ret = false;
    sem_wait(&g_aligned_memory_map_sem);
    for (i = 0; i < CONFIG_LIBC_BYPASS_POSIXMEMALIGN_MAP_SIZE;
         i++) {
        DEBUGASSERT(g_aligned_memory_map[i][0] != mapped);
        if (g_aligned_memory_map[i][0] == 0 && g_aligned_memory_map[i][1] == 0) {
            g_aligned_memory_map[i][0] = mapped;
            g_aligned_memory_map[i][1] = raw;
            ret = true;
            break;
        }
    }
    sem_post(&g_aligned_memory_map_sem);
    return ret;
}

static uintptr_t
remove_from_aligned_map(uintptr_t mapped)
{
    int i;
    uintptr_t ret = 0;
    if (mapped == 0) {
        return ret;
    }
    sem_wait(&g_aligned_memory_map_sem);
    for (i = 0; i < CONFIG_LIBC_BYPASS_POSIXMEMALIGN_MAP_SIZE;
         i++) {
        if (g_aligned_memory_map[i][0] == mapped) {
            g_aligned_memory_map[i][0] = 0;
            ret = g_aligned_memory_map[i][1];
            g_aligned_memory_map[i][1] = 0;
            break;
        }
    }
    sem_post(&g_aligned_memory_map_sem);
    return ret;
}

/****************************************************************************
 * Private Functions
 ****************************************************************************/

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

static void
scanf_begin(wasm_module_inst_t module_inst, va_list ap)
{
    uintptr_t* apv = *(uintptr_t**)&ap;
    if (apv == NULL) {
        return;
    }
    while (*apv != 0) {
        *apv = (uintptr_t)addr_app_to_native(*apv);
        apv++;
    }
}

static void
scanf_end(wasm_module_inst_t module_inst, va_list ap)
{
    uintptr_t* apv = *(uintptr_t**)&ap;
    if (apv == NULL) {
        return;
    }
    while (*apv != 0) {
        *apv = (uintptr_t)addr_native_to_app((void*)*apv);
        apv++;
    }
}

static pthread_mutex_t g_compare_mutex = PTHREAD_MUTEX_INITIALIZER;
static wasm_exec_env_t g_compare_env;
static void* g_compare_func;

static int
compare_proxy(const void* a, const void* b)
{
    wasm_module_inst_t module_inst = get_module_inst(g_compare_env);
    uint32_t argv[2];
    argv[0] = addr_native_to_app((void*)a);
    argv[1] = addr_native_to_app((void*)b);
    return wasm_runtime_call_indirect(g_compare_env,
               (uint32_t)addr_native_to_app(g_compare_func), 2, argv)
        ? argv[0]
        : 0;
}

#ifndef GLUE_FUNCTION_qsort
#define GLUE_FUNCTION_qsort
void glue_qsort(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2,
    uintptr_t parm3, uintptr_t parm4)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    pthread_mutex_lock(&g_compare_mutex);
    g_compare_env = env;
    g_compare_func = parm4;
    qsort((FAR void*)parm1, (size_t)parm2,
        (size_t)parm3, compare_proxy);
    pthread_mutex_unlock(&g_compare_mutex);
}
#endif /* GLUE_FUNCTION_qsort */

#ifndef GLUE_FUNCTION_bsearch
#define GLUE_FUNCTION_bsearch
uintptr_t glue_bsearch(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2,
    uintptr_t parm3, uintptr_t parm4, uintptr_t parm5)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    pthread_mutex_lock(&g_compare_mutex);
    g_compare_env = env;
    g_compare_func = parm5;
    ret = bsearch((FAR const void*)parm1,
        (FAR const void*)parm2,
        (size_t)parm3, (size_t)parm4, compare_proxy);
    pthread_mutex_unlock(&g_compare_mutex);
    ret = addr_native_to_app((void*)ret);
    return ret;
}
#endif /* GLUE_FUNCTION_bsearch */

static void glue_msghdr_begin(wasm_module_inst_t module_inst,
    FAR struct msghdr* hdr)
{
    int i;
    hdr->msg_iov = addr_app_to_native((uintptr_t)hdr->msg_iov);
    for (i = 0; i < hdr->msg_iovlen; i++) {
        hdr->msg_iov[i].iov_base = addr_app_to_native((uintptr_t)hdr->msg_iov[i].iov_base);
    }
    if (hdr->msg_name != NULL && hdr->msg_namelen > 0) {
        hdr->msg_name = addr_app_to_native(hdr->msg_name);
    }
    if (hdr->msg_control != NULL && hdr->msg_controllen > 0) {
        hdr->msg_control = addr_app_to_native((uintptr_t)hdr->msg_control);
    }
}

static void glue_msghdr_end(wasm_module_inst_t module_inst,
    FAR struct msghdr* hdr)
{
    int i;
    for (i = 0; i < hdr->msg_iovlen; i++) {
        hdr->msg_iov[i].iov_base = addr_native_to_app((uintptr_t)hdr->msg_iov[i].iov_base);
    }
    hdr->msg_iov = addr_native_to_app((uintptr_t)hdr->msg_iov);
    if (hdr->msg_name != NULL && hdr->msg_namelen > 0) {
        hdr->msg_name = addr_native_to_app(hdr->msg_name);
    }
    if (hdr->msg_control != NULL && hdr->msg_controllen > 0) {
        hdr->msg_control = addr_native_to_app((uintptr_t)hdr->msg_control);
    }
}

#ifndef GLUE_FUNCTION_sendmsg
#define GLUE_FUNCTION_sendmsg
uintptr_t glue_sendmsg(wasm_exec_env_t env, uintptr_t parm1,
    uintptr_t parm2, uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    FAR struct msghdr* hdr = (FAR struct msghdr*)(uintptr_t)parm2;
    glue_msghdr_begin(module_inst, hdr);
    uintptr_t ret = sendmsg((int)parm1,
        (FAR struct msghdr*)(parm2),
        (int)parm3);
    glue_msghdr_end(module_inst, hdr);
    return ret;
}
#endif /* GLUE_FUNCTION_sendmsg */

#ifndef GLUE_FUNCTION_recvmsg
#define GLUE_FUNCTION_recvmsg
uintptr_t glue_recvmsg(wasm_exec_env_t env, uintptr_t parm1,
    uintptr_t parm2, uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    FAR struct msghdr* hdr = (FAR struct msghdr*)((uintptr_t)parm2);
    glue_msghdr_begin(module_inst, hdr);
    uintptr_t ret = recvmsg((int)parm1,
        (FAR struct msghdr*)(parm2),
        (int)parm3);
    glue_msghdr_end(module_inst, hdr);
    return ret;
}
#endif /* GLUE_FUNCTION_recvmsg */

#ifndef GLUE_FUNCTION_strsep
#define GLUE_FUNCTION_strsep
uintptr_t glue_strsep(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    FAR char** stringp = parm1;
    if (*stringp != NULL) {
        *stringp = addr_app_to_native(*stringp);
    }
    return addr_native_to_app((uintptr_t)strsep(
        (FAR char**)addr_app_to_native(parm1),
        (FAR const char*)addr_app_to_native(parm2)));
}
#endif /* GLUE_FUNCTION_strsep */

#ifndef GLUE_FUNCTION_scandir
#define GLUE_FUNCTION_scandir
uintptr_t glue_scandir(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2,
    uintptr_t parm3, uintptr_t parm4)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    int ret = 0;
    int i = 0;
    ret = scandir((FAR const char*)parm1,
        (FAR struct dirent***)parm2,
        (FAR void*)parm3, alphasort);
    for (i = 0; i < ret; i++) {
        (*(uintptr_t**)parm2)[i] = addr_native_to_app((*(uintptr_t**)parm2)[i]);
    }
    *(uintptr_t*)parm2 = addr_native_to_app(*(uintptr_t*)parm2);
    return ret;
}
#endif /* GLUE_FUNCTION_scandir */

#ifndef GLUE_FUNCTION_daemon
#define GLUE_FUNCTION_daemon
uintptr_t glue_daemon(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2)
{
    return 0;
}
#endif /* GLUE_FUNCTION_daemon */

#ifndef GLUE_FUNCTION_posix_memalign
#define GLUE_FUNCTION_posix_memalign
uintptr_t glue_posix_memalign(wasm_exec_env_t env, uintptr_t parm1,
    uintptr_t parm2, uintptr_t parm3)
{
    uintptr_t rawptr;
    wasm_module_inst_t module_inst = get_module_inst(env);
    /* Extra size for align */
    parm3 += parm2;
    int ret = posix_memalign((FAR void**)parm1, (size_t)parm2, (size_t)parm3);
    /* If the memory allocation is failed, return NULL */
    if (ret != OK) {
        return NULL;
    }
    /* Add the original pointer to the map */
    rawptr = *(uintptr_t*)parm1;
    *(void**)parm1 = addr_native_to_app((uintptr_t) * (void**)parm1);
    *(uintptr_t*)parm1 = (*(uintptr_t*)parm1 + parm2 - 1) & ~(parm2 - 1);
    if (add_to_aligned_map(*(uintptr_t*)parm1, rawptr)) {
        return ret;
    } else {
        free(rawptr);
        return NULL;
    }
}
#endif /* GLUE_FUNCTION_posix_memalign */

#ifndef GLUE_FUNCTION_free
#define GLUE_FUNCTION_free
void glue_free(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret = NULL;
    uintptr_t app_addr = NULL;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app) {
        parm1 = (uintptr_t)NULL;
    }

    /* Try to pop the original pointer from the map */
    if (parm1 != (uintptr_t)NULL) {
        app_addr = addr_native_to_app(parm1);
        ret = remove_from_aligned_map(app_addr);
    }
    if (ret) {
        parm1 = ret;
    }
    free((FAR void*)parm1);
}
#endif /* GLUE_FUNCTION_free */

#ifndef GLUE_FUNCTION_vasprintf
#define GLUE_FUNCTION_vasprintf
uintptr_t glue_vasprintf(wasm_exec_env_t env, uintptr_t parm1,
    uintptr_t format, va_list ap)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    va_list_string2native(env, format, ap);
    ret = vasprintf((FAR char**)parm1, (FAR const IPTR char*)format, ap);
    *(uintptr_t*)parm1 = addr_native_to_app(*(uintptr_t*)parm1);
    return ret;
}
#endif /* GLUE_FUNCTION_vasprintf */

#ifndef GLUE_FUNCTION_strtol
#define GLUE_FUNCTION_strtol
uintptr_t glue_strtol(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2,
    uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    *(uintptr_t*)parm2 = addr_app_to_native(*(uintptr_t*)parm2);
    ret = strtol((FAR const char*)parm1, (FAR char**)parm2, (int)parm3);
    *(uintptr_t*)parm2 = addr_native_to_app(*(uintptr_t*)parm2);
    return ret;
}
#endif /* GLUE_FUNCTION_strtol */

#if defined(CONFIG_HAVE_LONG_LONG)
#ifndef GLUE_FUNCTION_strtoll
#define GLUE_FUNCTION_strtoll
uintptr_t glue_strtoll(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2,
    uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    *(uintptr_t*)parm2 = addr_app_to_native(*(uintptr_t*)parm2);
    ret = strtoll((FAR const char*)parm1, (FAR char**)parm2, (int)parm3);
    *(uintptr_t*)parm2 = addr_native_to_app(*(uintptr_t*)parm2);
    return ret;
}
#endif /* GLUE_FUNCTION_strtoll */
#endif

#ifndef GLUE_FUNCTION_strtoul
#define GLUE_FUNCTION_strtoul
uintptr_t glue_strtoul(wasm_exec_env_t env, uintptr_t parm1,
    uintptr_t parm2, uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    *(uintptr_t*)parm2 = addr_app_to_native(*(uintptr_t*)parm2);
    ret = strtoul((FAR const char*)parm1, (FAR char**)parm2, (int)parm3);
    *(uintptr_t*)parm2 = addr_native_to_app(*(uintptr_t*)parm2);
    return ret;
}
#endif /* GLUE_FUNCTION_strtoul */

#ifndef GLUE_FUNCTION_strtoull
#define GLUE_FUNCTION_strtoull
uintptr_t glue_strtoull(wasm_exec_env_t env, uintptr_t parm1,
    uintptr_t parm2, uintptr_t parm3)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    *(uintptr_t*)parm2 = addr_app_to_native(*(uintptr_t*)parm2);
    ret = strtoull((FAR const char*)parm1, (FAR char**)parm2, (int)parm3);
    *(uintptr_t*)parm2 = addr_native_to_app(*(uintptr_t*)parm2);
    return ret;
}
#endif /* GLUE_FUNCTION_strtoull */

#if defined(CONFIG_LIBC_LOCALE)
#ifndef GLUE_FUNCTION_iconv
#define GLUE_FUNCTION_iconv
uintptr_t glue_iconv(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2,
    uintptr_t parm3, uintptr_t parm4, uintptr_t parm5)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    *(uintptr_t*)parm2 = addr_app_to_native(*(uintptr_t*)parm2);
    *(uintptr_t*)parm4 = addr_app_to_native(*(uintptr_t*)parm4);
    ret = iconv((iconv_t)parm1, (FAR char**)parm2, (FAR size_t*)parm3,
        (FAR char**)parm4, (FAR size_t*)parm5);
    *(uintptr_t*)parm2 = addr_native_to_app(*(uintptr_t*)parm2);
    *(uintptr_t*)parm4 = addr_native_to_app(*(uintptr_t*)parm4);
    return ret;
}
#endif /* GLUE_FUNCTION_iconv */
#endif /* defined(CONFIG_LIBC_LOCALE) */

#ifndef GLUE_FUNCTION_versionsort
#define GLUE_FUNCTION_versionsort
uintptr_t glue_versionsort(wasm_exec_env_t env, uintptr_t parm1,
    uintptr_t parm2)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    *(uintptr_t*)parm1 = addr_app_to_native(*(uintptr_t*)parm1);
    *(uintptr_t*)parm2 = addr_app_to_native(*(uintptr_t*)parm2);
    ret = versionsort((FAR const struct dirent**)parm1,
        (FAR const struct dirent**)parm2);
    *(uintptr_t*)parm1 = addr_native_to_app(*(uintptr_t*)parm1);
    *(uintptr_t*)parm2 = addr_native_to_app(*(uintptr_t*)parm2);
    return ret;
}
#endif /* GLUE_FUNCTION_versionsort */

#if !defined(CONFIG_DISABLE_PTHREAD)
#ifndef GLUE_FUNCTION_pthread_once
#define GLUE_FUNCTION_pthread_once

static wasm_exec_env_t pthread_once_env;
static int init_routine_idx;

void init_routine_proxy(void)
{
    wasm_module_inst_t module_inst = get_module_inst(pthread_once_env);
    uint32_t argv[1];

    wasm_runtime_call_indirect(pthread_once_env, init_routine_idx, 0, argv);
}

uintptr_t glue_pthread_once(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    pthread_once_env = env;
    init_routine_idx = parm2;

    ret = pthread_once((FAR pthread_once_t*)parm1, init_routine_proxy);
    return ret;
}
#endif /* GLUE_FUNCTION_pthread_once */
#endif /* !defined(CONFIG_DISABLE_PTHREAD) */

#ifndef GLUE_FUNCTION_printf
#define GLUE_FUNCTION_printf
uintptr_t glue_printf(wasm_exec_env_t env, uintptr_t format, va_list ap)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    /* TODO: handle %.*s */
    if (strstr(format, "%.*s") != NULL) {
        return 0;
    }
    va_list_string2native(env, format, ap);
    ret = vprintf((FAR const IPTR char*)format, ap);
    va_list_string2app(env, format, ap);
    return ret;
}
#endif /* GLUE_FUNCTION_printf */

#ifndef GLUE_FUNCTION_snprintf
#define GLUE_FUNCTION_snprintf
int glue_snprintf(wasm_exec_env_t exec_env, char* str, uint32_t size,
    const char* format, _va_list va_args)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    struct str_context ctx;

    /* str and format have been checked by runtime */
    if (!validate_native_addr(va_args, (uint64_t)sizeof(uint32_t)))
        return 0;

    ctx.str = str;
    ctx.max = size;
    ctx.count = 0;

    if (!_vprintf_wa((out_func_t)sprintf_out, &ctx, format, va_args,
            module_inst))
        return 0;

    if (ctx.count < ctx.max) {
        str[ctx.count] = '\0';
    }

    return (int)ctx.count;
}
#endif /* GLUE_FUNCTION_snprintf */

#ifndef GLUE_FUNCTION_sprintf
#define GLUE_FUNCTION_sprintf
int glue_sprintf(wasm_exec_env_t exec_env, char* str, const char* format,
    _va_list va_args)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    struct str_context ctx;

    /* str and format have been checked by runtime */
    if (!validate_native_addr(va_args, (uint64_t)sizeof(uint32_t)))
        return 0;

    ctx.str = str;
    ctx.max = INT_MAX;
    ctx.count = 0;

    if (!_vprintf_wa((out_func_t)sprintf_out, &ctx, format, va_args,
            module_inst))
        return 0;

    if (ctx.count < ctx.max) {
        str[ctx.count] = '\0';
    }

    return (int)ctx.count;
}
#endif /* GLUE_FUNCTION_sprintf */

#ifndef GLUE_FUNCTION_strtod
#define GLUE_FUNCTION_strtod
uintptr_t glue_strtod(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    if ((void*)parm2 == addr_app)
        parm2 = (uintptr_t)NULL;

    char** end = (char**)parm2;

    ret = strtod((FAR const char*)parm1, (FAR char**)end);
    if (end != NULL) {
        *end = addr_native_to_app(*end);
    }

    return ret;
}
#endif /* GLUE_FUNCTION_strtod */

#ifndef GLUE_FUNCTION_pow
#define GLUE_FUNCTION_pow
double glue_pow(wasm_exec_env_t env, double parm1, double parm2)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    double ret;
    ret = pow((double)parm1, (double)parm2);
    return ret;
}
#endif /* GLUE_FUNCTION_pow */

#ifndef GLUE_FUNCTION_mq_open
#define GLUE_FUNCTION_mq_open
mqd_t glue_mq_open(wasm_exec_env_t env, const char* mq_name, int oflag, _va_list ap)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    int ret;
    if ((oflag & O_CREAT) != 0) {
        mode_t mode = _va_arg(ap, mode_t);
        uint32_t attr = _va_arg(ap, uint32_t);
        void* attr1 = (void*)addr_app_to_native(attr);
        ret = mq_open((FAR const char*)mq_name, oflag, mode, attr1);
        return (int)ret;
    }

    ret = mq_open((FAR const char*)mq_name, oflag);
    return (int)ret;
}
#endif /* GLUE_FUNCTION_mq_open */

#ifndef GLUE_FUNCTION_open
#define GLUE_FUNCTION_open
int glue_open(wasm_exec_env_t env, const char* path, int flags, _va_list ap)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    int ret;
    if ((flags & O_CREAT) != 0) {
        mode_t mode = _va_arg(ap, mode_t);
        ret = open((FAR const char*)path, flags, mode);
        return (int)ret;
    }

    ret = open((FAR const char*)path, flags);
    return ret;
}
#endif /* GLUE_FUNCTION_open */

#if defined(CONFIG_HAVE_DOUBLE) && !defined(CONFIG_LIBM_NONE)
#ifndef GLUE_FUNCTION_fmod
#define GLUE_FUNCTION_fmod
double glue_fmod(wasm_exec_env_t env, double parm1, double parm2)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    double ret;
    ret = fmod((double)parm1, (double)parm2);
    return ret;
}
#endif /* GLUE_FUNCTION_fmod */
#endif /* defined(CONFIG_HAVE_DOUBLE) && !defined(CONFIG_LIBM_NONE) */

#if defined(CONFIG_HAVE_DOUBLE) && !defined(CONFIG_LIBM_NONE)
#ifndef GLUE_FUNCTION_cos
#define GLUE_FUNCTION_cos
double glue_cos(wasm_exec_env_t env, double parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    double ret;
    ret = cos((double)parm1);
    return ret;
}

#endif /* GLUE_FUNCTION_cos */
#endif /* defined(CONFIG_HAVE_DOUBLE) && !defined(CONFIG_LIBM_NONE) */

#if defined(CONFIG_HAVE_DOUBLE) && !defined(CONFIG_LIBM_NONE)
#ifndef GLUE_FUNCTION_sin
#define GLUE_FUNCTION_sin
double glue_sin(wasm_exec_env_t env, double parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    double ret;
    ret = sin((double)parm1);
    return ret;
}
#endif /* GLUE_FUNCTION_sin */
#endif /* defined(CONFIG_HAVE_DOUBLE) && !defined(CONFIG_LIBM_NONE) */

#if defined(CONFIG_HAVE_DOUBLE) && !defined(CONFIG_LIBM_NONE)
#ifndef GLUE_FUNCTION_acos
#define GLUE_FUNCTION_acos
double glue_acos(wasm_exec_env_t env, double parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    double ret;
    ret = acos((double)parm1);
    return ret;
}
#endif /* GLUE_FUNCTION_acos */
#endif /* defined(CONFIG_HAVE_DOUBLE) && !defined(CONFIG_LIBM_NONE) */

#if defined(CONFIG_HAVE_DOUBLE) && !defined(CONFIG_LIBM_NONE)
#ifndef GLUE_FUNCTION_asin
#define GLUE_FUNCTION_asin
double glue_asin(wasm_exec_env_t env, double parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    double ret;
    ret = asin((double)parm1);
    return ret;
}
#endif /* GLUE_FUNCTION_asin */
#endif /* defined(CONFIG_HAVE_DOUBLE) && !defined(CONFIG_LIBM_NONE) */

#if defined(CONFIG_HAVE_DOUBLE) && !defined(CONFIG_LIBM_NONE)
#ifndef GLUE_FUNCTION_atan2
#define GLUE_FUNCTION_atan2
double glue_atan2(wasm_exec_env_t env, double parm1, double parm2)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    double ret;
    ret = atan2((double)parm1, (double)parm2);
    return ret;
}
#endif /* GLUE_FUNCTION_atan2 */
#endif /* defined(CONFIG_HAVE_DOUBLE) && !defined(CONFIG_LIBM_NONE) */

#if defined(CONFIG_HAVE_DOUBLE) && !defined(CONFIG_LIBM_NONE)

#ifndef GLUE_FUNCTION_ldexp
#define GLUE_FUNCTION_ldexp
double glue_ldexp(wasm_exec_env_t env, double parm1, int parm2)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    double ret;
    ret = ldexp((double)parm1, (int)parm2);
    return ret;
}
#endif /* GLUE_FUNCTION_ldexp */
#endif /* defined(CONFIG_HAVE_DOUBLE) && !defined(CONFIG_LIBM_NONE) */

#if defined(CONFIG_NET_IPv4)

#ifndef GLUE_FUNCTION_inet_ntoa
#define GLUE_FUNCTION_inet_ntoa
uintptr_t glue_inet_ntoa(wasm_exec_env_t env, uintptr_t parm1)
{
    struct in_addr addr;
    addr.s_addr = parm1;

    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    ret = addr_native_to_app((uintptr_t)inet_ntoa(addr));
    return ret;
}

#endif /* GLUE_FUNCTION_inet_ntoa */
#endif /* defined(CONFIG_NET_IPv4) */

#ifndef GLUE_FUNCTION_ioctl
#define GLUE_FUNCTION_ioctl

static void libc_ioctl_args_conv(wasm_exec_env_t env, int first_arg_type,
    va_list ap, bool to_native)
{
    wasm_module_inst_t module_inst = get_module_inst(env);

    struct ifconf* ifc = va_arg(ap, struct ifconf*);
    if (!ifc) {
        return;
    } else {

        ifc = (uintptr_t)addr_app_to_native((uintptr_t)ifc);
    }

    void* addr_app = addr_app_to_native((uintptr_t)NULL);

    if (SIOCGIFCONF == first_arg_type) {
        if (to_native) {
            if (ifc == addr_app) {
                return;
            }

            if (ifc->ifc_req == NULL) {
                ifc->ifc_req = (uintptr_t)NULL;
            } else {
                ifc->ifc_req = (uintptr_t)addr_app_to_native((uintptr_t)ifc->ifc_req);
            }
        } else {

            if (ifc == NULL) {
                return;
            }

            if (ifc->ifc_req != NULL) {
                ifc->ifc_req = (uintptr_t)addr_native_to_app((uintptr_t)ifc->ifc_req);
            }
        }
    }
}

uintptr_t glue_ioctl(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2, va_list ap)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;

    libc_ioctl_args_conv(env, parm2, ap, true);

    ret = ioctl((int)parm1, (int)parm2, (*(uintptr_t**)&ap != NULL && **(uintptr_t**)&ap != (uintptr_t)NULL) ? (uintptr_t)addr_app_to_native((uintptr_t)va_arg(ap, unsigned long)) : (uintptr_t)NULL);

    libc_ioctl_args_conv(env, parm2, ap, false);

    return ret;
}

#endif /* GLUE_FUNCTION_ioctl */

#if defined(CONFIG_LIBC_NETDB)

#ifndef GLUE_FUNCTION_getaddrinfo
#define GLUE_FUNCTION_getaddrinfo

static void addrinfo2app(wasm_exec_env_t env, struct addrinfo* addr)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    struct addrinfo* cur = addr;
    struct addrinfo* tmp;

    while (cur) {
        cur->ai_canonname = addr_native_to_app(cur->ai_canonname);
        cur->ai_addr = addr_native_to_app(cur->ai_addr);
        tmp = cur->ai_next;
        cur->ai_next = addr_native_to_app(cur->ai_next);
        cur = tmp;
    }
}

uintptr_t glue_getaddrinfo(wasm_exec_env_t env, uintptr_t parm1, uintptr_t parm2, uintptr_t parm3, uintptr_t parm4)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    struct addrinfo** addr = (struct addrinfo**)parm4;

    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    if ((void*)parm2 == addr_app)
        parm2 = (uintptr_t)NULL;

    if ((void*)parm3 == addr_app)
        parm3 = (uintptr_t)NULL;

    if ((void*)parm4 == addr_app)
        parm4 = (uintptr_t)NULL;

    ret = getaddrinfo((FAR const char*)parm1, (FAR const char*)parm2, (FAR const struct addrinfo*)parm3, addr);

    addrinfo2app(env, *addr);
    *addr = addr_native_to_app(*addr);

    return ret;
}

#endif /* GLUE_FUNCTION_getaddrinfo */
#endif /* defined(CONFIG_LIBC_NETDB) */

static void ifaddrsinfo2app(wasm_exec_env_t env, struct ifaddrs* addr)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    struct ifaddrs* cur = addr;
    struct ifaddrs* tmp;

    while (cur) {
        cur->ifa_ifu.ifu_broadaddr = addr_native_to_app(cur->ifa_ifu.ifu_broadaddr);
        cur->ifa_addr = addr_native_to_app(cur->ifa_addr);
        cur->ifa_name = addr_native_to_app(cur->ifa_name);
        cur->ifa_data = addr_native_to_app(cur->ifa_data);
        cur->ifa_netmask = addr_native_to_app(cur->ifa_netmask);
        tmp = cur->ifa_next;
        cur->ifa_next = addr_native_to_app(cur->ifa_next);
        cur = tmp;
    }
}

static void ifaddrsinfo2native(wasm_exec_env_t env, struct ifaddrs* addr)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    struct ifaddrs* cur = addr;

    while (cur) {
        cur->ifa_ifu.ifu_broadaddr = addr_app_to_native(cur->ifa_ifu.ifu_broadaddr);
        cur->ifa_addr = addr_app_to_native(cur->ifa_addr);
        cur->ifa_data = addr_app_to_native(cur->ifa_data);
        cur->ifa_name = addr_app_to_native(cur->ifa_name);
        cur->ifa_netmask = addr_app_to_native(cur->ifa_netmask);

        if (cur->ifa_next == NULL) {
            break;
        }

        cur->ifa_next = addr_app_to_native(cur->ifa_next);
        cur = cur->ifa_next;
    }
}

#ifndef GLUE_FUNCTION_getifaddrs
#define GLUE_FUNCTION_getifaddrs

uintptr_t glue_getifaddrs(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    struct ifaddrs** addr = (struct ifaddrs**)parm1;

    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    ret = getifaddrs(addr);

    ifaddrsinfo2app(env, *addr);
    *addr = addr_native_to_app(*addr);

    return ret;
}

#endif /* GLUE_FUNCTION_getifaddrs */

#ifndef GLUE_FUNCTION_freeifaddrs
#define GLUE_FUNCTION_freeifaddrs
void glue_freeifaddrs(wasm_exec_env_t env, uintptr_t parm1)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    struct ifaddrs* addr = (struct ifaddrs*)parm1;

    void* addr_app = addr_app_to_native((uintptr_t)NULL);
    if ((void*)parm1 == addr_app)
        parm1 = (uintptr_t)NULL;

    ifaddrsinfo2native(env, addr);
    freeifaddrs(addr);

    addr = NULL;
}

#endif /* GLUE_FUNCTION_freeifaddrs */

#ifndef GLUE_FUNCTION_execl
#define GLUE_FUNCTION_execl
uintptr_t glue_execl(wasm_exec_env_t env, uintptr_t parm1, uintptr_t format, va_list ap)
{
    wasm_module_inst_t module_inst = get_module_inst(env);
    uintptr_t ret;
    va_list_string2native(env, format, ap);
    ret = execl((FAR const char*)parm1, (FAR const char*)format, ap, NULL);
    va_list_string2app(env, format, ap);
    return ret;
}

#endif /* GLUE_FUNCTION_execl */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include "libc_glue.c"
#include "libm_glue.c"
#include "syscall_glue.c"

/* Register module, this function is called by the WAMR runtime */

bool wamr_module_libc_bypass_register(void)
{
    bool ret;

    /* Add extra init hook here */

    ret = wasm_runtime_register_natives("env", g_syscall_native_symbols, nitems(g_syscall_native_symbols));
    if (!ret) {
        return ret;
    }

    ret = wasm_runtime_register_natives("env", g_libc_native_symbols, nitems(g_libc_native_symbols));
    if (!ret) {
        return ret;
    }

    ret = wasm_runtime_register_natives("env", g_libm_native_symbols, nitems(g_libm_native_symbols));
    if (!ret) {
        return ret;
    }

    return ret;
}
