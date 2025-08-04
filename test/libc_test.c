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

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>

#include "wasm_export.h"

#include "generated/libc_wasm_array.c"

/* Define the size of wasm_test_libc for API test */
#define WASM_TEST_HEAPSIZE 2048

extern bool wamr_module_libc_register(void);

static wasm_module_t g_module;
static wasm_module_inst_t g_module_instance;

/*
 * Calls a WASM function indirectly by its name.
 *
 * @param func_name The name of the WASM function to call.
 * @param argc The number of arguments.
 * @param argv The array of arguments.
 */
static void call_indirect(const char* func_name, int argc, uint32_t* argv)
{
    assert_non_null(func_name);
    assert_non_null(g_module);
    assert_non_null(g_module_instance);

    wasm_function_inst_t func = wasm_runtime_lookup_function(g_module_instance, func_name);
    assert_non_null(func);

    if (!wasm_runtime_call_wasm(wasm_runtime_get_exec_env_singleton(g_module_instance), func, argc, argv)) {
        fail_msg("%s", wasm_runtime_get_exception(g_module_instance));
    }
}

/* Define the malloc_wasm function */
void* malloc_wasm(size_t size)
{
    uint32_t value = (uint32_t)size;
    void* result;

    call_indirect("malloc_proxy", 1, &value);

    result = (void*)(uintptr_t)value;

    return result;
}

/* Define the calloc_wasm function */
void* calloc_wasm(size_t nmemb, size_t size)
{
    uint32_t value[2] = { (uint32_t)nmemb, (uint32_t)size };
    void* result;

    call_indirect("calloc_proxy", 2, value);

    result = (void*)(uintptr_t)value[0];

    return result;
}

/* Define the free_wasm function */
void free_wasm(void* ptr)
{
    /* ptr: wasm addr */
    uint32_t value = (uint32_t)(uintptr_t)ptr;

    call_indirect("free_proxy", 1, &value);
}

/* Define the memcmp_wasm function */
int memcmp_wasm(const void* s1, const void* s2, size_t n)
{
    char* t1 = (char*)malloc_wasm(strlen(s1) * sizeof(char));
    char* t2 = (char*)malloc_wasm(strlen(s2) * sizeof(char));
    char* na_t1 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)t1);
    char* na_t2 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)t2);
    strcpy(na_t1, s1);
    strcpy(na_t2, s2);

    uint32_t value[3] = { (uint32_t)(uintptr_t)t1, (uint32_t)(uintptr_t)t2, (uint32_t)n };

    call_indirect("memcmp_proxy", 3, value);

    free_wasm(t1);
    free_wasm(t2);

    return (int)value[0];
}

/* Define the strdup_wasm function */
char* strdup_wasm(const char* s)
{
    char* t = (char*)malloc_wasm(strlen(s) * sizeof(char));
    char* na_t = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)t);
    strcpy(na_t, s);
    uint32_t value = (uint32_t)(uintptr_t)t;

    call_indirect("strdup_proxy", 1, &value);

    /* malloc_wasm need free */
    free_wasm(t);
    return (char*)(uintptr_t)value;
}

/* Define the atoi_wasm function */
int atoi_wasm(const char* nptr)
{
    char* ptr = (char*)malloc_wasm(strlen(nptr) * sizeof(char));
    char* na_ptr = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr);
    strcpy(na_ptr, nptr);
    uint32_t value = (uint32_t)(uintptr_t)ptr;

    call_indirect("atoi_proxy", 1, &value);

    /* malloc_wasm need free */
    free_wasm(ptr);
    return (int)value;
}

/* Define the strtol_wasm function */
long int strtol_wasm(const char* nptr, char** endptr, int base)
{
    /* nptr\endptr: wasm addr */
    uint32_t value[3] = { (uint32_t)(uintptr_t)nptr, (uint32_t)(uintptr_t)endptr, (uint32_t)base };

    call_indirect("strtol_proxy", 3, value);

    return (long int)value[0];
}

/* Define the strtoul_wasm function */
unsigned long int strtoul_wasm(const char* nptr, char** endptr, int base)
{
    /* nptr\endptr: wasm addr */
    uint32_t value[3] = { (uint32_t)(uintptr_t)nptr, (uint32_t)(uintptr_t)endptr, (uint32_t)base };

    call_indirect("strtoul_proxy", 3, value);

    return (unsigned long int)value[0];
}

/* Define the memchr_wasm function */
void* memchr_wasm(const void* s, int c, size_t n)
{
    /* s: wasm addr */
    uint32_t value[3] = { (uint32_t)(uintptr_t)s, (uint32_t)c, (uint32_t)n };

    call_indirect("memchr_proxy", 3, value);

    void* result = (void*)wasm_runtime_addr_app_to_native(g_module_instance, (uint64_t)value[0]);

    return result;
}

