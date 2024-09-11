# README for SDK 2.0

This folder is intended to hold the major implementation of the SDK 2.0:
- The `examples` folder contains a set of examples that show how to use the SDK in different scenarios.
- The `include` folder contains the header files needed by the SDK.
- The `src` folder contains the source code for the SDK.
- The `CMakeLists.txt` file is used by CMake to build the app with the SDK.
- The `cmake` folder contains the CMake modules that are used by the SDK.
  - The `wasi-sdk.cmake` file is used to configure the comiler and linker for the Wasm target.
  - The `defined-symbols.txt` file is used to provide a list of symbols that are exported from the BLFB native library.

# Setup

To use the SDK, you need to prepare the original source code of the Bouffalo SDK 2.0 and put it in a folder named `bouffalo_sdk`
in the same directory as this README file.

The cmake based build system will check if the `bouffalo_sdk` folder exists, and if not, it will report an error,
clone the repository from GitHub and put it together with the README.md file in the same directory:

```bash
git clone https://github.com/bouffalolab/bouffalo_sdk.git -b V2.0.0 bouffalo_sdk
```

# Build

To build the app with the SDK, you need to run CMake under root directory (where this README.md file is located):

```bash
cmake -Bbuild . -DAPP=examples/gpio_output
cmake --build build
```

Then you can find the executable wasm file `gpio.wasm` and `gpio.aot` in the `build/examples/gpio_output` folder.

# Examples

The examples are located under the `examples` directory, each example is a separate CMake project with its own `CMakeLists.txt`.

Currently, there are several examples:
- `adc_poll`: This example demonstrates how to use bouffalo SDK 2.0 API to read ADC value by polling.
- `adc_tsen`: This example demonstrates how to use bouffalo SDK 2.0 API to read temperature value by polling.
- `adc_vbat`: This example demonstrates how to use bouffalo SDK 2.0 API to read battery voltage value by polling.
- `i2c_10_bit`: This example demonstrates how to use bouffalo SDK 2.0 API to write data to i2c by polling mode.
- `i2c_eeprom`: This example demonstrates how to use bouffalo SDK 2.0 API to read/write data from/to i2c by polling mode.
- `ir_nec`: This example demonstrates how to use bouffalo SDK 2.0 API to recieve data from ir by NEC protocol.
- `ir_rc5`: This example demonstrates how to use bouffalo SDK 2.0 API to recieve data from ir by RC5 protocol.
- `ir_swm`: This example demonstrates how to use bouffalo SDK 2.0 API to recieve data from ir by software mode.
- `gpio_interrupt`: This example demonstrates how to use bouffalo SDK 2.0 API to set GPIO pin as input and trigger interrupt when the pin is high.
- `gpio_output`: This example demonstrates how to use bouffalo SDK 2.0 API to set GPIO pin as output and toggle it.
- `gpio_output_raw`: This example demonstrates how to use raw register access to control GPIO pins, used for debugging register access issues.
- `uart_fifo_interrupt`: This example demonstrates how to read and write data from/to UART using interrupt mode.
- `uart_poll`: This example demonstrates how to use bouffalo SDK 2.0 API to read and write data from/to UART by polling mode.

# Configuration

The configuration is done via CMake variables:
- `APP`: Required, the name of the application to build (e.g., `examples/gpio_output`)
- `WASI_SDK_PATH`: Required, path to the WASI SDK installation directory
- `VELA_SYSROOT`: Optional, path to the Vela sysroot directory, if not specified, it will be automatically detected
- `APP_STACK_SIZE`: Optional, stack size of the application, default is 2048 (bytes)

# Notice

1. The `defined-symbols.txt` file is used to identify which symbols are available in the Wasm runtime, which helps the compiler and linker to check for undefined references and link them at compile time. You should not modify this file manually to add new symbols. If an unsupported symbol is referenced by your code, it will cause a runtime error even if you have added the corresponding definition in `defined-symbols.txt`.

2. Not all of the BFLB SDK APIs are compiled into WebAssembly, since some of them rely on hardware-specific instructions or features that are tightly coupled with the underlying operating system and cannot be emulated by Wasm. For example, ROM functions, mtimer based delay functions, etc. Full list can be found in `cmake/defined-symbols.txt`.

Supported Peripherals:
- GPIO (Input/Output)
- UART (Poll)
- ADC (Poll)
- I2C (Poll)
- IR (Poll)

WIP Peripherals:
- GPIO (Interrupt)
- UART (Interrupt)
- ADC (Interrupt)
- I2C (Interrupt)
- PWM (Poll/Interrupt)
- SPI (Poll/Interrupt)
