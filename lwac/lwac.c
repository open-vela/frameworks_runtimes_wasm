/*
 * Copyright (C) 2025 Xiaomi Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <nuttx/config.h>

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lwac_internal.h"
#include "lwac_module_registry.h"

#include "wamr_external_module_proto.h"

// Default application stack and heap sizes
#define DEFAULT_APP_STACK_SIZE CONFIG_WASM_LWAC_APP_DEFAULT_STACK_SIZE
#define DEFAULT_APP_HEAP_SIZE CONFIG_WASM_LWAC_APP_DEFAULT_HEAP_SIZE

/**
 * Structure to hold command line arguments
 */
typedef struct {
    char* filename; // Input filename
    int stack_size; // Stack size in bytes
    int heap_size; // Heap size in bytes
    int no_mmap; // Flag to use malloc instead of mmap
    char* entry_func; // Entry function name
    int show_ps; // Flag to show running modules
    // Add more parameters here as needed
} lwac_args_t;

/**
 * Struct to hold WASM execution parameters
 */
typedef struct {
    void* file_content;
    int file_size;
    int stack_size;
    int heap_size;
    const char* entry_func;
    const char* file_path;
} lwac_exec_params_t;

/**
 * Reference counting for WASM runtime management
 *
 * The WASM runtime is a shared resource that needs to be initialized once
 * and destroyed when no longer used. Since multiple instances of this
 * application can run simultaneously, we use reference counting to:
 *
 * 1. Initialize the runtime only when the first instance starts
 * 2. Keep the runtime alive while any instances are running
 * 3. Destroy the runtime only when the last instance exits
 *
 * This prevents both resource leaks and crashes from multiple init/destroy calls.
 */
static int g_wasm_runtime_ref_count = 0;
static pthread_mutex_t g_wasm_runtime_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef bool (*module_register_t)(void);

static const module_register_t g_wamr_modules[] = {
#include "wamr_external_module_list.h"
};

// Function to increment reference count and initialize runtime if needed
static int lwac_runtime_init(void)
{
    int ret = 0;
    pthread_mutex_lock(&g_wasm_runtime_mutex);

    if (g_wasm_runtime_ref_count == 0) {
        // Initialize WASM runtime on first use
        if (!wasm_runtime_init()) {
            printf("Error: failed to initialize WASM runtime\n");
            ret = -1;
            goto unlock;
        }

        for (int i = 0; i < sizeof(g_wamr_modules) / sizeof(g_wamr_modules[0]); i++) {
            if (!g_wamr_modules[i]()) {
                printf("Error: failed to register external module %d\n", i);
                ret = -1;
                goto unlock;
            }
        }
    }

    g_wasm_runtime_ref_count++;

unlock:
    pthread_mutex_unlock(&g_wasm_runtime_mutex);
    return ret;
}

// Function to decrement reference count and destroy runtime if no more references
static int lwac_runtime_destroy(void)
{
    pthread_mutex_lock(&g_wasm_runtime_mutex);

    g_wasm_runtime_ref_count--;

    if (g_wasm_runtime_ref_count == 0) {
        // Destroy WASM runtime when no more instances are using it
        wasm_runtime_destroy();
    }

    pthread_mutex_unlock(&g_wasm_runtime_mutex);
    return 0;
}

static void print_usage(void)
{
    printf("Usage: lwac [file]\n");
    printf("Options:\n");
    printf("  -m file          Specify input module file\n");
    printf("  -f func          Specify entry function name (default: main/_main/__main_argc_argv)\n");
    printf("  --stack-size n   Specify stack size (integer value, default: %d)\n", DEFAULT_APP_STACK_SIZE);
    printf("  --heap-size n    Specify heap size (integer value, default: %d)\n", DEFAULT_APP_HEAP_SIZE);
    printf("  --no-mmap        Use malloc instead of mmap to load file\n");
    printf("  --ps             Show running modules\n");
    printf("  -h               Display this help message\n");
}

/**
 * Parse command line arguments
 * @param argc Argument count
 * @param argv Argument vector
 * @param args Pointer to parsed_args_t struct to store parsed arguments
 * @return 0 on success, -1 on parsing failure, 1 for help display
 */
