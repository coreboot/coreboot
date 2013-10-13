/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Stefan Reinauer <stefan.reinauer@coreboot.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <cbmem.h>

#define CMOS_ADDR_PORT 0x70
#define CMOS_DATA_PORT 0x71

#define HIGH_RAM_ADDR 0x35
#define LOW_RAM_ADDR 0x34

#define HIGH_HIGHRAM_ADDR 0x5d
#define MID_HIGHRAM_ADDR 0x5c
#define LOW_HIGHRAM_ADDR 0x5b

static unsigned long qemu_get_memory_size(void)
{
	unsigned long tomk;
	outb (HIGH_RAM_ADDR, CMOS_ADDR_PORT);
	tomk = ((unsigned long) inb(CMOS_DATA_PORT)) << 14;
	outb (LOW_RAM_ADDR, CMOS_ADDR_PORT);
	tomk |= ((unsigned long) inb(CMOS_DATA_PORT)) << 6;
	tomk += 16 * 1024;
	return tomk;
}

unsigned long get_top_of_ram(void)
{
	return qemu_get_memory_size() * 1024;
}
