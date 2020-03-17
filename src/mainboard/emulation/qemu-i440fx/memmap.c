/*
 * This file is part of the coreboot project.
 *
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

#include <cbmem.h>
#include <arch/io.h>
#include <arch/romstage.h>
#include "memory.h"
#include "fw_cfg.h"

#define CMOS_ADDR_PORT 0x70
#define CMOS_DATA_PORT 0x71

#define HIGH_RAM_ADDR 0x35
#define LOW_RAM_ADDR 0x34

#define HIGH_HIGHRAM_ADDR 0x5d
#define MID_HIGHRAM_ADDR 0x5c
#define LOW_HIGHRAM_ADDR 0x5b

unsigned long qemu_get_high_memory_size(void)
{
	unsigned long high;
	outb(HIGH_HIGHRAM_ADDR, CMOS_ADDR_PORT);
	high = ((unsigned long) inb(CMOS_DATA_PORT)) << 22;
	outb(MID_HIGHRAM_ADDR, CMOS_ADDR_PORT);
	high |= ((unsigned long) inb(CMOS_DATA_PORT)) << 14;
	outb(LOW_HIGHRAM_ADDR, CMOS_ADDR_PORT);
	high |= ((unsigned long) inb(CMOS_DATA_PORT)) << 6;
	return high;
}

unsigned long qemu_get_memory_size(void)
{
	unsigned long tomk;
	outb(HIGH_RAM_ADDR, CMOS_ADDR_PORT);
	tomk = ((unsigned long) inb(CMOS_DATA_PORT)) << 14;
	outb(LOW_RAM_ADDR, CMOS_ADDR_PORT);
	tomk |= ((unsigned long) inb(CMOS_DATA_PORT)) << 6;
	tomk += 16 * 1024;
	return tomk;
}

void *cbmem_top_chipset(void)
{
	uintptr_t top = 0;

	top = fw_cfg_tolud();
	if (!top)
		top = (uintptr_t)qemu_get_memory_size() * 1024;

	return (void *)top;
}

/* Nothing to do, MTRRs are no-op on QEMU. */
void fill_postcar_frame(struct postcar_frame *pcf)
{
}
