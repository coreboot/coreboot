/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Vladimir Serbinenko <phcoder@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 or, at your option, any later
 * version of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <cbmem.h>
#include <string.h>
#include <halt.h>
#include "mainboard.h"

static void mainboard_enable(device_t dev)
{
	int discovered;
	if (!dev) {
		printk(BIOS_EMERG, "No dev0; die\n");
		halt();
	}

	discovered = probe_ramsize();
	printk(BIOS_DEBUG, "%d MiB of RAM discovered\n", discovered);
	ram_resource(dev, 0, 0x60000000 >> 10, discovered << 10);
	cbmem_recovery(0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
