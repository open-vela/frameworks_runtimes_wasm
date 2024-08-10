#
# Copyright (C) 2024 Xiaomi Corperation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.20)

project(applet)

include(cmake/wasi-sdk.cmake)

# Set the path to the bouffalo sdk if it is existing
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/bouffalo_sdk)
    set(BOUFFALO_SDK_PATH ${CMAKE_CURRENT_SOURCE_DIR}/bouffalo_sdk)
else()
    message(FATAL "Bouffalo SDK not found, please clone it from https://github.com/bouffalolab/bouffalo_sdk.git -b V2.0.0")
endif()

# Check if the VELA_SYSROOT is set, if not, use the default path
if(NOT DEFINED VELA_SYSROOT)
    message(STATUS "VELA_SYSROOT is not defined, using default path")
    set(VELA_SYSROOT ${CMAKE_CURRENT_LIST_DIR}/../../vela-sysroot)
    message(STATUS "VELA_SYSROOT set to ${VELA_SYSROOT}")
endif()

# Check if the VELA_SYSROOT is really existing
if(NOT EXISTS ${VELA_SYSROOT})
    message(FATAL "VELA_SYSROOT not found, please set it correctly")
endif()

# BEGIN: Don't use stdlib (wasi-libc) to reduce RAM usage
# You can comment this out if you want to use stdlib (wasi-libc)

set(CMAKE_SYSROOT ${VELA_SYSROOT})
add_link_options(-nostdlib)
add_link_options(-Wl,--no-entry)
add_link_options(-Wl,--export=main)
add_link_options(-Wl,--export=__heap_base)
add_link_options(-Wl,--export=__data_end)
add_link_options(-Wl,--allow-undefined-file=${VELA_SYSROOT}/share/defined-symbols.txt)
# TODO: Change this value according by cmake configuration
add_link_options(-Wl,--initial-memory=65536)
add_link_options(-z stack-size=4096)

# END: Don't use stdlib (wasi-libc) to reduce RAM usage

# Add bouffalo sdk include directories
include_directories(${BOUFFALO_SDK_PATH}/drivers/lhal/include)
include_directories(${BOUFFALO_SDK_PATH}/drivers/lhal/include/arch)
include_directories(${BOUFFALO_SDK_PATH}/drivers/lhal/src/flash)
include_directories(${BOUFFALO_SDK_PATH}/drivers/soc/bl616/std/include)
include_directories(${BOUFFALO_SDK_PATH}/drivers/soc/bl616/std/include/hardware)
include_directories(include)

# Add definitions needed by bouffalo sdk
add_compile_definitions(ARCH_RISCV)
add_compile_definitions(__riscv)
add_compile_definitions(__riscv_xlen=32)
add_compile_definitions(BL616)

list(APPEND DRIVER_SRC ${BOUFFALO_SDK_PATH}/drivers/lhal/config/bl616/device_table.c)
list(APPEND DRIVER_SRC ${BOUFFALO_SDK_PATH}/drivers/lhal/src/bflb_uart.c)
list(APPEND DRIVER_SRC ${BOUFFALO_SDK_PATH}/drivers/lhal/src/bflb_adc.c)
list(APPEND DRIVER_SRC ${BOUFFALO_SDK_PATH}/drivers/lhal/src/bflb_gpio.c)
list(APPEND DRIVER_SRC ${BOUFFALO_SDK_PATH}/drivers/lhal/src/bflb_i2c.c)
list(APPEND DRIVER_SRC ${BOUFFALO_SDK_PATH}/drivers/lhal/src/bflb_i2s.c)
list(APPEND DRIVER_SRC ${BOUFFALO_SDK_PATH}/drivers/lhal/src/bflb_ir.c)
list(APPEND DRIVER_SRC ${BOUFFALO_SDK_PATH}/drivers/lhal/src/bflb_spi.c)

list(APPEND SOC_SRC ${BOUFFALO_SDK_PATH}/drivers/soc/bl616/std/port/bl616_clock.c)

# Essential global flags to build wasm application

# -fdata-sections and -ffunction-sections are used to reduce the size of the binary file
add_compile_options(-fdata-sections)
add_compile_options(-ffunction-sections)

# -Wl,--gc-sections is used to remove unused sections from the binary file
add_link_options(-Wl,-gc-sections)

add_library(driver ${DRIVER_SRC})
add_library(soc ${SOC_SRC})

# Utility functions for add wasm application to the build system
function(add_wasm_application name SRC)
    add_executable(${name} ${SRC})
    set_target_properties(${name} PROPERTIES SUFFIX .wasm)
    target_link_libraries(${name} driver soc)
endfunction()

# Add sub-directories to the build system by ${APP} from the command line
add_subdirectory(${APP})
