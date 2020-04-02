/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stdint.h>

u16 lpc_get_pmbase(void);

void write_pmbase32(const u8 addr, const u32 val);
void write_pmbase16(const u8 addr, const u16 val);
void write_pmbase8(const u8 addr, const u8 val);

u32 read_pmbase32(const u8 addr);
u16 read_pmbase16(const u8 addr);
u8 read_pmbase8(const u8 addr);
