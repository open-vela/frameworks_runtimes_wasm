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

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int memcmp_proxy(const void* s1, const void* s2, size_t n)
{
    return memcmp(s1, s2, n);
}

void* malloc_proxy(size_t size)
{
    return malloc(size);
}

void* calloc_proxy(size_t nmemb, size_t size)
{
    return calloc(nmemb, size);
}

char* strdup_proxy(const char* s)
{
    return strdup(s);
}

void free_proxy(void* ptr)
{
    free(ptr);
}

int atoi_proxy(const char* nptr)
{
    return atoi(nptr);
}

void exit_proxy(int status)
{
    exit(status);
}

long int strtol_proxy(const char* nptr, char** endptr, int base)
{
    return strtol(nptr, endptr, base);
}

unsigned long int strtoul_proxy(const char* nptr, char** endptr, int base)
{
    return strtoul(nptr, endptr, base);
}

void* memchr_proxy(const void* s, int c, size_t n)
{
    return memchr(s, c, n);
}

int strncasecmp_proxy(const char* s1, const char* s2, size_t n)
{
    return strncasecmp(s1, s2, n);
}

size_t strspn_proxy(const char* s, const char* accept)
{
    return strspn(s, accept);
}

size_t strcspn_proxy(const char* s, const char* reject)
{
    return strcspn(s, reject);
}

char* strstr_proxy(const char* haystack, const char* needle)
{
    return strstr(haystack, needle);
}

int isupper_proxy(int c)
{
    return isupper(c);
}

int isalpha_proxy(int c)
{
    return isalpha(c);
}

int isspace_proxy(int c)
{
    return isspace(c);
}

int isgraph_proxy(int c)
{
    return isgraph(c);
}

int isprint_proxy(int c)
{
    return isprint(c);
}

int isdigit_proxy(int c)
{
    return isdigit(c);
}

int isxdigit_proxy(int c)
{
    return isxdigit(c);
}

int tolower_proxy(int c)
{
    return tolower(c);
}

int toupper_proxy(int c)
{
    return toupper(c);
}

int isalnum_proxy(int c)
{
    return isalnum(c);
}

int nanosleep_proxy(const struct timespec* req, struct timespec* rem)
{
    return nanosleep(req, rem);
}

void* memcpy_proxy(void* dest, const void* src, size_t n)
{
    return memcpy(dest, src, n);
}

void* memmove_proxy(void* dest, const void* src, size_t count)
{
    return memmove(dest, src, count);
}

void* memset_proxy(void* s, int c, size_t n)
{
    return memset(s, c, n);
}

char* strchr_proxy(const char* s, int c)
{
    return strchr(s, c);
}

int strcmp_proxy(const char* s1, const char* s2)
{
    return strcmp(s1, s2);
}

int strncmp_proxy(const char* s1, const char* s2, size_t n)
{
    return strncmp(s1, s2, n);
}

char* strcpy_proxy(char* dest, const char* src)
{
    return strcpy(dest, src);
}

char* strncpy_proxy(char* dest, const char* src, size_t n)
{
    return strncpy(dest, src, n);
}

size_t strlen_proxy(const char* s)
{
    return strlen(s);
}
