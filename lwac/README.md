# LWAC - Lightweight WebAssembly Application Container

LWAC is a command-line tool for executing WebAssembly (WASM) modules on NuttX using the WAMR (WebAssembly Micro Runtime) interpreter.

## Features

- Execute WebAssembly modules with customizable stack and heap sizes
- Support for both mmap and malloc-based file loading
- Automatic entry function detection (main, _main, __main_argc_argv)
- Custom entry function specification
- XIP (Execute In Place) compatibility checking
- Reference-counted runtime management for concurrent execution

## Building and Configuration

### Kconfig Options

Enable manually in your menuconfig:

```kconfig
CONFIG_WASM_LWAC=y
CONFIG_WASM_LWAC_PRIORITY=100
CONFIG_WASM_LWAC_STACKSIZE=2048
CONFIG_WASM_LWAC_APP_DEFAULT_STACK_SIZE=2048
CONFIG_WASM_LWAC_APP_DEFAULT_HEAP_SIZE=4096
```

#### Configuration Parameters

| Option | Description | Default |
|--------|-------------|---------|
| `CONFIG_WASM_LWAC` | Enable LWAC application | n |
| `CONFIG_WASM_LWAC_PRIORITY` | Task priority for LWAC | 100 |
| `CONFIG_WASM_LWAC_STACKSIZE` | Stack size for LWAC task itself | 2048 |
| `CONFIG_WASM_LWAC_APP_DEFAULT_STACK_SIZE` | Default stack size for WASM apps | 2048 |
| `CONFIG_WASM_LWAC_APP_DEFAULT_HEAP_SIZE` | Default heap size for WASM apps | 4096 |

### Dependencies

LWAC requires the WAMR interpreter to be enabled:
```kconfig
CONFIG_INTERPRETERS_WAMR=y
```

## Usage

### Basic Syntax

```bash
lwac [options] [file]
```

### Command Line Options

| Option | Description | Default |
|--------|-------------|---------|
| `-m file` | Specify input WASM module file | - |
| `-f func` | Specify entry function name | Auto-detect |
| `--stack-size n` | Set stack size in bytes | From Kconfig |
| `--heap-size n` | Set heap size in bytes | From Kconfig |
| `--no-mmap` | Use malloc instead of mmap | Use mmap |
| `-h` | Display help message | - |

### Examples

#### Basic execution
```bash
lwac hello.wasm
```

#### Using command line options
```bash
lwac -m hello.wasm -f main --stack-size 4096 --heap-size 8192
```

#### Custom entry function
```bash
lwac -m app.wasm -f custom_entry
```

#### Force RAM loading (disable XIP)
```bash
lwac --no-mmap app.wasm
```

#### Using positional argument
```bash
lwac /path/to/module.wasm
```

## Entry Function Detection

LWAC automatically detects the entry function in the following order:

1. `__main_argc_argv` (C main with arguments)
2. `_main` (C main function)
3. `main` (Standard main function)

You can override this by specifying a custom function with the `-f` option.

## Memory Management

### Stack and Heap Sizing

- **Stack Size**: Controls the execution stack for the WASM module
- **Heap Size**: Controls the linear memory available to the WASM module

### File Loading Modes

#### mmap Mode (Default)
- Maps the WASM file directly from storage
- Supports XIP (Execute In Place) for compatible files
- More memory efficient
- Requires XIP-compatible WASM files

#### malloc Mode (--no-mmap)
- Loads the entire file into RAM
- Compatible with all WASM files
- Uses more memory but more flexible
- Required for non-XIP WASM files

## XIP (Execute In Place) Support

LWAC automatically checks if a WASM file is XIP-compatible when using mmap mode. If the file is not XIP-compatible, you'll see:

```
Error: The file is not a XIP file, please use --no-mmap option to run it in RAM.
```

Use the `--no-mmap` option to load such files into RAM.

## Runtime Management

LWAC uses reference counting for runtime management:

- First instance initializes the WASM runtime
- Runtime stays active while any instances are running
- Last instance cleans up the runtime
- Supports concurrent execution of multiple WASM modules

## Error Handling

Common error scenarios and solutions:

### File Not Found
```
Error opening file 'nonexistent.wasm': errno=2
```
**Solution**: Check file path and permissions

### Memory Allocation Failed
```
Error allocating memory for file (size: 1048576 bytes): errno=12
```
**Solution**: Increase available memory or use mmap mode

### Entry Function Not Found
```
Error: Entry function 'custom_main' not found
```
**Solution**: Check function name or use auto-detection

### XIP Compatibility
```
Error: The file is not a XIP file, please use --no-mmap option to run it in RAM.
```
**Solution**: Add `--no-mmap` flag

## Building WASM Applications

To create WASM files compatible with LWAC, compile your C/C++ code with:

```bash
# Using wasi-sdk
clang --target=wasm32-wasi -o app.wasm app.c

# Using Emscripten
emcc -o app.wasm app.c
```

## Troubleshooting

1. **Module fails to load**: Ensure the WASM file is valid and compatible with WAMR
2. **Out of memory errors**: Increase heap/stack sizes or available system memory
3. **Function not found**: Verify the entry function exists in the WASM module
4. **Permission denied**: Check file permissions and storage access

## Performance Considerations

- Use mmap mode when possible for better memory efficiency
- Tune stack and heap sizes based on your application needs
- Consider XIP-compatible builds for embedded systems with limited RAM
- Monitor system memory usage when running multiple instances

## License

Licensed under the Apache License, Version 2.0. See the source files for full license text.
