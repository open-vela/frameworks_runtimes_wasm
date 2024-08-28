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

# Generated defined symbols file for the project,
# it's combines the symbols from VELA_SYSROOT and BLFB_SDK
file(READ ${VELA_SYSROOT}/share/defined-symbols.txt VELA_SYSROOT_SYMBOLS)
file(READ ${CMAKE_CURRENT_LIST_DIR}/cmake/defined-symbols.txt BLFB_SDK_SYMBOLS)
# Write the combined symbols to a file
file(WRITE ${CMAKE_BINARY_DIR}/defined-symbols.txt "${VELA_SYSROOT_SYMBOLS}${BLFB_SDK_SYMBOLS}")

# BEGIN: Don't use stdlib (wasi-libc) to reduce RAM usage
# You can comment this out if you want to use stdlib (wasi-libc)

set(CMAKE_SYSROOT ${VELA_SYSROOT})
add_link_options(-nostdlib)
add_link_options(-Wl,--no-entry)
add_link_options(-Wl,--export=main)
add_link_options(-Wl,--export=__heap_base)
add_link_options(-Wl,--export=__data_end)
add_link_options(-Wl,--allow-undefined-file=${CMAKE_BINARY_DIR}/defined-symbols.txt)
# TODO: Change this value according by cmake configuration
add_link_options(-Wl,--initial-memory=65536)
add_link_options(-z stack-size=4096)

# END: Don't use stdlib (wasi-libc) to reduce RAM usage

# Make include directories available to the build as higher priority than bouffalo sdk
include_directories(include)

# Add bouffalo sdk include directories
include_directories(${BOUFFALO_SDK_PATH}/drivers/lhal/include)
include_directories(${BOUFFALO_SDK_PATH}/drivers/lhal/include/arch)
include_directories(${BOUFFALO_SDK_PATH}/drivers/lhal/src/flash)
include_directories(${BOUFFALO_SDK_PATH}/drivers/soc/bl616/std/include)
include_directories(${BOUFFALO_SDK_PATH}/drivers/soc/bl616/std/include/hardware)

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

# Essential global flags to build wasm application

# -fdata-sections and -ffunction-sections are used to reduce the size of the binary file
add_compile_options(-fdata-sections)
add_compile_options(-ffunction-sections)

# -Wl,--gc-sections is used to remove unused sections from the binary file
add_link_options(-Wl,--gc-sections)

add_library(driver ${DRIVER_SRC})

# Utility functions for add wasm application to the build system
function(add_wasm_application name SRC)
    """
    Adds a WebAssembly application to the build system.

    This function creates an executable target for the given source files,
    sets the target properties to produce a .wasm file, links the target with
    the 'driver' library, and adds a custom post-build command to compile the
    WebAssembly application to an Ahead-Of-Time (AOT) module using wamrc.

    Args:
        name (str): The name of the WebAssembly application.
        SRC (list): List of source files for the application.
    """
    add_executable(${name} ${SRC})
    set_target_properties(${name} PROPERTIES SUFFIX .wasm)
    target_link_libraries(${name} driver)

    # Post process for wasm application, compile it to AOT module by wamrc
    add_custom_command(TARGET ${name} POST_BUILD
        COMMAND ${WASI_SDK_PATH}/wamrc --target=riscv32 --target-abi=ilp32f --cpu=generic-rv32 --cpu-features=+m,+a,+f,+c --xip --enable-builtin-intrinsics=i64.common,f64.common,f32.const,f64.const,f64xi32,f64xi64,f64_promote_f32,f32_demote_f64 --runtime-bounds-checks -o $<TARGET_FILE:${name}>.aot $<TARGET_FILE:${name}>)
endfunction()

# Add sub-directories to the build system by ${APP} from the command line
add_subdirectory(${APP})
