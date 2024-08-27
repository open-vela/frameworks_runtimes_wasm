/*
 * Copyright (C) 2024 Xiaomi Corperation
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

#include "bflb_gpio.h"

/* This macro is required by original BFLB SDK's example code,
 * it defines the UART port name used in the example.
 */

#define DEFAULT_TEST_UART "uart1"

/* Stub function that is needed by original BFLB SDK's example code */

void board_init(void) { }

/* Customized UART initialization function, called by BFLB SDK's example code */

int board_uartx_gpio_init(void)
{
    struct bflb_device_s* gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_uart_init(gpio, GPIO_PIN_27, GPIO_UART_FUNC_UART1_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_28, GPIO_UART_FUNC_UART1_RX);
    return 0;
}

/* Delay function that is defined and used by BFLB SDK's example code */

void bflb_mtimer_delay_ms(uint32_t delay)
{
    /*
     * FIXME: Implement the following functions by clock_sleep/nanosleep
     * Origin implementation is based on mtimer in RISCV platform, which is used
     * by Vela OS, so in the Wasm side, we need to implement the same
     * functionality to achieve the same effect.
     */
}

#include "../../bouffalo_sdk/examples/peripherals/uart/uart_fifo_interrupt/main.c"
