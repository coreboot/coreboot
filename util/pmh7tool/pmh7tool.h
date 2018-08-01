/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Evgeny Zinoviev <me@ch1p.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef PMH7TOOL_H
#define PMH7TOOL_H

#define EC_LENOVO_PMH7_BASE 0x15e0
#define EC_LENOVO_PMH7_ADDR_L (EC_LENOVO_PMH7_BASE + 0x0c)
#define EC_LENOVO_PMH7_ADDR_H (EC_LENOVO_PMH7_BASE + 0x0d)
#define EC_LENOVO_PMH7_DATA (EC_LENOVO_PMH7_BASE + 0x0e)

uint8_t pmh7_register_read(uint16_t reg);
void pmh7_register_write(uint16_t reg, uint8_t val);
void pmh7_register_set_bit(uint16_t reg, uint8_t bit);
void pmh7_register_clear_bit(uint16_t reg, uint8_t bit);

#endif /* PMH7TOOL_H */