static int parse_arguments(int argc, char* argv[], lwac_args_t* args)
{
    int opt;
    int f_flag = 0; // Tracks if filename was provided via -m option rather than as a positional argument
    int stack_size, heap_size; // Variables for validation before assignment
    args->filename = NULL;
    args->stack_size = DEFAULT_APP_STACK_SIZE; // Default stack size
    args->heap_size = DEFAULT_APP_HEAP_SIZE; // Default heap size
    args->no_mmap = 0; // Default to using mmap
    args->entry_func = NULL; // Default to NULL (auto-detect)
    args->show_ps = 0; // Default to not show ps

    static struct option long_options[] = {
        { "stack-size", required_argument, 0, 's' },
        { "heap-size", required_argument, 0, 'e' },
        { "no-mmap", no_argument, 0, 'n' },
        { "ps", no_argument, 0, 'p' },
        { 0, 0, 0, 0 }
    };

    int option_index = 0;

    // Parse command line options using getopt_long
    while ((opt = getopt_long(argc, argv, "m:f:ph", long_options, &option_index)) != -1) {
        switch (opt) {
        case 'm':
            args->filename = optarg;
            f_flag = 1;
            break;
        case 'f':
            args->entry_func = optarg;
            break;
        case 's':
            stack_size = atoi(optarg);
            if (stack_size <= 0) {
                printf("Error: Invalid stack size '%s'. Must be a positive integer.\n", optarg);
                return -1;
            }
            args->stack_size = stack_size;
            break;
        case 'e':
            heap_size = atoi(optarg);
            if (heap_size <= 0) {
                printf("Error: Invalid heap size '%s'. Must be a positive integer.\n", optarg);
                return -1;
            }
            args->heap_size = heap_size;
            break;
        case 'n':
            args->no_mmap = 1;
            break;
        case 'p':
            args->show_ps = 1;
            break;
        case 'h':
            print_usage();
            return 1;
        default:
            print_usage();
            return -1;
        }
    }

    // If no -m option but there's a positional argument
    if (!f_flag && optind < argc) {
        args->filename = argv[optind];
    } else if (f_flag && optind < argc) {
        // Both -m and positional arguments are provided
        // We prioritize the -m flag and warn about unused positional argument
        printf("Warning: Both -m option and positional argument provided. Using file '%s' from -m option.\n",
            args->filename);
    }

    // Check if we have a filename
    if (args->filename == NULL && !args->show_ps) {
        printf("No input file specified.\n");
        print_usage();
        return -1;
    }

    return 0;
}

/**
 * Map a file into memory using mmap or load it using malloc
 * @param filename Path to the file
 * @param size Pointer to store the size of the mapped/loaded file
 * @param use_malloc Flag to indicate whether to use malloc instead of mmap
 * @return Pointer to the file content, NULL on error
 */
static void* load_file(const char* filename, int* size, int use_malloc)
{
    int fd;
    struct stat file_stat;
    void* file_content = NULL;

    // Open the input file
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("Error opening file '%s': errno=%d\n", filename, errno);
        return NULL;
    }

    // Get file statistics
    if (fstat(fd, &file_stat) != 0) {
        printf("Error getting file stats: errno=%d\n", errno);
        close(fd);
        return NULL;
    }

    // Store the file size
    *size = file_stat.st_size;

    if (use_malloc) {
        // Allocate memory and read the file
        file_content = malloc(file_stat.st_size);
        if (file_content == NULL) {
            printf("Error allocating memory for file (size: %lld bytes): errno=%d\n",
                (long long)file_stat.st_size, errno);
            close(fd);
            return NULL;
        }

        // Read the entire file into the allocated memory
        ssize_t bytes_read = read(fd, file_content, file_stat.st_size);
        if (bytes_read != file_stat.st_size) {
            printf("Error reading file, expected %lld bytes, got %d: errno=%d\n",
                (long long)file_stat.st_size, (int)bytes_read, errno);
            free(file_content);
            close(fd);
            return NULL;
        }
    } else {
        // Map the file into memory
        file_content = mmap(NULL, file_stat.st_size, PROT_READ | PROT_EXEC, MAP_PRIVATE, fd, 0);
        if (file_content == MAP_FAILED) {
            printf("Error mapping file: errno=%d\n", errno);
            close(fd);
            return NULL;
        }
    }

    // Close the file descriptor
    close(fd);
    return file_content;
}

/**
 * Free memory allocated for file content
 * @param content Pointer to the file content
 * @param size Size of the file
 * @param use_malloc Flag indicating whether malloc was used
 */
static void free_file_content(void* content, int size, int use_malloc)
{
    if (content) {
        if (use_malloc) {
            free(content);
        } else {
            munmap(content, size);
        }
    }
}

/**
 * Execute a WebAssembly module using the provided execution parameters.
 *
 * This function is responsible for the full lifecycle of loading, instantiating, and executing
 * a WebAssembly (WASM) module using the runtime APIs. It performs the following steps:
 *
 * 1. Loads the WASM binary from memory (provided in params->file_content and params->file_size).
 * 2. Instantiates the module with the specified stack and heap sizes.
 * 3. Creates an execution environment for running WASM functions.
 * 4. Locates the entry function to invoke (either user-specified or one of several common defaults).
 * 5. Calls the entry function with a default argc/argv (currently two zeroed arguments).
 * 6. Handles and reports any errors encountered during these steps.
 * 7. Cleans up all allocated resources (execution environment, module instance, and module) before returning.
 *
 * Error handling is performed at each stage, with detailed messages printed to stdout on failure.
 * The function ensures that all resources are properly released even if an error occurs partway through.
 *
 * @param params Pointer to lwac_exec_params_t containing all execution parameters
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure
 */
