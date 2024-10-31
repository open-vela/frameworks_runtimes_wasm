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

/* This determines the importance of the message. The levels are, in order
 * of decreasing importance:
 */

#define LOG_EMERG 0 /* System is unusable */
#define LOG_ALERT 1 /* Action must be taken immediately */
#define LOG_CRIT 2 /* Critical conditions */
#define LOG_ERR 3 /* Error conditions */
#define LOG_WARNING 4 /* Warning conditions */
#define LOG_NOTICE 5 /* Normal, but significant, condition */
#define LOG_INFO 6 /* Informational message */
#define LOG_DEBUG 7 /* Debug-level message */

/* Macro to retarget syslog to printf */
#define syslog(priority, fmt, ...) printf(fmt, ##__VA_ARGS__)