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

#include <sys/boardctl.h>

// Workmode enumeration for LWAC operations
typedef enum {
    LWAC_WORKMODE_COMMON_MODULE,
    LWAC_WORKMODE_INC_MODULE,
} lwac_workmode_e;

// Magic number for INC workmode detection
#define LWAC_WORKMODE_MAGIC_INC 0xBF1BB1FB

// Base command ID for LWAC board control operations

#define BOARDCTL_LWAC (BOARDIOC_USER + 0x20)

// Command ID for mounting OTA firmware
#define BOARDIOC_LWAC_MOUNT_OTA_FW (BOARDCTL_LWAC + 0)

// Command IDs for LWAC system information and control
#define BOARDIOC_LWAC_GET_WORKMODE (BOARDCTL_LWAC + 1)
#define BOARDIOC_LWAC_GET_MODEL (BOARDCTL_LWAC + 2)
#define BOARDIOC_LWAC_GET_VERSION (BOARDCTL_LWAC + 3)
#define BOARDIOC_LWAC_GET_COMPILE_TIME (BOARDCTL_LWAC + 4)

// Command IDs for LWAC system switching operations
#define BOARDIOC_LWAC_SWITCH_FLASH_PARTION (BOARDCTL_LWAC + 5)
#define BOARDIOC_LWAC_SWITCH_MINSYS (BOARDCTL_LWAC + 6)
#define BOARDIOC_LWAC_GET_CURRENT_FLASH (BOARDCTL_LWAC + 7)

// Command IDs for LWAC flash device operations
#define BOARDIOC_LWAC_OPEN_CURRENT_FLASH_DEVICE (BOARDCTL_LWAC + 8)
#define BOARDIOC_LWAC_OPEN_UPDATE_FLASH_DEVICE (BOARDCTL_LWAC + 9)

// Command IDs for LWAC mount or unmount OTA firmware operations
#define BOARDIOC_LWAC_MOUNT_INC_OTA (BOARDIOC_USER + 10)
#define BOARDIOC_LWAC_UMOUNT_INC_OTA (BOARDIOC_USER + 11)

// OTA flash partition identifiers
#define LWAC_OTA_PARTITION_A 0
#define LWAC_OTA_PARTITION_B 1
#define LWAC_OTA_PARTITION_MINSYS 2

/**
 * @brief Structure for BOARDIOC_LWAC_GET_MODEL parameter
 *
 * This structure is used to pass model information request parameters
 * to the boardctl system call.
 */
struct boardctl_lwac_get_model_s {
    char* model; /**< Pointer to buffer to store model information */
    int len; /**< Length of the buffer */
};

/**
 * @brief Structure for BOARDIOC_LWAC_GET_VERSION parameter
 *
 * This structure is used to pass version information request parameters
 * to the boardctl system call.
 */
struct boardctl_lwac_get_version_s {
    char* version; /**< Pointer to buffer to store version information */
    int len; /**< Length of the buffer */
};

/**
 * @brief Structure for BOARDIOC_LWAC_GET_COMPILE_TIME parameter
 *
 * This structure is used to pass compile time information request parameters
 * to the boardctl system call.
 */
struct boardctl_lwac_get_compile_time_s {
    char* time; /**< Pointer to buffer to store compile time information */
    int len; /**< Length of the buffer */
};

/**
 * @brief Structure for BOARDIOC_LWAC_OPEN_UPDATE_FLASH_DEVICE parameter
 *
 * This structure is used to pass flash device open parameters
 * to the boardctl system call for update operations.
 */
struct boardctl_lwac_open_update_flash_s {
    int oflags; /**< Open flags for the flash device */
    unsigned int mode; /**< Access mode for the flash device */
};

/**
 * @brief Structure for BOARDIOC_LWAC_OPEN_CURRENT_FLASH_DEVICE parameter
 *
 * This structure is used to pass flash device open parameters
 * to the boardctl system call for current flash operations.
 */
struct boardctl_lwac_open_current_flash_s {
    int oflags; /**< Open flags for the flash device */
    unsigned int mode; /**< Access mode for the flash device */
};

// Static inline implementations of applet functions based on boardctl
// for backward compatibility with the legacy code

/**
 * @brief Get the current workmode of the LWAC system.
 *
 * @return The current workmode as defined in lwac_workmode_e.
 */
