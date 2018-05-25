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

#include <device/device.h>
#include <soc/cygnus.h>
#include <soc/sdram.h>
#include <stddef.h>
#include <stdlib.h>
#include <symbols.h>

static void soc_init(struct device *dev)
{
	ram_resource(dev, 0, (uintptr_t)_dram/KiB, sdram_size_mb()*(MiB/KiB));
	usb_init();
}

static void soc_noop(struct device *dev)
{
}

static struct device_operations soc_ops = {
	.read_resources   = soc_noop,
	.set_resources    = soc_noop,
	.enable_resources = soc_noop,
	.init             = soc_init,
	.scan_bus         = 0,
};

static void enable_cygnus_dev(struct device *dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_broadcom_cygnus_ops = {
	CHIP_NAME("SOC Broadcom Cygnus")
	.enable_dev = enable_cygnus_dev,
};