/* Define the strncasecmp_wasm function */
int strncasecmp_wasm(const char* s1, const char* s2, size_t n)
{
    /* s1\s2: wasm addr */
    uint32_t value[3] = { (uint32_t)(uintptr_t)s1, (uint32_t)(uintptr_t)s2, (uint32_t)n };

    call_indirect("strncasecmp_proxy", 3, value);

    return (int)value[0];
}

/* Define the strspn_wasm function */
size_t strspn_wasm(const char* s, const char* accept)
{
    char* ptr1 = (char*)malloc_wasm(strlen(s) * sizeof(char));
    char* ptr2 = (char*)malloc_wasm(strlen(accept) * sizeof(char));

    char* na_ptr1 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr1);
    char* na_ptr2 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr2);

    strcpy(na_ptr1, s);
    strcpy(na_ptr2, accept);

    uint32_t value[2] = { (uint32_t)(uintptr_t)ptr1, (uint32_t)(uintptr_t)ptr2 };

    call_indirect("strspn_proxy", 2, value);

    free_wasm(ptr1);
    free_wasm(ptr2);

    return (size_t)value[0];
}

/* Define the strcspn_wasm function */
size_t strcspn_wasm(const char* s, const char* reject)
{
    char* ptr1 = (char*)malloc_wasm(strlen(s) * sizeof(char));
    char* ptr2 = (char*)malloc_wasm(strlen(reject) * sizeof(char));

    char* na_ptr1 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr1);
    char* na_ptr2 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr2);

    strcpy(na_ptr1, s);
    strcpy(na_ptr2, reject);

    uint32_t value[2] = { (uint32_t)(uintptr_t)ptr1, (uint32_t)(uintptr_t)ptr2 };

    call_indirect("strcspn_proxy", 2, value);

    free_wasm(ptr1);
    free_wasm(ptr2);

    return (size_t)value[0];
}

/* Define the strstr_wasm function */
char* strstr_wasm(const char* haystack, const char* needle)
{
    /* haystack\needle: wasm addr */
    uint32_t value[2] = { (uint32_t)(uintptr_t)haystack, (uint32_t)(uintptr_t)needle };

    call_indirect("strstr_proxy", 2, value);

    return (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uint64_t)value[0]);
}

/* Define the isupper_wasm function */
int isupper_wasm(int c)
{
    uint32_t value = c;

    call_indirect("isupper_proxy", 1, &value);

    return (int)value;
}

/* Define the isalpha_wasm function */
int isalpha_wasm(int c)
{
    uint32_t value = c;

    call_indirect("isalpha_proxy", 1, &value);

    return (int)value;
}

/* Define the isspace_wasm function */
int isspace_wasm(int c)
{
    uint32_t value = c;

    call_indirect("isspace_proxy", 1, &value);

    return (int)value;
}

/* Define the isgraph_wasm function */
int isgraph_wasm(int c)
{
    uint32_t value = c;

    call_indirect("isgraph_proxy", 1, &value);

    return (int)value;
}

/* Define the isprint_wasm function */
int isprint_wasm(int c)
{
    uint32_t value = c;

    call_indirect("isprint_proxy", 1, &value);

    return (int)value;
}

/* Define the isdigit_wasm function */
int isdigit_wasm(int c)
{
    uint32_t value = c;

    call_indirect("isdigit_proxy", 1, &value);

    return (int)value;
}

/* Define the isxdigit wasm function */
int isxdigit_proxy(int c)
{
    uint32_t value = c;

    call_indirect("isxdigit_proxy", 1, &value);

    return (int)value;
}

/* Define the tolower_wasm function */
int tolower_wasm(int c)
{
    uint32_t value = c;

    call_indirect("tolower_proxy", 1, &value);

    return (int)value;
}

/* Define the toupper_wasm function */
int toupper_wasm(int c)
{
    uint32_t value = c;

    call_indirect("toupper_proxy", 1, &value);

    return (int)value;
}

/* Define the isalnum_wasm function */
int isalnum_wasm(int c)
{
    uint32_t value = c;

    call_indirect("isalnum_proxy", 1, &value);

    return (int)value;
}

/* Define the nanosleep_wasm function */
int nanosleep_wasm(const struct timespec* req, struct timespec* rem)
{
    /* req/rem: wasm addr */
    uint32_t value[2] = { (uint32_t)(uintptr_t)req, (uint32_t)(uintptr_t)rem };

    call_indirect("nanosleep_proxy", 2, value);

    return (int)value[0];
}

/* Define the memcpy_wasm function */
void* memcpy_wasm(void* dest, const void* src, size_t n)
{
    /* dest/src: wasm addr */
    uint32_t value[3] = { (uint32_t)(uintptr_t)dest, (uint32_t)(uintptr_t)src, (uint32_t)n };
    void* result;

    call_indirect("memcpy_proxy", 3, value);
    result = (void*)wasm_runtime_addr_app_to_native(g_module_instance, (uint64_t)value[0]);

    return result;
}

