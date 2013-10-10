/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <soc/nvidia/tegra/dc.h>
#include <soc/addressmap.h>

/* this sucks, but for now, fb size/location are hardcoded.
 * Will break if we get 2. Sigh.
 * We assume it's all multiples of MiB for MMUs sake.
 */
static void soc_enable(device_t dev)
{
	unsigned long fb_size = FB_SIZE_MB;
	u32 lcdbase = FB_BASE_MB;
	ram_resource(dev, 0, CONFIG_SYS_SDRAM_BASE/KiB,
		(CONFIG_DRAM_SIZE_MB - fb_size)*KiB);
	mmio_resource(dev, 1, lcdbase*KiB, fb_size*KiB);
}

static void soc_init(device_t dev)
{
	display_startup(dev);
	printk(BIOS_INFO, "CPU: Tegra124\n");
}

static void soc_noop(device_t dev)
{
}

static struct device_operations soc_ops = {
	.read_resources   = soc_noop,
	.set_resources    = soc_noop,
	.enable_resources = soc_enable,
	.init             = soc_init,
	.scan_bus         = 0,
};

static void enable_tegra124_dev(device_t dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_nvidia_tegra124_ops = {
	CHIP_NAME("SOC Nvidia Tegra124")
	.enable_dev = enable_tegra124_dev,
};
