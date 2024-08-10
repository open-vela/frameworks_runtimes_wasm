#include "bflb_gpio.h"

/* Main function */

int main(void)
{
    struct bflb_device_s* gpio;
    gpio = bflb_device_get_by_name("gpio");

    /**
     * Configure GPIO pin 1 and 3 as output
     * This code does not work with aot_bounds_check method
     */

    *((volatile uint32_t*)0x200008c8) = 0x40400b44;
    *((volatile uint32_t*)0x200008d0) = 0x40400b44;

    /* Set GPIO pin 1 and 3 to low, that disables the LEDs on the board */

    bflb_gpio_reset(gpio, GPIO_PIN_1);
    bflb_gpio_reset(gpio, GPIO_PIN_3);
}
