/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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
#include <arch/io.h>
#include <arch/acpi.h>
#include <cbmem.h>
#include "SBPLATFORM.h"

int acpi_get_sleep_type(void)
{
	u16 tmp = inw(PM1_CNT_BLK_ADDRESS);
	tmp = ((tmp & (7 << 10)) >> 10);
	return (int)tmp;
}

void backup_top_of_low_cacheable(uintptr_t ramtop)
{
	u32 dword = ramtop;
	int nvram_pos = 0xf8, i; /* temp */
	for (i = 0; i < 4; i++) {
		outb(nvram_pos, BIOSRAM_INDEX);
		outb((dword >> (8 * i)) & 0xff, BIOSRAM_DATA);
		nvram_pos++;
	}
}

uintptr_t restore_top_of_low_cacheable(void)
{
	u32 xdata = 0;
	int xnvram_pos = 0xf8, xi;
	for (xi = 0; xi < 4; xi++) {
		outb(xnvram_pos, BIOSRAM_INDEX);
		xdata &= ~(0xff << (xi * 8));
		xdata |= inb(BIOSRAM_DATA) << (xi *8);
		xnvram_pos++;
	}
	return xdata;
}
