# LWAC Test WebAssembly Modules

This directory contains WebAssembly test modules for the LWAC framework testing suite.

## Modules

### simple_test.wasm
Basic WebAssembly module compiled from `simple_test.c` that provides:
- `main()` - Standard main function returning 42
- `_main()` - Alternative main entry point
- `__main_argc_argv(argc, argv)` - Entry point with command line arguments
- `test_function()` - Custom function for testing entry point specification
- `add_numbers(a, b)` - Function for testing parameter passing

### entry_point_test.wasm
Entry point test module compiled from `entry_point_test.c` that provides:
- `custom_entry()` - Custom entry point returning 200
- `alt_entry()` - Alternative custom entry point returning 300
- `param_entry(param1, param2)` - Entry point with parameters
- `main()` - Standard main returning 150 (different from simple_test)
- `stack_test()` - Function that uses stack space for testing

## Compilation

The modules are compiled using the NuttX WASM build system with CMake and wasi-sdk:

```bash
# Build from the project root directory
make -C nuttx

# Or build specifically for WASM modules
cd nuttx && make cmake_build
```

The build system will automatically:
- Compile all modules using `wasm_add_application`
- Apply WASI SDK toolchain and flags
- Generate optimized `.wasm` files
- Install modules to `${TOPBINDIR}/wasm/`

## Compilation Details

**Build System:** CMake with `wasm_add_application`
**Target:** `wasm32-wasi` (WebAssembly with WASI support)
**Stack Size:** 2048 bytes
**Initial Memory:** 65536 bytes (1 page)
**Optimization:** Applied automatically via `wasm-opt -Oz --enable-bulk-memory`

## Usage in Tests

These modules are used by the LWAC test framework to verify:
- Basic module execution
- Custom entry point functionality
- Command line argument handling
- Stack and heap usage
- Function parameter passing
- Error handling scenarios

## Prerequisites

- WASI SDK installed and `WASI_SDK_PATH` environment variable set
- WASM toolchain installed and `WASM_TOOLCHAIN_PATH` environment variable set
- NuttX build system configured with WASM support enabled

## Module Design

The modules are designed to be minimal and self-contained:
- WASI-compatible (no direct NuttX API calls)
- Minimal external dependencies (only `usleep` in simple_test)
- Export all functions for testing using `__attribute__((export_name))`
- Return predictable values for verification
- Use stack space for memory testing
- Compatible with both interpreter and AOT compilation modes