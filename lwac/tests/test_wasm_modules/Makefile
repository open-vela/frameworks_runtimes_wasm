# Makefile for compiling LWAC test WASM modules
# Requires wasi-sdk to be installed and WASI_SDK_PATH to be set

# Check required environment variables
ifndef APPDIR
$(error APPDIR must be set. Please run from the project root or set APPDIR environment variable)
endif
ifndef WASI_SDK_PATH
$(error WASI_SDK_PATH must be set. Please set WASI_SDK_PATH environment variable)
endif

# Compiler settings - use WASI SDK if available
CC = $(WASI_SDK_PATH)/bin/clang
WAMRC = $(WASI_SDK_PATH)/wamrc
# Essential compile flags from bouffalo CMakeLists.txt
WASM_COMPILE_FLAGS = -fdata-sections -ffunction-sections -Oz -flto \
                     -fno-unwind-tables -fno-asynchronous-unwind-tables \
                     -fomit-frame-pointer -fno-exceptions -fno-rtti -fno-builtin \
                     -fno-stack-protector

# Essential link flags from bouffalo CMakeLists.txt
WASM_LINK_FLAGS = --target=wasm32 -nostdlib -Wl,--no-entry \
                  -Wl,--export=__heap_base \
                  -Wl,--export=__data_end \
                  -Wl,--initial-memory=65536 \
                  -Wl,--gc-sections \
				  -Wl,--allow-undefined \
                  -z stack-size=2048

# Combined WASM flags
WASM_FLAGS = $(WASM_COMPILE_FLAGS) $(WASM_LINK_FLAGS)
WASM_OPT_FLAGS =

# AOT compilation settings (can be overridden by Kconfig)
AOT_COMPILE ?= n
AOT_FLAGS ?=

# Installation directory for compiled WASM modules
WASM_BIN_DIR = $(APPDIR)/bin/wasm

# Source and target files
SOURCES = simple_test.c entry_point_test.c
WASM_TARGETS = $(SOURCES:.c=.wasm)
AOT_TARGETS = $(SOURCES:.c=.aot)

# Determine final targets based on AOT compilation setting
ifeq ($(AOT_COMPILE),y)
TARGETS = $(SOURCES:.c=.aot)
else
TARGETS = $(WASM_TARGETS)
endif

# Default target - check tools first, then build and install
all: check $(TARGETS) install

# Rule to compile .c to .wasm with error checking
%.wasm: %.c
	$(CC) $(WASM_FLAGS) $(WASM_OPT_FLAGS) -o $@ $< || { echo "Error: Failed to compile $<"; exit 1; }
	@echo "Compiled $< to $@"

# Rule to compile .wasm to .aot using wamrc with error checking
%.aot: %.wasm
	$(WAMRC) $(AOT_FLAGS) -o $@ $< || { echo "Error: Failed to AOT compile $<"; exit 1; }
	@echo "AOT compiled $< to $@"

# Note: Individual targets are now handled by pattern rules above

# Installation target - copy compiled modules to APPDIR/bin/wasm
install: $(TARGETS)
	@echo "Installing WASM modules to $(WASM_BIN_DIR)"
	@mkdir -p $(WASM_BIN_DIR)
	@cp $(TARGETS) $(WASM_BIN_DIR)/ || { echo "Error: Failed to install modules"; exit 1; }
	@echo "Installed: $(TARGETS) -> $(WASM_BIN_DIR)/"

# Check if wasi-sdk is available
check:
	@test -n "$(WASI_SDK_PATH)" || (echo "Error: WASI_SDK_PATH not set. Please set WASI_SDK_PATH environment variable"; exit 1)
	@test -f "$(CC)" || (echo "Error: WASI SDK clang not found at $(CC). Please check WASI_SDK_PATH"; exit 1)
	@echo "WASI SDK found: $(CC)"
ifeq ($(AOT_COMPILE),y)
	@test -f "$(WAMRC)" || (echo "Error: WAMRC not found at $(WAMRC). Please check WASI_SDK_PATH"; exit 1)
	@echo "WAMRC found: $(WAMRC)"
endif

# Help target
help:
	@echo "Available targets:"
	@echo "  all       - Compile all WASM modules (AOT if AOT_COMPILE=y) and install"
	@echo "  install   - Install compiled modules to \$$(APPDIR)/bin/wasm"
	@echo "  check     - Check if wasi-sdk and wamrc are available"
	@echo "  clean     - Remove compiled WASM and AOT files"
	@echo "  realclean - Remove all compiled and installed files"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  AOT_COMPILE=y|n    - Enable/disable AOT compilation (default: n)"
	@echo "  AOT_FLAGS=<flags>  - Flags for wamrc AOT compiler"
	@echo "  APPDIR=<path>      - Application directory (default: ../../../..)"
	@echo ""
	@echo "Installation directory: $(WASM_BIN_DIR)"
	@echo ""
	@echo "Examples:"
	@echo "  make                              # Compile to WASM and install"
	@echo "  make AOT_COMPILE=y                # Compile to AOT and install"
	@echo "  make AOT_COMPILE=y AOT_FLAGS='--target=riscv64 --enable-simd'"
	@echo "  make install                      # Install already compiled modules"
	@echo "  make clean                        # Remove compiled files"
	@echo "  make realclean                    # Remove all generated files"

# Clean target - remove all compiled artifacts
clean:
	@echo "Cleaning compiled WASM and AOT files..."
	@rm -f $(WASM_TARGETS) $(AOT_TARGETS)
	@echo "Clean complete"

# Real clean - also remove installed files
realclean: clean
	@echo "Removing installed files..."
	@rm -rf $(WASM_BIN_DIR)
	@echo "Real clean complete"

.PHONY: all install check help clean realclean