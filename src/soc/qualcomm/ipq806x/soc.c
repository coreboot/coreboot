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

static void soc_read_resources(device_t dev)
{
	ram_resource(dev, 0, CONFIG_SYS_SDRAM_BASE/KiB,
		     CONFIG_DRAM_SIZE_MB * (1 << 10));
}

static void soc_init(device_t dev)
{
	printk(BIOS_INFO, "CPU: Qualcomm 8064\n");
}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.init		= soc_init,
};

static void enable_soc_dev(device_t dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_qualcomm_ipq806x_ops = {
	CHIP_NAME("SOC Qualcomm 8064")
	.enable_dev = enable_soc_dev,
};
