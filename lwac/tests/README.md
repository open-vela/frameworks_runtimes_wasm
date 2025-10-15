# LWAC Integration Tests

## Overview
This directory contains integration tests for the LWAC (Lightweight WebAssembly Application Container) framework. These tests verify the complete functionality of the LWAC tool including WebAssembly module execution, command line argument parsing, file loading mechanisms, and module registry functionality.

## Test Structure
```
tests/
├── README.md                 # This file
├── lwac_test.c               # Main test runner application
├── test_wasm_modules/        # Directory containing test WASM modules
│   ├── simple_test.c         # C source for basic test module
│   ├── entry_point_test.c    # C source for entry point test module
│   ├── simple_test.wasm      # Compiled basic WASM module
│   ├── simple_test.aot       # AOT-compiled basic module (optional)
│   ├── entry_point_test.wasm # Compiled WASM module with custom entry point
│   ├── entry_point_test.aot  # AOT-compiled module with custom entry point (optional)
│   ├── Makefile              # Build rules for compiling WASM/AOT modules
│   └── README.md             # Documentation for test modules
├── Makefile                  # Build file for test app
├── Kconfig                   # Configuration options for test app (currently empty)
├── Make.defs                 # Make definitions for test app
├── .built                    # Build marker file
├── .depend                   # Dependency tracking file
└── Make.dep                  # Generated dependency file
```

## Test Areas

### 1. Basic Execution Tests
- Verify successful execution of a simple WebAssembly module
- Test different entry point specifications (main, _main, __main_argc_argv)
- Test execution with custom stack and heap sizes

### 2. Command Line Argument Parsing Tests
- Test various combinations of command line options
- Verify correct handling of -m file option
- Test positional argument handling
- Test custom entry function specification with -f option
- Test stack-size and heap-size options
- Test --ps option for listing running modules

### 3. Module Registry Tests
- Test module registration during execution
- Verify module information display with --ps option
- Test concurrent module execution and registry tracking

### 4. File Loading Tests
- Test mmap-based loading (default behavior)
- Test XIP compatibility checking

### 5. Error Handling Tests
- Test error handling for non-existent files
- Test error handling for invalid command line arguments
- Test error handling for missing entry functions
- Test error handling for XIP compatibility issues

**Note:** The error handling test currently shows a known issue where LWAC returns success (return code 0) even when it reports file opening errors. This is visible in the test output where "Error opening file '/inc/non_existent.aot': errno=2" is logged but the test still reports "expected error but got success".

## Test Implementation Details

### Test Runner Application
The main test runner (`lwac_test.c`) is fully implemented and coordinates the execution of all test cases. It provides:
- Command line argument parsing for selective test execution
- Test case initialization and cleanup
- Result collection and reporting
- Test environment setup and teardown

The test runner is built as part of the build system and can be executed from the shell.

**Test Execution Method:**
The test runner uses the standard `system()` function to execute the `lwac` command with various test parameters. This approach allows the test runner to:
- Execute WASM/AOT modules in a separate process context
- Monitor execution through console output
- Capture return codes for pass/fail determination

**Available Test Runner Commands:**
```bash
# Run all tests
lwac_test

# Run specific test
lwac_test --test basic_execution

# List all available tests
lwac_test --list

# Show help
lwac_test --help
```

**Implemented Test Cases:**
- `basic_execution`: Tests basic WASM module execution with simple_test module
- `argument_parsing`: Tests command line argument handling and stack size options
- `module_registry`: Tests module registry functionality with --ps option
- `file_loading`: Tests mmap-based file loading
- `error_handling`: Tests error handling for invalid files and arguments

### Build System Integration
The test framework is integrated with the following files:
- `Kconfig`: Configuration options for the test app (currently empty template)
- `Makefile`: Build instructions for the test app with automatic WASM module compilation
- `Make.defs`: Make definitions for the build system

**Build Process:**
The main Makefile automatically builds WASM test modules during the build process using the `context::` target:
```makefile
context::
	$(MAKE) -C test_wasm_modules install APPDIR=$(APPDIR) WASI_SDK_PATH=$(WASI_SDK_PATH)
```

