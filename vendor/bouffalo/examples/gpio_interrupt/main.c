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

#include <vela/irq.h>

struct bflb_device_s* gpio;
static int i = 0;

void gpio_isr(int irq, void* arg)
{
    bool intstatus = bflb_gpio_get_intstatus(gpio, GPIO_PIN_28);
    if (intstatus) {
        bflb_gpio_int_clear(gpio, GPIO_PIN_28);
    }
    printf("gpio_isr callback called, the gpio interrupt was triggered %d times!!\r\n", ++i);
    bflb_irq_enable(gpio->irq_num);
}

int main(void)
{
    int i_time = 0;
    irq_initialize();

    gpio = bflb_device_get_by_name("gpio");
    printf("gpio interrupt, You have 10 seconds to trigger the interrupt\r\n");

    bflb_gpio_int_init(gpio, GPIO_PIN_28, GPIO_INT_TRIG_MODE_SYNC_FALLING_EDGE);
    bflb_gpio_int_mask(gpio, GPIO_PIN_28, false);

    bflb_irq_attach(gpio->irq_num, gpio_isr, gpio);
    bflb_irq_enable(gpio->irq_num);

    while (i_time < 5) {
        bflb_mtimer_delay_ms(2000);
        i_time++;
    }

    printf("Interrupt trigger time ended and then goto irq_uninitialize\r\n");
    bflb_irq_disable(gpio->irq_num);
    irq_uninitialize();

    printf("irq_uninitialize end !!!\r\n");
    return 0;
}
