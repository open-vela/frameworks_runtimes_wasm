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

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int atoi(const char* s);
void exit(int status);
long strtol(const char* nptr, char** endptr, register int base);
unsigned long strtoul(const char* nptr, char** endptr, register int base);
void* malloc(size_t size);
void* calloc(size_t n, size_t size);
void free(void* ptr);

#ifdef __cplusplus
}
#endif