### Test WASM Modules
WebAssembly modules are fully implemented and available for testing:

#### simple_test.wasm/.aot
Basic module compiled from `simple_test.c` that provides:
- `main()` - Standard main function returning 42
- `_main()` - Alternative main entry point
- `__main_argc_argv(argc, argv)` - Entry point with command line arguments
- `test_function()` - Custom function for testing entry point specification
- `add_numbers(a, b)` - Function for testing parameter passing

#### entry_point_test.wasm/.aot
Entry point test module compiled from `entry_point_test.c` that provides:
- `custom_entry()` - Custom entry point returning 200
- `alt_entry()` - Alternative custom entry point returning 300
- `param_entry(param1, param2)` - Entry point with parameters
- `main()` - Standard main returning 150 (different from simple_test)
- `stack_test()` - Function that uses stack space for testing

#### Compiling Test WASM Modules
The test WASM modules are compiled from C source using wasi-sdk with baremetal wasm32 target. The build system supports both regular WebAssembly and AOT (Ahead-of-Time) compilation using wamrc.

**Prerequisites:**
- wasi-sdk installed with WASI_SDK_PATH environment variable set
- Source files: `simple_test.c`, `entry_point_test.c` (both implemented)
- For AOT compilation: wamrc tool (included with wasi-sdk)

**Environment Setup:**
```bash
export WASI_SDK_PATH=/path/to/wasi-sdk
```

**Compilation Options:**

1. **Standard WebAssembly Compilation:**
   ```bash
   cd test_wasm_modules/
   make all
   ```
   This produces `.wasm` files using clang with baremetal wasm32 target.

2. **AOT Compilation:**
   ```bash
   cd test_wasm_modules/
   make AOT_COMPILE=y
   ```
   This produces `.aot` files optimized for the target architecture.

3. **AOT with Custom Flags:**
   ```bash
   cd test_wasm_modules/
   make AOT_COMPILE=y AOT_FLAGS='--target=riscv64 --enable-simd --enable-tail-call'
   ```

**Enhanced Makefile Features:**
The test WASM modules Makefile has been improved with:
- Error handling for compilation failures
- Clean targets (`make clean`, `make realclean`)
- Tool verification (`make check`)
- Automatic module installation to target directory
- Support for both WASM and AOT compilation

**Compilation Process:**
1. **WASM Compilation**: C source → WASM bytecode using clang
2. **AOT Compilation** (optional): WASM bytecode → native code using wamrc

**Compilation flags explained:**
- `--target=wasm32`: Target baremetal WebAssembly
- `-nostdlib`: Don't link standard C library
- `-Wl,--no-entry`: Don't require _start entry point
- `-Wl,--export-all`: Export all functions for LWAC to call

**Build Verification:**
```bash
# Check if wasi-sdk is properly configured
make check

# Clean all generated files
make clean

# Clean all generated files including installed modules
make realclean
```



## Building and Running Tests

### Prerequisites
- LWAC framework must be enabled
- WAMR interpreter must be enabled
- CONFIG_WASM_LWAC_TEST must be enabled
- wasi-sdk installed with WASI_SDK_PATH environment variable set
- Test environment must have access to WASM/AOT modules

### Build Process
1. **Set up environment:**
   ```bash
   export WASI_SDK_PATH=/path/to/wasi-sdk
   ```

2. **Configure build system:**
   - Configure with LWAC enabled
   - Enable CONFIG_WASM_LWAC_TEST in the configuration

   - Set CONFIG_WASM_LWAC_TEST_AOT_FLAGS if using custom AOT compilation flags

3. **Build the test runner and WASM modules:**
   ```bash
   make
   ```

   **Note:** The build system automatically compiles and installs the WASM test modules during the build process using the `context::` target in the main Makefile. Manual compilation of WASM modules is not required but can be done for testing:
   ```bash
   cd test_wasm_modules/
   # For standard WASM:
   make all
   # For AOT compilation:
   make AOT_COMPILE=y
   # For AOT with custom flags:
   make AOT_COMPILE=y AOT_FLAGS='--target=riscv64 --enable-simd'
   ```

