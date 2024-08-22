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

#include <stdint.h>

uint8_t getreg8(uint32_t addr);
uint16_t getreg16(uint32_t addr);
uint32_t getreg32(uint32_t addr);
void putreg8(uint8_t value, uint32_t addr);
void putreg16(uint16_t value, uint32_t addr);
void putreg32(uint32_t value, uint32_t addr);