/* Define the memmove_wasm function */
void* memmove_wasm(void* dest, const void* src, size_t n)
{
    /* dest/src: wasm addr */
    uint32_t value[3] = { (uint32_t)(uintptr_t)dest, (uint32_t)(uintptr_t)src, (uint32_t)n };
    void* result;

    call_indirect("memmove_proxy", 3, value);

    result = (void*)wasm_runtime_addr_app_to_native(g_module_instance, (uint64_t)value[0]);

    return result;
}

/* Define the memset_wasm function */
void* memset_wasm(void* s, int c, size_t n)
{
    /* s: wasm addr */
    uint32_t value[3] = { (uint32_t)(uintptr_t)s, (uint32_t)c, (uint32_t)n };
    void* result;

    call_indirect("memset_proxy", 3, value);

    result = (void*)wasm_runtime_addr_app_to_native(g_module_instance, (uint64_t)value[0]);

    return result;
}

/* Define the strchr_wasm function */
char* strchr_wasm(const char* s, int c)
{
    /* s: wasm addr */
    uint32_t value[2] = { (uint32_t)(uintptr_t)s, (uint32_t)c };
    void* result;

    call_indirect("strchr_proxy", 2, value);

    result = (void*)wasm_runtime_addr_app_to_native(g_module_instance, (uint64_t)value[0]);
    return result;
}

/* Define the strcmp_wasm function */
int strcmp_wasm(const char* s1, const char* s2)
{
    /* s1\s2: wasm addr */
    uint32_t value[2] = { (uint32_t)(uintptr_t)s1, (uint32_t)(uintptr_t)s2 };

    call_indirect("strcmp_proxy", 2, value);

    return (int)value[0];
}

/* Define the strncmp_wasm function */
int strncmp_wasm(const char* s1, const char* s2, size_t n)
{
    /* s1\s2: wasm addr */
    uint32_t value[3] = { (uint32_t)(uintptr_t)s1, (uint32_t)(uintptr_t)s2, (uint32_t)n };

    call_indirect("strncmp_proxy", 3, value);

    return (int)value[0];
}

/* Define the strcpy_wasm function */
char* strcpy_wasm(char* dest, const char* src)
{
    /* dest\src: wasm addr */
    uint32_t value[2] = { (uint32_t)(uintptr_t)dest, (uint32_t)(uintptr_t)src };
    void* result;

    call_indirect("strcpy_proxy", 2, value);

    result = (void*)wasm_runtime_addr_app_to_native(g_module_instance, (uint64_t)value[0]);
    return result;
}

/* Define the strncpy_wasm function */
char* strncpy_wasm(char* dest, const char* src, size_t n)
{
    /* dest\src: wasm addr */
    uint32_t value[3] = { (uint32_t)(uintptr_t)dest, (uint32_t)(uintptr_t)src, (uint32_t)n };
    void* result;

    call_indirect("strncpy_proxy", 3, value);

    result = (void*)wasm_runtime_addr_app_to_native(g_module_instance, (uint64_t)value[0]);
    return result;
}

/* Define the strlen_wasm function */
size_t strlen_wasm(const char* s)
{
    /* s: wasm addr */
    uint32_t value = (uint32_t)(uintptr_t)s;

    call_indirect("strlen_proxy", 1, &value);

    return (size_t)value;
}

/* Define the test case function for malloc */
void test_wasm_malloc(void** state)
{
    void* ptr = NULL;

    /* malloc size = 0, malloc return non_NULL pointer */
    size_t size = 0;
    ptr = malloc_wasm(size);
    assert_non_null(ptr);
    free_wasm(ptr);
    char* ptr1 = NULL;
    ptr1 = (char*)malloc_wasm(2 * sizeof(char));
    char* na_ptr1 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr1);

    assert_non_null(ptr1);

    na_ptr1[0] = 'a';
    na_ptr1[1] = 'b';
    assert_int_equal('a', na_ptr1[0]);
    assert_int_equal('b', na_ptr1[1]);

    free_wasm(ptr1);

    int* ptr2 = NULL;
    ptr2 = (int*)malloc_wasm(2 * sizeof(int));
    assert_non_null(ptr2);
    int* na_ptr2 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr2);

    na_ptr2[0] = 1;
    na_ptr2[1] = 2;
    assert_int_equal(1, na_ptr2[0]);
    assert_int_equal(2, na_ptr2[1]);

    free_wasm(ptr2);

    /* malloc size > heap, malloc return NULL pointer */
    size = 2052;
    ptr = malloc_wasm(size);
    assert_null(ptr);
}

/* Define the test case function for calloc */
void test_wasm_calloc(void** state)
{
    int* ptr = NULL;
    ptr = (int*)calloc_wasm(10, sizeof(int));
    assert_non_null(ptr);

    free_wasm(ptr);

    char* ptr1 = NULL;
    ptr1 = (char*)calloc_wasm(10, sizeof(char));
    assert_non_null(ptr1);

    free_wasm(ptr1);

    /* calloc: size > heap_size, return NULL */
    int* ptr2 = NULL;
    ptr2 = (int*)calloc_wasm(1024, sizeof(int));
    assert_null(ptr2);
}

