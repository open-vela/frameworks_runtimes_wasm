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

#include "bflb_irq.h"

/* Offset 16 is only applicable to specific product and rel-3.6 branches */
#define IRQ_OFFSET 16

int bflb_irq_attach(int irq, irq_callback isr, void* arg)
{
    return irq_attach(irq + IRQ_OFFSET, (xcpt_t)isr, arg);
}

void bflb_irq_enable(int irq)
{
    irq_enable(irq + IRQ_OFFSET);
}

void bflb_irq_disable(int irq)
{
    irq_disable(irq + IRQ_OFFSET);
}
