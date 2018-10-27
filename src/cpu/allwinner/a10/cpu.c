/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Ramstage initialization for Allwinner CPUs
 *
 */

#include <device/device.h>
#include <symbols.h>

static void cpu_enable_resources(struct device *dev)
{
	ram_resource(dev, 0, (uintptr_t)_dram/KiB,
		     CONFIG_DRAM_SIZE_MB << 10);
	/* TODO: Declare CBFS cache as reserved? There's no guarantee we won't
	 * overwrite it. It seems to stay intact, being so high in RAM
	 */
}

static void cpu_init(struct device *dev)
{
	/* TODO: Check if anything else needs to be explicitly initialized */
}

static struct device_operations cpu_ops = {
	.read_resources   = DEVICE_NOOP,
	.set_resources    = DEVICE_NOOP,
	.enable_resources = cpu_enable_resources,
	.init             = cpu_init,
	.scan_bus         = NULL,
};

static void a1x_cpu_enable_dev(struct device *dev)
{
	dev->ops = &cpu_ops;
}

struct chip_operations cpu_allwinner_a10_ops = {
	CHIP_NAME("CPU Allwinner A10")
	.enable_dev = a1x_cpu_enable_dev,
};
