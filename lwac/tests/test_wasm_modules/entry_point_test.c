/*
 * Entry point test WebAssembly module for LWAC testing
 * This module tests custom entry point functionality
 */

// Custom entry point function
__attribute__((export_name("custom_entry"))) int custom_entry(void)
{
    return 200; // Return a recognizable code for custom entry
}

// Alternative custom entry point
__attribute__((export_name("alt_entry"))) int alt_entry(void)
{
    return 300;
}

// Entry point with parameters
__attribute__((export_name("param_entry"))) int param_entry(int param1, int param2)
{
    return param1 + param2;
}

// Standard main for fallback testing
int main(void)
{
    return 150; // Different from simple_test main
}

// Function to test stack usage
__attribute__((export_name("stack_test"))) int stack_test(void)
{
    volatile int stack_array[100]; // Use some stack space

    // Initialize array to prevent optimization
    for (int i = 0; i < 100; i++) {
        stack_array[i] = i;
    }

    // Return sum to prevent optimization
    int sum = 0;
    for (int i = 0; i < 100; i++) {
        sum += stack_array[i];
    }

    return sum % 1000; // Return manageable value
}