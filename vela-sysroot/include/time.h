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

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int64_t time_t;

struct timespec {
    time_t tv_sec; /* seconds */
    long tv_nsec; /* nanoseconds */
};

/**
 * @brief Pauses the execution of the calling thread for a specified time period.
 *
 * This function causes the calling thread to be suspended from execution until
 * either the time interval specified by the `req` argument has elapsed, or a
 * signal is delivered to the thread and its action is to invoke a signal-catching
 * function.
 *
 * @param req Pointer to a `timespec` structure specifying the amount of time to
 *            sleep in seconds and nanoseconds.
 * @param rem If not NULL, and the function returns due to a signal interruption,
 *            this pointer will be used to store the remaining time that the
 *            thread should sleep.
 * @return 0 on success, or -1 on failure with `errno` set to indicate the error.
 */

int nanosleep(const struct timespec* req, struct timespec* rem);

#ifdef __cplusplus
}
#endif
