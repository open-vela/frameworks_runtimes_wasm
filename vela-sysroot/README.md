# Overview

This folder contains the sysroot for the Vela Wasm Application Framework.

Generally, a sysroot for WebAssembly is a minimum set of files and directories that are required to run an application or library compiled with WASI-SDK. This includes standard libraries and other necessary components.

For standard compilactions (e.g., C/C++), the sysroot typically contains:
- Standard C library headers (like `<stdio.h>`, `<stdlib.h>` etc.)
- List of defined symbols for linkers to know what functions are available at runtime
- Third party libraries headers and static libraries (if needed)

For now, currently this sysroot contains:
- libc
  - ctypes.h
  - errno.h
  - inttypes.h
  - limits.h
  - pthread.h
  - semaphore.h
  - stdarg.h
  - stdbool.h
  - stdlib.h
  - string.h
- misc
  - defined-symbols.txt