/* Define the test case function for free */
void test_wasm_free(void** state)
{
    int result = 0;
    int* p1;
    p1 = (int*)malloc_wasm(sizeof(int));
    assert_non_null(p1);

    free_wasm(p1);
    result++;

    char* p2;
    p2 = (char*)malloc_wasm(15 * sizeof(char));
    assert_non_null(p2);

    free_wasm(p2);
    result++;

    assert_int_equal(2, result);
}

/* Define the test case function for memcmp */
void test_wasm_memcmp(void** state)
{
    const char* s1 = "abcdefghi";
    const char* s2 = "abcdef123";
    assert_int_equal(0, memcmp_wasm(s1, s2, 6));
    assert_true(memcmp_wasm(s1, s2, 9) != 0);

    const char* s3 = "abcde";
    const char* s4 = "12345";
    assert_true(memcmp_wasm(s3, s4, 5) != 0);
}

/* Define the test case function for strdup */
void test_wasm_strdup(void** state)
{
    char* ptr = strdup_wasm("abc");
    assert_non_null(ptr);
    char* na_ptr = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr);
    assert_int_equal('a', na_ptr[0]);
    assert_int_equal('b', na_ptr[1]);
    assert_int_equal('c', na_ptr[2]);

    char* ptr1 = strdup_wasm("a1b2c! ?");
    assert_non_null(ptr1);
    char* na_ptr1 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr1);
    assert_int_equal('a', na_ptr1[0]);
    assert_int_equal('1', na_ptr1[1]);
    assert_int_equal('2', na_ptr1[3]);
    assert_int_equal('!', na_ptr1[5]);
    assert_int_equal(' ', na_ptr1[6]);
}

/* Define the test case function for atoi */
void test_wasm_atoi(void** state)
{
    assert_int_equal(123, atoi_wasm("123"));
    assert_int_equal(-123, atoi_wasm("-123"));
    assert_int_equal(123, atoi_wasm("+123"));
    assert_int_equal(123, atoi_wasm("    123"));
    assert_int_equal(123, atoi_wasm("123abc"));
    assert_int_equal(0, atoi_wasm(""));
    assert_int_equal(2147483647, atoi_wasm("2147483647"));
    assert_int_equal(INT_MIN, atoi_wasm("-2147483649"));
    assert_int_equal(INT_MAX, atoi_wasm("2147483648"));
}

/* Define the test case function for strtol */
void test_wasm_strtol(void** state)
{
    /* case 1 */
    char* nptr1 = "123";
    char* ptr1 = (char*)malloc_wasm(strlen(nptr1) * sizeof(char));
    char* na_ptr1 = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr1);
    strcpy(na_ptr1, nptr1);

    assert_int_equal(123, strtol_wasm(ptr1, NULL, 10));

    free_wasm(ptr1);

    /* case 2 */
    char* nptr2 = "123abc";

    char** endptr2 = (char**)malloc_wasm(sizeof(char*));
    char* ptr2 = (char*)malloc_wasm(strlen(nptr2) * sizeof(char));

    char* na_ptr2 = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr2);
    char** na_endptr2 = (char**)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)endptr2);
    strcpy(na_ptr2, nptr2);
    *na_endptr2 = na_ptr2;

    assert_int_equal(123, strtol_wasm(ptr2, endptr2, 10));

    /* fi_ptr: the pointer of endptr after strtol */
    char* fi_ptr2 = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)*na_endptr2);
    assert_int_equal('a', fi_ptr2[0]);
    assert_int_equal('b', fi_ptr2[1]);
    assert_int_equal('c', fi_ptr2[2]);
    free_wasm(ptr2);
    free_wasm(endptr2);
}

/* Define the test case function for strtoul */
void test_wasm_strtoul(void** state)
{
    /* case 1 */
    char* nptr1 = "12345";
    char* ptr1 = (char*)malloc_wasm(strlen(nptr1) * sizeof(char));
    char* na_ptr1 = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr1);
    strcpy(na_ptr1, nptr1);

    assert_int_equal(12345, strtoul_wasm(ptr1, NULL, 10));

    free_wasm(ptr1);

    /* case 2 */
    char* nptr2 = "12453def";

    char** endptr2 = (char**)malloc_wasm(sizeof(char*));
    char* ptr2 = (char*)malloc_wasm(strlen(nptr2) * sizeof(char));

    char* na_ptr2 = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr2);
    char** na_endptr2 = (char**)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)endptr2);
    strcpy(na_ptr2, nptr2);
    *na_endptr2 = na_ptr2;

    assert_int_equal(12453, strtoul_wasm(ptr2, endptr2, 10));

    /* fi_ptr2: the pointer of endptr after strtol */
    char* fi_ptr2 = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)*na_endptr2);
    assert_int_equal('d', fi_ptr2[0]);
    assert_int_equal('e', fi_ptr2[1]);
    assert_int_equal('f', fi_ptr2[2]);
    free_wasm(ptr2);
    free_wasm(endptr2);
}

