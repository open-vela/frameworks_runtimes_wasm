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

struct bflb_device_s* gpio;
static int i = 0;

void gpio_isr(int irq, void* arg)
{
    bool intstatus = bflb_gpio_get_intstatus(gpio, GPIO_PIN_28);
    if (intstatus) {
        bflb_gpio_int_clear(gpio, GPIO_PIN_28);
    }
}

int main(void)
{
    gpio = bflb_device_get_by_name("gpio");
    printf("gpio interrupt\r\n");

    bflb_gpio_int_init(gpio, GPIO_PIN_28, GPIO_INT_TRIG_MODE_SYNC_HIGH_LEVEL);
    bflb_gpio_int_mask(gpio, GPIO_PIN_28, false);

    bflb_irq_attach(gpio->irq_num, gpio_isr, gpio);
    bflb_irq_enable(gpio->irq_num);

    while (1) {
        printf("GPIO Interrupt Count: %d\r\n", i);
        bflb_mtimer_delay_ms(2000);
    }
}
