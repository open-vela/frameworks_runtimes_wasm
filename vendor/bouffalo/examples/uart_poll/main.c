#include "bflb_gpio.h"
#include "bflb_mtimer.h"
#include "bflb_uart.h"

struct bflb_device_s* uartx;

void board_uartx_gpio_init()
{
    struct bflb_device_s* gpio;

    gpio = bflb_device_get_by_name("gpio");

    bflb_gpio_uart_init(gpio, GPIO_PIN_27, GPIO_UART_FUNC_UART1_TX);
    bflb_gpio_uart_init(gpio, GPIO_PIN_28, GPIO_UART_FUNC_UART1_RX);
}

void uartx_puts(struct bflb_device_s* dev, char* str)
{
    while (*str != '\0') {
        bflb_uart_putchar(dev, *str++);
    }
}

int main(void)
{
    board_uartx_gpio_init();

    uartx = bflb_device_get_by_name("uart1");

    struct bflb_uart_config_s cfg;

    cfg.baudrate = 2000000;
    cfg.data_bits = UART_DATA_BITS_8;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.parity = UART_PARITY_NONE;
    cfg.flow_ctrl = 0;
    cfg.tx_fifo_threshold = 7;
    cfg.rx_fifo_threshold = 7;
    bflb_uart_init(uartx, &cfg);

    /* Print "Hello from Wasm" by UART */
    uartx_puts(uartx, "Hello from Wasm\n");

    int ch;
    while (1) {
        ch = bflb_uart_getchar(uartx);
        if (ch != -1) {
            bflb_uart_putchar(uartx, ch);
        }
    }
}
