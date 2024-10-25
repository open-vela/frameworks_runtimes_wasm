# Introduction

This directory contains the test cases for the libc API provided by Wasm runtimes.

# Requirements

1. **Test Cases**: The test cases should cover all the built-in libc functions of WAMR and addons implemented by ourselves.
2. **Framework**: We should use CMocka as the test framework.
3. **Execution**: Should have a shell command in nsh to run all the test cases.

# Design

For this task, we should call the Wasm runtime API to call libc functions and compare the result with expected value. But we can not directly call the internal function of Wasm runtime because it is not exposed directly.
So we need a proxy module that could be able to expose the internal APIs of Wasm runtime to us.

# Implementation

In order to test the API from https://github.com/bytecodealliance/wasm-micro-runtime/blob/main/core/iwasm/libraries/libc-builtin/libc_builtin_wrapper.c, we need to create a proxy module that contains some proxy functions like this:
```c
void *malloc_proxy(size_t size) {
    return malloc(size);
}
```

Then we can call the malloc function in Wasm runtime through `malloc_proxy` and compare the result with expected value in native code.

# Proxy Module

The source code of proxy module is located at: proxy/libc.c

You can use this command to build the proxy module:
```sh
$(WASI_SDK_PATH)/bin/clang --target=wasm32 -O3 -flto --sysroot=PWD/frameworks/runtimes/wasm/vela-sysroot -nostdlib -z stack-size=4096 -Wl,--initial-memory=65536 -o proxy/libc.wasm proxy/libc.c -Wl,--no-entry -Wl,--export-all
```
