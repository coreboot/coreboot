/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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

#include <arch/cache.h>
#include <bootmode.h>
#include <cbmem.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <gpio.h>
#include <soc/display.h>
#include <soc/soc.h>
#include <soc/sdram.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <symbols.h>

#include "chip.h"

static void soc_init(struct device *dev)
{
	ram_resource(dev, 0, (uintptr_t)_dram/KiB, sdram_size_mb()*(MiB/KiB));
	if (display_init_required())
		rk_display_init(dev, (uintptr_t)_framebuffer,
				_framebuffer_size);
	else
		printk(BIOS_INFO, "Skipping display init.\n");
}

static struct device_operations soc_ops = {
	.read_resources   = DEVICE_NOOP,
	.set_resources    = DEVICE_NOOP,
	.init             = soc_init,
	.scan_bus         = 0,
};

static void enable_rk3288_dev(struct device *dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_rockchip_rk3288_ops = {
	CHIP_NAME("SOC Rockchip 3288")
	.enable_dev = enable_rk3288_dev,
};
