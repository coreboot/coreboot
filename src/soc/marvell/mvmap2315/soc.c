/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <device/device.h>
#include <soc/addressmap.h>
#include <symbols.h>

static void soc_enable(struct device *dev)
{
	ram_resource(dev, 0, 0x0, MAX_DRAM_ADDRESS / KiB);
}

static struct device_operations soc_ops = {
	.enable_resources = soc_enable,
};

static void enable_mvmap2315_dev(struct device *dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_marvell_mvmap2315_ops = {
	CHIP_NAME("SOC Marvell MVMAP2315")
	.enable_dev = enable_mvmap2315_dev,
};
