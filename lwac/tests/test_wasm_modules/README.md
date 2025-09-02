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

The modules are compiled using wasi-sdk with baremetal wasm32 target:

```bash
# Compile all modules
make all

# Compile specific module
make simple_test.wasm
make entry_point_test.wasm

# Clean compiled files
make clean

# Check if wasi-sdk is available
make check
```

## Compilation Details

**Target:** `wasm32` (baremetal WebAssembly)
**Flags:**
- `--target=wasm32` - Target baremetal WebAssembly
- `-nostdlib` - Don't link standard C library
- `-Wl,--no-entry` - Don't require _start entry point
- `-Wl,--export-all` - Export all functions for LWAC to call
- `-O2` - Optimize for size and performance

## Usage in Tests

These modules are used by the LWAC test framework to verify:
- Basic module execution
- Custom entry point functionality
- Command line argument handling
- Stack and heap usage
- Function parameter passing
- Error handling scenarios

## Prerequisites

- wasi-sdk installed and `clang` available in PATH
- LLVM WebAssembly backend support

## Module Design

The modules are designed to be minimal and self-contained:
- No external dependencies
- No standard library usage
- Export all functions for testing
- Return predictable values for verification
- Use stack space for memory testing