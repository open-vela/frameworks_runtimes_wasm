/*
 * Copyright (C) 2024 Xiaomi Corperation
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

#include <string.h>
#include <time.h>

/**
 * @brief Delays the execution for a specified number of microseconds.
 *
 * This function is required by BFLB SDK for timing operations and
 * is implemented using nanosleep system call.
 *
 * @param delay The number of microseconds to delay the execution.
 */

void bflb_mtimer_delay_us(uint32_t delay)
{
    struct timespec ts;

    /* Convert delay to nanoseconds and back to seconds/nanoseconds */
    uint64_t nsec = (uint64_t)delay * 1000ULL;
    ts.tv_sec = nsec / 1000000000ULL;
    ts.tv_nsec = nsec % 1000000000ULL;

    /* Sleep for the specified time */
    nanosleep(&ts, NULL);
}

/**
 * @brief Delays the execution for a specified number of milliseconds.
 *
 * This function is required by BFLB SDK for timing operations and
 * is implemented using nanosleep system call.
 *
 * @param delay The number of milliseconds to delay the execution.
 */

void bflb_mtimer_delay_ms(uint32_t delay)
{
    /* Call the delay_us function with the delay in microseconds */
    bflb_mtimer_delay_us(delay * 1000);
}
