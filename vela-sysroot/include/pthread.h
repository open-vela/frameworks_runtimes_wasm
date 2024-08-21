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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Data type define of pthread, mutex, cond and key */
typedef unsigned int pthread_t;
typedef unsigned int pthread_mutex_t;
typedef unsigned int pthread_cond_t;
typedef unsigned int pthread_key_t;

/* Thread APIs */
int pthread_create(pthread_t* thread, const void* attr,
    void* (*start_routine)(void*), void* arg);

int pthread_join(pthread_t thread, void** retval);

int pthread_detach(pthread_t thread);

int pthread_cancel(pthread_t thread);

pthread_t
pthread_self(void);

void pthread_exit(void* retval);

/* Mutex APIs */
int pthread_mutex_init(pthread_mutex_t* mutex, const void* attr);

int pthread_mutex_lock(pthread_mutex_t* mutex);

int pthread_mutex_unlock(pthread_mutex_t* mutex);

int pthread_mutex_destroy(pthread_mutex_t* mutex);

/* Cond APIs */
int pthread_cond_init(pthread_cond_t* cond, const void* attr);

int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex);

int pthread_cond_timedwait(pthread_cond_t* cond, pthread_mutex_t* mutex,
    uint64_t useconds);

int pthread_cond_signal(pthread_cond_t* cond);

int pthread_cond_broadcast(pthread_cond_t* cond);

int pthread_cond_destroy(pthread_cond_t* cond);

/* Pthread key APIs */
int pthread_key_create(pthread_key_t* key, void (*destructor)(void*));

int pthread_setspecific(pthread_key_t key, const void* value);

void* pthread_getspecific(pthread_key_t key);

int pthread_key_delete(pthread_key_t key);

#ifdef __cplusplus
}
#endif