/* Define the test case function for memchr */
void test_wasm_memchr(void** state)
{
    char* p;
    const char* s = "Hello World!";

    char* ptr = (char*)malloc_wasm(strlen(s) * sizeof(char));
    char* na_ptr = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr);
    strcpy(na_ptr, s);

    p = memchr_wasm(ptr, 'H', 5);
    assert_int_equal(*p, 'H');

    p = memchr_wasm(ptr, 'd', 11);
    assert_int_equal(*p, 'd');
}

/* Define the test case function for strncasecmp */
void test_wasm_strncasecmp(void** state)
{
    /* case 1 */
    const char* s1 = "abcde123G";
    const char* s2 = "AbCde123f";
    char* ptr1 = (char*)malloc_wasm(strlen(s1) * sizeof(char));
    char* ptr2 = (char*)malloc_wasm(strlen(s2) * sizeof(char));

    char* na_ptr1 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr1);
    char* na_ptr2 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr2);

    strcpy(na_ptr1, s1);
    strcpy(na_ptr2, s2);

    assert_int_equal(0, strncasecmp_wasm(ptr1, ptr2, 0));
    assert_int_equal(0, strncasecmp_wasm(ptr1, ptr2, 6));
    assert_int_equal(0, strncasecmp_wasm(ptr1, ptr2, 8));
    assert_true(strncasecmp_wasm(ptr1, ptr2, 9) != 0);

    free_wasm(ptr1);
    free_wasm(ptr2);

    /* case 2 */
    const char* s3 = "abcde";
    const char* s4 = "ABCDF";
    char* ptr3 = (char*)malloc_wasm(strlen(s3) * sizeof(char));
    char* ptr4 = (char*)malloc_wasm(strlen(s4) * sizeof(char));

    char* na_ptr3 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr3);
    char* na_ptr4 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr4);

    strcpy(na_ptr3, s3);
    strcpy(na_ptr4, s4);

    assert_int_equal(-1, strncasecmp_wasm(ptr3, ptr4, 5));
    assert_int_equal(1, strncasecmp_wasm(ptr4, ptr3, 5));

    free_wasm(ptr3);
    free_wasm(ptr4);
}

/* Define the test case function for strspn */
void test_wasm_strspn(void** state)
{
    const char* s1 = "Hello World!";
    const char* a1 = "Hello";
    assert_int_equal(5, strspn_wasm(s1, a1));

    const char* s2 = "Hello World!";
    const char* a2 = "World";
    assert_int_equal(0, strspn_wasm(s2, a2));

    const char* s3 = "ab";
    const char* a3 = "abc";
    assert_int_equal(2, strspn_wasm(s3, a3));
}

/* Define the test case function for strcspn */
void test_wasm_strcspn(void** state)
{
    const char* s1 = "Hello, World!";
    const char* reject1 = ",!?";
    assert_int_equal(5, strcspn_wasm(s1, reject1));

    const char* s2 = "Hello, World!";
    const char* reject2 = "123";
    assert_int_equal(strlen(s2), strcspn_wasm(s2, reject2));

    const char* s3 = "ab";
    const char* reject3 = "cdef";
    assert_int_equal(strlen(s3), strcspn_wasm(s3, reject3));
}

/* Define the test case function for strstr */
void test_wasm_strstr(void** state)
{
    char* str;
    const char* haystack = "Hello, World!";
    const char* needle = "World";

    char* ptr1 = (char*)malloc_wasm(strlen(haystack) * sizeof(char));
    char* ptr2 = (char*)malloc_wasm(strlen(needle) * sizeof(char));

    char* na_ptr1 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr1);
    char* na_ptr2 = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)ptr2);

    strcpy(na_ptr1, haystack);
    strcpy(na_ptr2, needle);

    str = strstr_wasm(ptr1, ptr2);

    assert_non_null(str);
    assert_int_equal('W', str[0]);
    assert_int_equal('o', str[1]);
    assert_int_equal('r', str[2]);
    assert_int_equal('l', str[3]);
    assert_int_equal('d', str[4]);

    free_wasm(ptr1);
    free_wasm(ptr2);
}

/* Define the test case function for isupper */
void test_wasm_isupper(void** state)
{
    assert_int_equal(0, isupper_wasm('w'));
    assert_int_equal(0, isupper_wasm('a'));
    assert_int_equal(1, isupper_wasm('S'));
    assert_int_equal(1, isupper_wasm('M'));
    assert_int_equal(0, isupper_wasm('1'));
    assert_int_equal(0, isupper_wasm('!'));
    assert_int_equal(0, isupper_wasm('['));
}

