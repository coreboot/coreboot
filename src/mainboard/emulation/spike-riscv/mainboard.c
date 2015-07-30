/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
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
		printk(BIOS_EMERG, "No dev0; die\n");
		while (1);
	}

	ram_resource(dev, 0, 2048, 32768);
	cbmem_recovery(0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
