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

#pragma once

typedef int (*xcpt_t)(int irq, void* context, void* arg);

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Name: irq_attach
 *
 * Description:
 *   Configure the IRQ subsystem so that IRQ number 'irq' is dispatched to
 *   'isr' with argument 'arg'
 *
 ****************************************************************************/

int irq_attach(int irq, xcpt_t isr, void* arg);

/****************************************************************************
 * Name: up_enable_irq
 *
 * Description:
 *   Enable the IRQ specified by 'irq'
 *
 ****************************************************************************/

void irq_enable(int irq);

/****************************************************************************
 * Name: irq_disable
 *
 * Description:
 *   This function must be called from the architecture-specific logic in
 *   order to dispatch an interrupt to the appropriate, registered handling
 *   logic.
 *
 ****************************************************************************/

void irq_disable(int irq);

/****************************************************************************
 * Name: irq_initialize
 *
 * Description:
 *   This function is mainly used to create IRQ interrupt threads during
 *   initialization
 *
 ****************************************************************************/

void irq_initialize();

/****************************************************************************
 * Name: irq_uninitialize
 *
 * Description:
 *   This function mainly releases IRQ thread resources
 *
 ****************************************************************************/

void irq_uninitialize();
