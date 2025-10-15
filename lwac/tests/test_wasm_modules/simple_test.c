/*
 * Simple WebAssembly test module for LWAC testing
 * This module provides basic functionality to test WASM execution
 */

extern int usleep(unsigned int usec);

// Export a simple main function that returns a success code
int main(void)
{
    return 42; // Return a recognizable success code
}

// Export alternative entry points for testing
__attribute__((export_name("_main"))) int _main(void)
{
    return 42;
}

__attribute__((export_name("__main_argc_argv"))) int __main_argc_argv(int argc, char** argv)
{
    usleep(500000);
    return argc;
}

// Export a test function that can be called with custom entry point
__attribute__((export_name("test_function"))) int test_function(void)
{
    return 100;
}

// Export a function that performs basic arithmetic
__attribute__((export_name("add_numbers"))) int add_numbers(int a, int b)
{
    return a + b;
}