/* Define the test case function for isalpha */
void test_wasm_isalpha(void** state)
{
    assert_int_equal(1, isalpha_wasm('w'));
    assert_int_equal(1, isalpha_wasm('A'));
    assert_int_equal(1, isalpha_wasm('z'));
    assert_int_equal(0, isalpha_wasm('1'));
    assert_int_equal(0, isalpha_wasm('!'));
    assert_int_equal(0, isalpha_wasm(' '));
    assert_int_equal(0, isalpha_wasm('\n'));
}

/* Define the test case function for isspace */
void test_wasm_isspace(void** state)
{
    assert_int_equal(1, isspace_wasm(' '));
    assert_int_equal(1, isspace_wasm('\t'));
    assert_int_equal(1, isspace_wasm('\n'));
    assert_int_equal(1, isspace_wasm('\r'));
    assert_int_equal(1, isspace_wasm('\f'));
    assert_int_equal(1, isspace_wasm('\v'));
    assert_int_equal(0, isspace_wasm('a'));
    assert_int_equal(0, isspace_wasm('1'));
    assert_int_equal(0, isspace_wasm('!'));
}

/* Define the test case function for isgraph */
void test_wasm_isgraph(void** state)
{
    assert_int_equal(1, isgraph_wasm('a'));
    assert_int_equal(1, isgraph_wasm('Z'));
    assert_int_equal(1, isgraph_wasm('1'));
    assert_int_equal(1, isgraph_wasm('!'));
    assert_int_equal(0, isgraph_wasm(' '));
    assert_int_equal(0, isgraph_wasm('\n'));
    assert_int_equal(1, isgraph_wasm('~'));
}

/* Define the test case function for isprint */
void test_wasm_isprint(void** state)
{
    assert_int_equal(1, isprint_wasm('a'));
    assert_int_equal(1, isprint_wasm('Z'));
    assert_int_equal(1, isprint_wasm('1'));
    assert_int_equal(1, isprint_wasm('!'));
    assert_int_equal(1, isprint_wasm(' '));
    assert_int_equal(0, isprint_wasm('\n'));
    assert_int_equal(1, isprint_wasm('~'));
}

/* Define the test case function for isdigit */
void test_wasm_isdigit(void** state)
{
    assert_int_equal(1, isdigit_wasm('0'));
    assert_int_equal(1, isdigit_wasm('1'));
    assert_int_equal(1, isdigit_wasm('9'));
    assert_int_equal(0, isdigit_wasm('a'));
    assert_int_equal(0, isdigit_wasm('Z'));
    assert_int_equal(0, isdigit_wasm(' '));
    assert_int_equal(0, isdigit_wasm('@'));
}

/* Define the test case function for isxdigit */
void test_wasm_isxdigit(void** state)
{
    assert_int_equal(1, isxdigit_proxy('0'));
    assert_int_equal(1, isxdigit_proxy('9'));
    assert_int_equal(1, isxdigit_proxy('a'));
    assert_int_equal(1, isxdigit_proxy('f'));
    assert_int_equal(1, isxdigit_proxy('A'));
    assert_int_equal(1, isxdigit_proxy('F'));
    assert_int_equal(0, isxdigit_proxy('g'));
    assert_int_equal(0, isxdigit_proxy('z'));
    assert_int_equal(0, isxdigit_proxy('G'));
    assert_int_equal(0, isxdigit_proxy(' '));
    assert_int_equal(0, isxdigit_proxy('@'));
}

/* Define the test case function for tolower */
void test_wasm_tolower(void** state)
{
    assert_int_equal('u', tolower_wasm('U'));
    assert_int_equal('a', tolower_wasm('A'));
    assert_int_equal('z', tolower_wasm('Z'));
    assert_int_equal('b', tolower_wasm('b'));
    assert_int_equal('x', tolower_wasm('x'));
    assert_int_equal('1', tolower_wasm('1'));
    assert_int_equal('@', tolower_wasm('@'));
}

/* Define the test case function for toupper */
void test_wasm_toupper(void** state)
{
    assert_int_equal('U', toupper_wasm('u'));
    assert_int_equal('A', toupper_wasm('a'));
    assert_int_equal('Z', toupper_wasm('z'));
    assert_int_equal('B', toupper_wasm('b'));
    assert_int_equal('X', toupper_wasm('x'));
    assert_int_equal('1', toupper_wasm('1'));
    assert_int_equal('@', toupper_wasm('@'));
}

/* Define the test case function for isalnum */
void test_wasm_isalnum(void** state)
{
    assert_int_equal(1, isalnum_wasm('0'));
    assert_int_equal(1, isalnum_wasm('9'));
    assert_int_equal(1, isalnum_wasm('a'));
    assert_int_equal(1, isalnum_wasm('Z'));
    assert_int_equal(0, isalnum_wasm(' '));
    assert_int_equal(0, isalnum_wasm('@'));
    assert_int_equal(0, isalnum_wasm('['));
}

