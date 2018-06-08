/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 The Chromium OS Authors.
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
#include <symbols.h>

static void soc_read_resources(struct device *dev)
{
	ram_resource(dev, 0, (uintptr_t)_dram / KiB,
		     (CONFIG_DRAM_SIZE_MB * MiB) / KiB);
}

static void soc_init(struct device *dev)
{
	printk(BIOS_INFO, "CPU: Imgtec Pistachio\n");
}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.init		= soc_init,
};

static void enable_soc_dev(struct device *dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_imgtec_pistachio_ops = {
	CHIP_NAME("SOC: Imgtec Pistachio")
	.enable_dev = enable_soc_dev,
};