static int execute_wasm_module(const lwac_exec_params_t* params)
{
    char error_buf[128];
    int ret;
    wasm_module_t module = NULL;
    wasm_module_inst_t module_inst = NULL;
    wasm_exec_env_t exec_env = NULL;
    wasm_function_inst_t entry_func = NULL;
    uint32_t argv_buf[2] = { 0 };

    // Load the WASM module
    module = wasm_runtime_load(params->file_content, params->file_size, error_buf, sizeof(error_buf));
    if (!module) {
        printf("Error loading WASM module: %s\n", error_buf);
        return EXIT_FAILURE;
    }

    // Instantiate the WASM module
    module_inst = wasm_runtime_instantiate(module,
        params->stack_size,
        params->heap_size,
        error_buf,
        sizeof(error_buf));
    if (!module_inst) {
        printf("Error instantiating WASM module: %s\n", error_buf);
        ret = EXIT_FAILURE;
        goto cleanup_module;
    }

    // Create execution environment
    exec_env = wasm_runtime_create_exec_env(module_inst, params->stack_size);
    if (!exec_env) {
        printf("Error creating execution environment\n");
        ret = EXIT_FAILURE;
        goto cleanup_instance;
    }

    // Look up the entry function
    if (params->entry_func && params->entry_func[0]) {
        entry_func = wasm_runtime_lookup_function(module_inst, params->entry_func);
        if (!entry_func) {
            printf("Error: Entry function '%s' not found\n", params->entry_func);
            ret = EXIT_FAILURE;
            goto cleanup_env;
        }
    } else {
        if (!(entry_func = wasm_runtime_lookup_function(module_inst, "__main_argc_argv"))) {
            if (!(entry_func = wasm_runtime_lookup_function(module_inst, "_main"))) {
                if (!(entry_func = wasm_runtime_lookup_function(module_inst, "main"))) {
                    printf("Error: Entry function 'main' not found\n");
                    ret = EXIT_FAILURE;
                    goto cleanup_env;
                }
            }
        }
    }

    // Register the module in the info system after entry function lookup OK
    wasm_module_info_t* module_info = NULL;
    module_info = lwac__register_wasm_module(params->file_path ? params->file_path : "unknown",
        params->stack_size,
        params->heap_size,
        module,
        module_inst,
        exec_env);
    if (!module_info) {
        printf("Warning: Failed to register module in info system\n");
    }

    if (!wasm_runtime_call_wasm(exec_env, entry_func, 2, argv_buf)) {
        printf("Error executing entry function: %s\n",
            wasm_runtime_get_exception(module_inst));
        ret = EXIT_FAILURE;
    } else {
        ret = EXIT_SUCCESS;
    }

cleanup_env:
    // Clean up execution environment
    wasm_runtime_destroy_exec_env(exec_env);

    // Unregister the module from the info system
    if (module_inst) {
        lwac__unregister_wasm_module(module_inst);
    }

cleanup_instance:
    // Destroy module instance
    wasm_runtime_deinstantiate(module_inst);

cleanup_module:
    wasm_runtime_unload(module);

    return ret;
}

int main(int argc, char* argv[])
{
    lwac_args_t args = { 0 };
    int file_size = 0;
    int ret = EXIT_FAILURE;
    void* file_content = NULL;
    lwac_exec_params_t exec_params;

    ret = parse_arguments(argc, argv, &args);
    if (ret != 0) {
        ret = ret < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
        goto out;
    }

    // Handle ps option
    if (args.show_ps) {
        lwac__print_running_modules();
        ret = EXIT_SUCCESS;
        goto out;
    }

    file_content = load_file(args.filename, &file_size, args.no_mmap);
    if (file_content == NULL) {
        goto out;
    }

    // Reference count increment - initializes runtime if it's the first instance
    if (lwac_runtime_init() != 0) {
        printf("Error: Failed to initialize WASM runtime\n");
        goto out;
    }

    // Pack parameters into struct and execute the WASM module
    exec_params.file_content = file_content;
    exec_params.file_size = file_size;
    exec_params.stack_size = args.stack_size;
    exec_params.heap_size = args.heap_size;
    exec_params.entry_func = args.entry_func;
    exec_params.file_path = args.filename;

    // Check if the file is XIP compatible when using mmap
    if (!wasm_runtime_is_xip_file(exec_params.file_content, exec_params.file_size)) {
        if (!args.no_mmap) {
            printf("Error: The file is not a XIP file, please use --no-mmap option to run it in RAM.\n");
            goto cleanup_runtime;
        }
    }

    ret = execute_wasm_module(&exec_params);

cleanup_runtime:
    lwac_runtime_destroy();

out:
    free_file_content(file_content, file_size, args.no_mmap);
    return ret;
}
