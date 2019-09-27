/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
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

#include <console/console.h>
#include <device/pci_ops.h>

#include "vx900.h"

void dump_pci_device(pci_devfn_t dev)
{
	int i;
	for (i = 0; i <= 0xff; i++) {
		unsigned char val;
		if ((i & 0x0f) == 0)
			printk(BIOS_DEBUG, "%.2x:", i);

		if ((i & 0x0f) == 0x08)
			printk(BIOS_DEBUG, " |");

		val = pci_s_read_config8(dev, i);
		printk(BIOS_DEBUG, " %.2x", val);

		if ((i & 0x0f) == 0x0f)
			printk(BIOS_DEBUG, "\n");
	}
}
