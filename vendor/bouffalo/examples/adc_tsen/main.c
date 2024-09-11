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

void board_init(void) { }

void board_adc_gpio_init()
{
    struct bflb_device_s* gpio;

    gpio = bflb_device_get_by_name("gpio");
    /* ADC_CH0 */
    bflb_gpio_init(gpio, GPIO_PIN_20, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC_CH1 */
    bflb_gpio_init(gpio, GPIO_PIN_19, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC_CH2 */
    bflb_gpio_init(gpio, GPIO_PIN_2, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC_CH3 */
    bflb_gpio_init(gpio, GPIO_PIN_3, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC_CH4 */
    bflb_gpio_init(gpio, GPIO_PIN_14, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC_CH5 */
    bflb_gpio_init(gpio, GPIO_PIN_13, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC_CH6 */
    bflb_gpio_init(gpio, GPIO_PIN_12, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC_CH7 */
    bflb_gpio_init(gpio, GPIO_PIN_10, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC_CH8 */
    bflb_gpio_init(gpio, GPIO_PIN_1, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC_CH9 */
    bflb_gpio_init(gpio, GPIO_PIN_0, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC_CH10 */
    bflb_gpio_init(gpio, GPIO_PIN_27, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
    /* ADC_CH11 */
    bflb_gpio_init(gpio, GPIO_PIN_28, GPIO_ANALOG | GPIO_SMT_EN | GPIO_DRV_0);
}

#include "../../bouffalo_sdk/examples/peripherals/adc/adc_tsen/main.c"