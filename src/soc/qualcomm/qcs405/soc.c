/*
 * This file is part of the coreboot project.
 *
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

#include <symbols.h>
#include <device/device.h>
#include <soc/mmu.h>
#include <soc/symbols.h>

static void soc_read_resources(struct device *dev)
{
	ram_resource(dev, 0, (uintptr_t)_dram / KiB, (1 * GiB) / KiB);
	reserved_ram_resource(dev, 1, (uintptr_t)_dram_reserved / KiB,
			      REGION_SIZE(dram_reserved) / KiB);
}

static void soc_init(struct device *dev)
{

}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.init = soc_init,
};

static void enable_soc_dev(struct device *dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_qualcomm_qcs405_ops = {
	CHIP_NAME("SOC Qualcomm QCS405")
	.enable_dev = enable_soc_dev,
};
