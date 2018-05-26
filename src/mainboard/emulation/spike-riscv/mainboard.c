/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google, Inc.
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

#include <console/console.h>
#include <device/device.h>
#include <cbmem.h>

static void mainboard_enable(device_t dev)
{
	/*
	 * Size of the emulated system RAM. On hardware, this would be external
	 * DDR memory.
	 *
	 * TODO: Get this size from the hardware-supplied configuration string.
	 */
	const size_t ram_size = 1*GiB;

	if (!dev) {
		die("No dev0; die\n");
	}

	ram_resource(dev, 0, 0x80000000/KiB, ram_size/KiB);

	cbmem_recovery(0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
