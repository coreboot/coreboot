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

	if (!dev) {
		die("No dev0; die\n");
	}

	/* Where does RAM live? */
	ram_resource(dev, 0, 2048, 32768);
	cbmem_recovery(0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