/* Define the test case function for nanosleep */
void test_wasm_nanosleep(void** state)
{
    struct timespec req = { 1, 500000000 };
    struct timespec rem = { 0, 0 };
    int result;

    struct timespec* req_ptr = (struct timespec*)malloc_wasm(sizeof(struct timespec));
    struct timespec* rem_ptr = (struct timespec*)malloc_wasm(sizeof(struct timespec));

    struct timespec* na_req_ptr = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)req_ptr);
    struct timespec* na_rem_ptr = wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)rem_ptr);

    na_req_ptr->tv_sec = req.tv_sec;
    na_req_ptr->tv_nsec = req.tv_nsec;

    na_rem_ptr->tv_sec = rem.tv_sec;
    na_rem_ptr->tv_nsec = rem.tv_nsec;

    result = nanosleep_wasm(req_ptr, rem_ptr);
    assert_int_equal(0, result);

    assert_int_equal(0, na_rem_ptr->tv_sec);
    assert_int_equal(0, na_rem_ptr->tv_nsec);
}

/* Define the test case function for memcpy */
void test_wasm_memcpy(void** state)
{
    const char* src;
    char* dest;
    char* dest_native;
    char* src_app;
    char* src_native;

    src = "Hello World";
    dest = (char*)malloc_wasm(strlen(src) + 1);
    src_app = (char*)malloc_wasm(strlen(src) + 1);
    src_native = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)src_app);
    strcpy(src_native, src);
    dest_native = (char*)memcpy_wasm(dest, src_app, strlen(src) + 1);
    assert_int_equal(dest_native[0], 'H');
    assert_int_equal(dest_native[10], 'd');
    assert_memory_equal(dest_native, src, strlen(src));

    free_wasm(dest);
    free_wasm(src_app);
}

/* Define the test case function for memmove */
void test_wasm_memmove(void** state)
{
    char* src;
    char* src_native;

    src = (char*)malloc_wasm(10 * sizeof(char));
    src_native = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)src);
    strcpy(src_native, "abcdefghij");

    memmove_wasm(src + 2, src, 5);

    assert_memory_equal(src_native, "ababcdehij", 10);

    free_wasm(src);
}

/* Define the test case function for memset */
void test_wasm_memset(void** state)
{
    char* dest;
    char* dest_native;
    dest = (char*)malloc_wasm(10 * sizeof(char));
    dest_native = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)dest);

    memset(dest_native, 'X', 10);
    memset_wasm(dest, 'A', 5);

    assert_memory_equal(dest_native, "AAAAAXXXXX", 10);
    free_wasm(dest);
}

/* Define the test case function for strchr */
void test_wasm_strchr(void** state)
{
    const char* s;
    char* s_app;
    char* s_native;
    char* result;

    s = "Hello World";
    s_app = (char*)malloc_wasm(strlen(s) + 1);
    s_native = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)s_app);
    strcpy(s_native, s);
    result = strchr_wasm(s_app, 'W');
    assert_int_equal(*result, 'W');

    free_wasm(s_app);
}

/* Define the test case function for strcmp */
void test_wasm_strcmp(void** state)
{
    const char* s1;
    const char* s2;
    char* s1_app;
    char* s2_app;
    char* s1_native;
    char* s2_native;
    int result;

    s1 = "Hello World";
    s2 = "Hello World";
    s1_app = (char*)malloc_wasm(strlen(s1) + 1);
    s2_app = (char*)malloc_wasm(strlen(s2) + 1);
    s1_native = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)s1_app);
    s2_native = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)s2_app);
    strcpy(s1_native, s1);
    strcpy(s2_native, s2);
    result = strcmp_wasm(s1_app, s2_app);
    assert_int_equal(result, 0);

    free_wasm(s1_app);
    free_wasm(s2_app);
}

/* Define the test case function for strncmp */
void test_wasm_strncmp(void** state)
{
    const char* s1;
    const char* s2;
    char* s1_app;
    char* s2_app;
    char* s1_native;
    char* s2_native;
    int result;

    s1 = "Hello World";
    s2 = "Hello Wasm";
    s1_app = (char*)malloc_wasm(strlen(s1) + 1);
    s2_app = (char*)malloc_wasm(strlen(s2) + 1);
    s1_native = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)s1_app);
    s2_native = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)s2_app);
    strcpy(s1_native, s1);
    strcpy(s2_native, s2);
    result = strncmp_wasm(s1_app, s2_app, 5);
    assert_int_equal(result, 0);
    result = strncmp_wasm(s1_app, s2_app, 8);
    assert_true(result > 0);

    free_wasm(s1_app);
    free_wasm(s2_app);
}

