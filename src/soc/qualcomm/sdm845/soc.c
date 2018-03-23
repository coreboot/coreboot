/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018, The Linux Foundation.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <timestamp.h>

static void soc_read_resources(device_t dev)
{

}

static void soc_init(device_t dev)
{

}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.init = soc_init,
};

static void enable_soc_dev(device_t dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_qualcomm_sdm845_ops = {
	CHIP_NAME("SOC Qualcomm SDM845")
	.enable_dev = enable_soc_dev,
};
