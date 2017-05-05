/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
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
#include <chip.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/hudson.h>

static void pci_domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.set_resources    = &pci_domain_set_resources,
};

static struct device_operations cpu_bus_ops = {
};

static void enable_dev(device_t dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	} else if (dev->path.type == DEVICE_PATH_PCI) {
		hudson_enable(dev);
	}
}

static void soc_init(void *chip_info)
{
	hudson_init(chip_info);
}

static void soc_final(void *chip_info)
{
	hudson_final(chip_info);
}

struct chip_operations soc_amd_stoneyridge_ops = {
	CHIP_NAME("AMD StoneyRidge SOC")
	.enable_dev = &enable_dev,
	.init = &soc_init,
	.final = &soc_final
};

