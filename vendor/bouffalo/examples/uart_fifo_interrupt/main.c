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

#include <vela/irq.h>

#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "bflb_uart.h"
#include "board.h"

struct bflb_device_s* uartx;
static int i = 0;
static uint8_t uart_txbuf[128] = { 0 };

/* This macro is required by original BFLB SDK's example code,
 * it defines the UART port name used in the example.
 */

#define DEFAULT_TEST_UART "uart1"

/* Customized UART initialization function, called by BFLB SDK's example code */

int board_uartx_gpio_init(void)
{
    struct bflb_device_s* gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_uart_init(gpio, GPIO_PIN_27, GPIO_UART_FUNC_UART1_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_28, GPIO_UART_FUNC_UART1_RX);
    return 0;
}

void uart_isr(int irq, void* arg)
{
    uint32_t intstatus = bflb_uart_get_intstatus(uartx);

    if (intstatus & UART_INTSTS_RX_FIFO) {
        printf("rx fifo\r\n");
        while (bflb_uart_rxavailable(uartx)) {
            printf("0x%02x\r\n", bflb_uart_getchar(uartx));
        }
        bflb_uart_feature_control(uartx, UART_CMD_SET_RTS_VALUE, 1);
    }
    if (intstatus & UART_INTSTS_RTO) {
        printf("rto\r\n");
        while (bflb_uart_rxavailable(uartx)) {
            printf("0x%02x\r\n", bflb_uart_getchar(uartx));
        }
        bflb_uart_int_clear(uartx, UART_INTCLR_RTO);
    }
    if (intstatus & UART_INTSTS_TX_FIFO) {
        printf("tx fifo\r\n");
        for (uint8_t i = 0; i < 27; i++) {
            bflb_uart_putchar(uartx, uart_txbuf[i]);
        }
        bflb_uart_txint_mask(uartx, true);
    }

    printf("uart_isr callback called, the uart interrupt was triggered %d times!!\r\n", ++i);
    bflb_irq_enable(uartx->irq_num);
}

int main(void)
{
    int i_time = 0;

    printf("Initialize IRQ for WASM environment\r\n");
    irq_initialize();

    board_uartx_gpio_init();

    uartx = bflb_device_get_by_name(DEFAULT_TEST_UART);

    for (uint8_t i = 0; i < 128; i++) {
        uart_txbuf[i] = i;
    }

    printf("uart interrupt, You have 20 seconds to trigger the interrupt\r\n");

    struct bflb_uart_config_s cfg;

    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    bflb_uart_init(uartx, &cfg);

    bflb_uart_txint_mask(uartx, false);
    bflb_uart_rxint_mask(uartx, false);
    bflb_irq_attach(uartx->irq_num, uart_isr, NULL);
    bflb_irq_enable(uartx->irq_num);

    while (i_time < 10) {
        bflb_mtimer_delay_ms(2000);
        i_time++;
    }

    printf("Interrupt trigger time ended and then goto irq_uninitialize\r\n");
    bflb_irq_disable(uartx->irq_num);
    irq_uninitialize();
    printf("irq_uninitialize end !!!\r\n");

    return 0;
}