static inline lwac_workmode_e applet_get_workmode(void)
{
    lwac_workmode_e workmode;
    boardctl(BOARDIOC_LWAC_GET_WORKMODE, (uintptr_t)&workmode);
    return workmode;
}

/**
 * @brief Get the model information of the LWAC system.
 *
 * @param model Pointer to the buffer to store the model information.
 * @param len Length of the buffer.
 * @return 0 on success, negative value on failure.
 */
static inline int applet_get_model(char* model, int len)
{
    struct boardctl_lwac_get_model_s get_model;
    get_model.model = model;
    get_model.len = len;
    return boardctl(BOARDIOC_LWAC_GET_MODEL, (uintptr_t)&get_model);
}

/**
 * @brief Get the version information of the LWAC system.
 *
 * @param version Pointer to the buffer to store the version information.
 * @param len Length of the buffer.
 * @return 0 on success, negative value on failure.
 */
static inline int applet_get_version(char* version, int len)
{
    struct boardctl_lwac_get_version_s get_version;
    get_version.version = version;
    get_version.len = len;
    return boardctl(BOARDIOC_LWAC_GET_VERSION, (uintptr_t)&get_version);
}

/**
 * @brief Get the compile time information of the LWAC system.
 *
 * @param time Pointer to the buffer to store the compile time information.
 * @param len Length of the buffer.
 * @return 0 on success, negative value on failure.
 */
static inline int applet_get_package_time(char* time, int len)
{
    struct boardctl_lwac_get_compile_time_s get_time;
    get_time.time = time;
    get_time.len = len;
    return boardctl(BOARDIOC_LWAC_GET_COMPILE_TIME, (uintptr_t)&get_time);
}

/**
 * @brief Switch to the INC flash partition.
 *
 * @return 0 on success, negative value on failure.
 */
static inline int applet_switch_inc_flash(void)
{
    return boardctl(BOARDIOC_LWAC_SWITCH_FLASH_PARTION, 0);
}

/**
 * @brief Switch to the INC minsystem.
 *
 * @return 0 on success, negative value on failure.
 */
static inline int applet_switch_inc_minsystem(void)
{
    return boardctl(BOARDIOC_LWAC_SWITCH_MINSYS, 0);
}

/**
 * @brief Open the INC update flash device.
 *
 * @param oflags Open flags.
 * @param mode Access mode.
 * @return File descriptor on success, negative value on failure.
 */
static inline int applet_open_inc_update_flash(int oflags, unsigned int mode)
{
    struct boardctl_lwac_open_update_flash_s open_flash;
    open_flash.oflags = oflags;
    open_flash.mode = mode;
    return boardctl(BOARDIOC_LWAC_OPEN_UPDATE_FLASH_DEVICE, (uintptr_t)&open_flash);
}

/**
 * @brief Open the INC current flash device.
 *
 * @param oflags Open flags.
 * @param mode Access mode.
 * @return File descriptor on success, negative value on failure.
 */
static inline int applet_open_inc_current_flash(int oflags, unsigned int mode)
{
    struct boardctl_lwac_open_current_flash_s open_flash;
    open_flash.oflags = oflags;
    open_flash.mode = mode;
    return boardctl(BOARDIOC_LWAC_OPEN_CURRENT_FLASH_DEVICE, (uintptr_t)&open_flash);
}

/**
 * @brief Get the current running flash status.
 *
 * @return Flash status on success, negative value on failure.
 */
static inline int applet_get_inc_running_flash(void)
{
    int flash_status;
    boardctl(BOARDIOC_LWAC_GET_CURRENT_FLASH, (uintptr_t)&flash_status);
    return flash_status;
}

/**
 * @brief mount inc_ota device.
 *
 * @return Flash status on success, negative value on failure.
 */
static inline int applet_mount_inc_ota(void)
{
    int flash_status;
    boardctl(BOARDIOC_LWAC_MOUNT_INC_OTA, (uintptr_t)&flash_status);
    return flash_status;
}

/**
 * @brief umount inc_ota device.
 *
 * @return Flash status on success, negative value on failure.
 */
static inline int applet_umount_inc_ota(void)
{
    int flash_status;
    boardctl(BOARDIOC_LWAC_UMOUNT_INC_OTA, (uintptr_t)&flash_status);
    return flash_status;
}

#ifdef __cplusplus
}
#endif