/*
 * Copyright (C) 2024 Xiaomi Corporation
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
#include <stdio.h>

struct bflb_device_s* gpio;

/* Sleep function by busy-wait */
void delay(volatile uint32_t count)
{
    while (count--) {
    }
}

int main(void)
{
    gpio = bflb_device_get_by_name("gpio");
    printf("gpio output\r\n");

    bflb_gpio_init(gpio, GPIO_PIN_1,
        GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);
    bflb_gpio_init(gpio, GPIO_PIN_3,
        GPIO_OUTPUT | GPIO_PULLUP | GPIO_SMT_EN | GPIO_DRV_0);

    while (1) {
        printf("led on\r\n");
        bflb_gpio_set(gpio, GPIO_PIN_1);
        bflb_gpio_set(gpio, GPIO_PIN_3);
        delay(5000000);
        printf("led off\r\n");
        bflb_gpio_reset(gpio, GPIO_PIN_1);
        bflb_gpio_reset(gpio, GPIO_PIN_3);
        delay(5000000);
    }
}
