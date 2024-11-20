# # Wasm Module

[English | [简体中文](./README_zh-cn.md)]

The openvela Wasm framework provides additional modules and SDKs to build `WebAssembly` applications.

## Project Structure

```tree
├── chre
├── libc
├── memory
├── vela-sysroot
├── vendor
├── README.md
└── README_zh-cn.md
```

* `chre`: This module provides functionality from the Context Hub Runtime Environment to the Wasm runtime.
* `libc`: C standard library for Wasm.
* `memory`: Memory access management between Wasm and native code.
* `vela-sysroot`: A collection of additional header files for building WebAssembly applications.
* `vendor`: Wasm application SDKs for third-party chip vendors.


## How to use

### Wasm runtime function module

Currently only WAMR (WebAssembly Micro Runtime) is supported. If you want to use this function module:
1. Enable `WAMR` in `menuconfig`.

2. Enable `WASM_LIBC` (libc) or `WASM_MEMORY_OPERATIONS` (memory) in the corresponding submenu.

3. The functions of libc or memory are available in the `iwasm` command line tool and can be used directly in Wasm applications.

### Build Wasm applications using SDK

The `vendor` directory provides a collection of SDKs for third-party chip vendors. Currently, only BL616 from `bouffalo lab` is supported. To build applications using the SDK, you can refer to the following document: [BL616 SDK](./vendor/bouffalo/README.md)