/* Define the test case function for strcpy */
void test_wasm_strcpy(void** state)
{
    const char* src;
    char* src_app;
    char* src_native;
    char* dest;
    char* dest_native;

    src = "Hello World";
    src_app = (char*)malloc_wasm(strlen(src) + 1);
    src_native = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)src_app);
    memcpy(src_native, src, strlen(src) + 1);
    dest = (char*)malloc_wasm(strlen(src) + 1);

    strcpy_wasm(dest, src_app);
    dest_native = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)dest);
    assert_string_equal(dest_native, src);

    free_wasm(src_app);
    free_wasm(dest);
}

/* Define the test case function for strncpy */
void test_wasm_strncpy(void** state)
{
    const char* src;
    char* src_app;
    char* src_native;
    char* dest;
    char* dest_native;

    src = "Hello_World";
    src_app = (char*)malloc_wasm(strlen(src) + 1);
    src_native = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)src_app);
    memcpy(src_native, src, strlen(src) + 1);
    dest = (char*)malloc_wasm(strlen(src) + 1);
    dest_native = (char*)strncpy_wasm(dest, src_app, 4);
    assert_memory_equal(dest_native, "Hell", 4);
    free_wasm(src_app);
    free_wasm(dest);
}

/* Define the test case function for strlen */
void test_wasm_strlen(void** state)
{
    const char* s;
    char* s_app;
    char* s_native;
    size_t result;

    s = "Hello World";
    s_app = (char*)malloc_wasm(strlen(s) + 1);
    s_native = (char*)wasm_runtime_addr_app_to_native(g_module_instance, (uintptr_t)s_app);
    strcpy(s_native, s);
    result = strlen_wasm(s_app);
    assert_int_equal(result, 11);

    free_wasm(s_app);
}

int main(int argc, char** argv)
{
    unsigned char* libc_wasm;
    int result = 0;

    /* Initialize the WAMR runtime environment */
    if (!wasm_runtime_init()) {
        return -1;
    }

    /* Register libc module into runtime */
    wamr_module_libc_register();

    libc_wasm = (unsigned char*)malloc(generated_libc_wasm_len);
    if (!libc_wasm) {
        goto cleanup_runtime;
    }

    memcpy(libc_wasm, generated_libc_wasm, generated_libc_wasm_len);

    /* Load the WASM module from the buffer */
    g_module = wasm_runtime_load(libc_wasm, generated_libc_wasm_len, NULL, 0);
    if (!g_module) {
        goto cleanup_runtime;
    }

    /* Instantiate the module */
    g_module_instance = wasm_runtime_instantiate(g_module, CONFIG_WASM_TEST_STACKSIZE, WASM_TEST_HEAPSIZE, NULL, 0);
    if (!g_module_instance) {
        goto cleanup_module;
    }

    /* Run the tests */
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_wasm_malloc),
        cmocka_unit_test(test_wasm_calloc),
        cmocka_unit_test(test_wasm_free),
        cmocka_unit_test(test_wasm_memcmp),
        cmocka_unit_test(test_wasm_strdup),
        cmocka_unit_test(test_wasm_atoi),
        cmocka_unit_test(test_wasm_strtol),
        cmocka_unit_test(test_wasm_strtoul),
        cmocka_unit_test(test_wasm_memchr),
        cmocka_unit_test(test_wasm_strncasecmp),
        cmocka_unit_test(test_wasm_strspn),
        cmocka_unit_test(test_wasm_strcspn),
        cmocka_unit_test(test_wasm_strstr),
        cmocka_unit_test(test_wasm_isupper),
        cmocka_unit_test(test_wasm_isalpha),
        cmocka_unit_test(test_wasm_isspace),
        cmocka_unit_test(test_wasm_isgraph),
        cmocka_unit_test(test_wasm_isprint),
        cmocka_unit_test(test_wasm_isdigit),
        cmocka_unit_test(test_wasm_isxdigit),
        cmocka_unit_test(test_wasm_tolower),
        cmocka_unit_test(test_wasm_toupper),
        cmocka_unit_test(test_wasm_isalnum),
        cmocka_unit_test(test_wasm_nanosleep),
        cmocka_unit_test(test_wasm_memcpy),
        cmocka_unit_test(test_wasm_memmove),
        cmocka_unit_test(test_wasm_memset),
        cmocka_unit_test(test_wasm_strchr),
        cmocka_unit_test(test_wasm_strcmp),
        cmocka_unit_test(test_wasm_strncmp),
        cmocka_unit_test(test_wasm_strcpy),
        cmocka_unit_test(test_wasm_strncpy),
        cmocka_unit_test(test_wasm_strlen),
    };
    result = cmocka_run_group_tests(tests, NULL, NULL);

    wasm_runtime_deinstantiate(g_module_instance);

cleanup_module:
    wasm_runtime_unload(g_module);

cleanup_runtime:
    if (libc_wasm) {
        free(libc_wasm);
    }

    wasm_runtime_destroy();

    return result;
}
