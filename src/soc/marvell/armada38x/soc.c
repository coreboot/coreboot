/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <symbols.h>

#define RESERVED_SIZE_KB (1 * KiB)

static void soc_enable(device_t dev)
{
	/* Reserve bottom 1M bytes for MMU/TTB */
	reserved_ram_resource(dev, 0, ((uintptr_t)_dram / KiB +
	    (CONFIG_DRAM_SIZE_MB * KiB - RESERVED_SIZE_KB)),
	    RESERVED_SIZE_KB);
	ram_resource(dev, 0, (uintptr_t)_dram / KiB,
		     (CONFIG_DRAM_SIZE_MB * KiB) - RESERVED_SIZE_KB);
}

static void soc_init(device_t dev)
{
	printk(BIOS_INFO, "CPU: Armada 38X\n");
}

static struct device_operations soc_ops = {
	.read_resources = DEVICE_NOOP,
	.set_resources = DEVICE_NOOP,
	.enable_resources = soc_enable,
	.init = soc_init,
	.scan_bus = 0,
};

static void enable_armada38x_dev(device_t dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_marvell_armada38x_ops = {
	CHIP_NAME("SOC Marvell Armada 38x")
	.enable_dev = enable_armada38x_dev,
};