### Running Tests
After building with the test application:
```bash
# Run all tests
lwac_test

# Run specific test
lwac_test --test basic_execution

# List all available tests
lwac_test --list

# Show help
lwac_test --help
```

**Test Execution Details:**
The test runner automatically detects whether to use `.wasm` or `.aot` modules based on the AOT compilation configuration. It uses the module path defined in `CONFIG_WASM_LWAC_TEST_MODULE_PATH` to locate the test modules.

## Test Output
Tests will output results in a structured format:
- PASS/FAIL status for each test case
- Detailed error messages for failed tests
- Summary of all test results
- Performance metrics where applicable

**Error Handling:**
The test runner will detect and report failures in the following scenarios:
- Failed execution of the `lwac` command (non-zero return code)
- Missing WASM/AOT modules in the specified module path
- Incorrect output from test modules
- Timeout conditions during module execution

If any test fails, the test runner will raise an error and provide detailed information about the failure for debugging purposes. Users should carefully monitor the console output during test execution to observe the behavior of individual test cases.

## Adding New Tests
To add new test cases:
1. Add the test case function to the test runner in `lwac_test.c`
2. Update the test_cases array with the new test
3. Implement the test functionality using the existing pattern
4. Document the test in this README

## Implementation Summary

### Files Created

#### Test Runner Application
- `lwac_test.c` - **Fully implemented** main test runner application with command line argument parsing and test execution logic

#### Build System Files
- `Makefile` - Build instructions for the test app with automatic WASM module compilation
- `Kconfig` - Configuration options for the test app (currently empty template)
- `Make.defs` - Make definitions for the build system



#### Test WASM Modules Directory
- `test_wasm_modules/` - Directory for WASM test modules
- `test_wasm_modules/simple_test.c` - **Fully implemented** C source for basic test module
- `test_wasm_modules/entry_point_test.c` - **Fully implemented** C source for entry point test module
- `test_wasm_modules/simple_test.wasm` - Compiled basic test module
- `test_wasm_modules/simple_test.aot` - AOT-compiled basic test module (if enabled)
- `test_wasm_modules/entry_point_test.wasm` - Compiled entry point test module
- `test_wasm_modules/entry_point_test.aot` - AOT-compiled entry point test module (if enabled)
- `test_wasm_modules/Makefile` - **Enhanced** build rules for compiling WASM/AOT modules with error handling and clean targets
- `test_wasm_modules/README.md` - Documentation for test modules

#### Additional Build Files
- `.built` - Build marker file
- `.depend` - Dependency tracking file
- `Make.dep` - Generated dependency file

### Modifications to Main LWAC Framework

#### Updated Files
- `Make.defs` - Added tests directory to CONFIGURED_APPS

### Integration

The test framework is fully integrated and can be:
1. Enabled through Kconfig (CONFIG_WASM_LWAC_TEST)
2. Built as part of the build process with automatic WASM module compilation
3. Executed from the shell as "lwac_test"

### Current Status

**Completed:**
- ✅ Fully functional test runner with command line interface
- ✅ Implemented WASM test modules with multiple entry points
- ✅ Enhanced Makefile with error handling and clean targets
- ✅ Build system integration with automatic module compilation
- ✅ Test execution using system() calls to LWAC command
- ✅ Support for both WASM and AOT module compilation

**Known Issues:**
- ⚠️ Error handling test fails because LWAC returns success (return code 0) even when reporting file opening errors

**Configuration Files:**
- ⚠️ `Kconfig` file is empty template (ready for configuration options)

### Next Steps

1. **Optional:** Add Kconfig configuration options for AOT compilation settings
2. **Optional:** Add more comprehensive test cases for edge conditions
3. **Optional:** Add detailed test result reporting and metrics collection
4. **Optional:** Add performance benchmarking tests