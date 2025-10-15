/****************************************************************************
 * frameworks/wasm/lwac/tests/lwac_test.c
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define TEST_RESULT_PASS 0
#define TEST_RESULT_FAIL 1

/* Default module path - can be overridden with environment variable */
#define DEFAULT_MODULE_PATH CONFIG_WASM_LWAC_TEST_MODULE_PATH
#define MAX_CMD_LEN 128

/* Use .wasm extension for modules */
#define MODULE_EXT ".wasm"

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const char* g_test_names[] = {
    "basic_execution",
    "argument_parsing",
    "module_registry",
    "file_loading",
    "error_handling"
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: print_usage
 ****************************************************************************/

static void print_usage(const char* progname)
{
    printf("Usage: %s [OPTIONS]\n", progname);
    printf("  --test <test_name>    Run specific test\n");
    printf("  --list                List all available tests\n");
    printf("  --help                Show this help message\n");
}

/****************************************************************************
 * Name: list_tests
 ****************************************************************************/

static void list_tests(void)
{
    int i;
    printf("Available tests:\n");
    for (i = 0; i < sizeof(g_test_names) / sizeof(g_test_names[0]); i++) {
        printf("  %s\n", g_test_names[i]);
    }
}

/****************************************************************************
 * Name: run_basic_execution_test
 ****************************************************************************/

static int run_basic_execution_test(void)
{
    int ret;
    char cmd[MAX_CMD_LEN];

    printf("Running basic execution test...\n");

    /* Execute simple_test module with appropriate extension */
    snprintf(cmd, sizeof(cmd), "lwac --no-mmap -m %s/simple_test%s", DEFAULT_MODULE_PATH, MODULE_EXT);
    printf("Executing: %s\n", cmd);

    ret = system(cmd);
    if (ret != 0) {
        printf("Basic execution test failed with return code: %d\n", ret);
        return TEST_RESULT_FAIL;
    }

    printf("Basic execution test passed\n");
    return TEST_RESULT_PASS;
}

/****************************************************************************
 * Name: run_argument_parsing_test
 ****************************************************************************/

static int run_argument_parsing_test(void)
{
    int ret;
    char cmd[MAX_CMD_LEN];

    printf("Running argument parsing test...\n");

    /* Test with custom arguments */
    snprintf(cmd, sizeof(cmd), "lwac --no-mmap -m %s/simple_test%s arg1 arg2", DEFAULT_MODULE_PATH, MODULE_EXT);
    printf("Executing: %s\n", cmd);

    ret = system(cmd);
    if (ret != 0) {
        printf("Argument parsing test failed with return code: %d\n", ret);
        return TEST_RESULT_FAIL;
    }

    /* Test with stack size option */
    snprintf(cmd, sizeof(cmd), "lwac --no-mmap -m %s/simple_test%s --stack-size=8192", DEFAULT_MODULE_PATH, MODULE_EXT);
    printf("Executing: %s\n", cmd);

    ret = system(cmd);
    if (ret != 0) {
        printf("Argument parsing test failed with return code: %d\n", ret);
        return TEST_RESULT_FAIL;
    }

    printf("Argument parsing test passed\n");
    return TEST_RESULT_PASS;
}

/****************************************************************************
 * Name: run_module_registry_test
 ****************************************************************************/

static int run_module_registry_test(void)
{
    int ret;
    char cmd[MAX_CMD_LEN];

    printf("Running module registry test...\n");

    /* Test module registry with --ps option */
    snprintf(cmd, sizeof(cmd), "lwac --ps");
    printf("Executing: %s\n", cmd);

    ret = system(cmd);
    if (ret != 0) {
        printf("Module registry test failed with return code: %d\n", ret);
        return TEST_RESULT_FAIL;
    }

    /* Test execution with registry tracking */
    /* First start the module in background */
    snprintf(cmd, sizeof(cmd), "lwac --no-mmap -m %s/simple_test%s &", DEFAULT_MODULE_PATH, MODULE_EXT);
    printf("Executing: %s\n", cmd);

    ret = system(cmd);
    if (ret != 0) {
        printf("Module registry test failed with return code: %d\n", ret);
        return TEST_RESULT_FAIL;
    }

    /* Then check the registry */
    snprintf(cmd, sizeof(cmd), "lwac --ps");
    printf("Executing: %s\n", cmd);

    ret = system(cmd);
    if (ret != 0) {
        printf("Module registry test failed with return code: %d\n", ret);
        return TEST_RESULT_FAIL;
    }

    printf("Module registry test passed\n");
    return TEST_RESULT_PASS;
}

/****************************************************************************
 * Name: run_file_loading_test
 ****************************************************************************/

static int run_file_loading_test(void)
{
    int ret;
    char cmd[MAX_CMD_LEN];

    printf("Running file loading test...\n");

    /* Test RAM-based loading */
    snprintf(cmd, sizeof(cmd), "lwac --no-mmap -m %s/simple_test%s", DEFAULT_MODULE_PATH, MODULE_EXT);
    printf("Executing: %s\n", cmd);

    ret = system(cmd);
    if (ret != 0) {
        printf("File loading test failed with return code: %d\n", ret);
        return TEST_RESULT_FAIL;
    }

    printf("File loading test passed\n");
    return TEST_RESULT_PASS;
}

/****************************************************************************
 * Name: run_error_handling_test
 ****************************************************************************/

static int run_error_handling_test(void)
{
    int ret;
    char cmd[MAX_CMD_LEN];

    printf("Running error handling test...\n");

    /* Test error handling for non-existent file */
    snprintf(cmd, sizeof(cmd), "lwac -m %s/non_existent%s", DEFAULT_MODULE_PATH, MODULE_EXT);
    printf("Executing: %s\n", cmd);

    ret = system(cmd);
    /* Expecting non-zero return code for error */
    if (ret == 0) {
        printf("Error handling test failed - expected error but got success\n");
        return TEST_RESULT_FAIL;
    }

    /* Test error handling for invalid arguments */
    snprintf(cmd, sizeof(cmd), "lwac --invalid-option");
    printf("Executing: %s\n", cmd);

    ret = system(cmd);
    /* Expecting non-zero return code for error */
    if (ret == 0) {
        printf("Error handling test failed - expected error but got success\n");
        return TEST_RESULT_FAIL;
    }

    printf("Error handling test passed\n");
    return TEST_RESULT_PASS;
}

/****************************************************************************
 * Name: run_specific_test
 ****************************************************************************/

static int run_specific_test(const char* test_name)
{
    if (strcmp(test_name, "basic_execution") == 0) {
        return run_basic_execution_test();
    } else if (strcmp(test_name, "argument_parsing") == 0) {
        return run_argument_parsing_test();
    } else if (strcmp(test_name, "module_registry") == 0) {
        return run_module_registry_test();
    } else if (strcmp(test_name, "file_loading") == 0) {
        return run_file_loading_test();
    } else if (strcmp(test_name, "error_handling") == 0) {
        return run_error_handling_test();
    } else {
        printf("Unknown test: %s\n", test_name);
        return TEST_RESULT_FAIL;
    }
}

/****************************************************************************
 * Name: run_all_tests
 ****************************************************************************/

static int run_all_tests(void)
{
    int result = TEST_RESULT_PASS;
    int i;

    printf("Running all tests...\n");
    printf("Using module path: %s\n", DEFAULT_MODULE_PATH);

    for (i = 0; i < sizeof(g_test_names) / sizeof(g_test_names[0]); i++) {
        if (run_specific_test(g_test_names[i]) != TEST_RESULT_PASS) {
            result = TEST_RESULT_FAIL;
        }
    }

    return result;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: main
 ****************************************************************************/

int main(int argc, char* argv[])
{
    int option_index = 1;

    /* Print module path for reference */
    printf("Using module path: %s\n", DEFAULT_MODULE_PATH);

    /* Parse command line arguments */
    while (option_index < argc) {
        if (strcmp(argv[option_index], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[option_index], "--list") == 0) {
            list_tests();
            return 0;
        } else if (strcmp(argv[option_index], "--test") == 0) {
            if (option_index + 1 < argc) {
                return run_specific_test(argv[option_index + 1]);
            } else {
                printf("Error: --test requires a test name argument\n");
                print_usage(argv[0]);
                return 1;
            }
        } else {
            printf("Unknown option: %s\n", argv[option_index]);
            print_usage(argv[0]);
            return 1;
        }

        option_index++;
    }

    /* Run all tests if no specific test was requested */
    return run_all_tests();
}