/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google, Inc.
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

#include <cbmem.h>
#include <device/device.h>
#include <symbols.h>

static void mainboard_enable(device_t dev)
{
	/*
	 * TODO: Get this size from the hardware-supplied configuration string.
	 */
	const size_t ram_size = 1*GiB;

	ram_resource(dev, 0, (uintptr_t)_dram / KiB, ram_size / KiB);

	cbmem_initialize_empty();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
