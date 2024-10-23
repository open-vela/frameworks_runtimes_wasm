# Introduction

[English|[简体中文](./README_zh-cn.md)]

The Vela Wasm Framework provides extra modules and SDKs to build WebAssembly applications.

# Project Structure

```tree
├── chre
├── libc
├── memory
├── vela-sysroot
├── vendor
├── README.md
└── README_zh-cn.md
```

* `chre`: This module provides the function from Context Hub Runtime Environment to Wasm runtime
* `libc`: The C standard library for Wasm
* `memory`: Memory aceess management between wasm and native code
* `vela-sysroot`: A collection of extra header files to build WebAssembly applications
* `vendor`: The Wasm application SDKs for 3rd party chip vendor

# Usage

## Function module for Wasm runtime

Only WAMR (WebAssembly Micro Runtime) is supported now. To use the function module, you need to enable WAMR firstly in menuconfig, and then enable `WASM_LIBC` (libc) or `WASM_MEMORY_OPERATIONS` (memory) in the corresponding submenu.

And then the function from libc or memory will be available in `iwasm` command line tool, and can be used directly in Wasm applications.

## Build Wasm application by SDKs

The `vendor` directory provides a collection of SDKs for 3rd party chip vendors, only bouffalo lab's BL616 is supported now. To build an applicaiton using the SDK, you can refer to the following documntation: [BL616 SDK](./vendor/bouffalo/README.md)
