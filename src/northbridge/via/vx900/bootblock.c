/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Lubomir Rintel <lkundrak@v3.sk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/pci_ops.h>

#if CONFIG_ROM_SIZE == 0x80000
#  define ROM_DECODE_MAP 0x00
#elif CONFIG_ROM_SIZE == 0x100000
#  define ROM_DECODE_MAP 0x40
#elif CONFIG_ROM_SIZE == 0x180000
#  define ROM_DECODE_MAP 0x60
#elif CONFIG_ROM_SIZE == 0x200000
#  define ROM_DECODE_MAP 0x70
#elif CONFIG_ROM_SIZE == 0x280000
#  define ROM_DECODE_MAP 0x78
#elif CONFIG_ROM_SIZE == 0x300000
#  define ROM_DECODE_MAP 0x7c
#elif CONFIG_ROM_SIZE == 0x380000
#  define ROM_DECODE_MAP 0x7e
#elif CONFIG_ROM_SIZE == 0x400000
#  define ROM_DECODE_MAP 0x7f
#else
#  error "Bad CONFIG_ROM_SIZE"
#endif

static void bootblock_northbridge_init(void)
{
	u8 reg;

	pci_io_read_config8(PCI_DEV(0, 0x11, 0), 0x41);
	reg |= ROM_DECODE_MAP;
	pci_io_write_config8(PCI_DEV(0, 0x11, 0), 0x41, reg);
}
