/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Patrick Rudolph <patrick.rudolph@9elements.com>
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

#include <stdint.h>

u16 lpc_get_pmbase(void);

void write_pmbase32(const u8 addr, const u32 val);
void write_pmbase16(const u8 addr, const u16 val);
void write_pmbase8(const u8 addr, const u8 val);

u32 read_pmbase32(const u8 addr);
u16 read_pmbase16(const u8 addr);
u8 read_pmbase8(const u8 addr);
