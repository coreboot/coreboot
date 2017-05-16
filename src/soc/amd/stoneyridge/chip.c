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
#include <cpu/amd/mtrr.h>
#include <cpu/cpu.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/hudson.h>
#include <soc/northbridge.h>

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(dev->link_list);
}

struct device_operations cpu_bus_ops = {
	.read_resources	  = DEVICE_NOOP,
	.set_resources	  = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init		  = &cpu_bus_init,
	.scan_bus	  = cpu_bus_scan,
};

struct device_operations pci_domain_ops = {
	.read_resources	  = domain_read_resources,
	.set_resources	  = domain_set_resources,
	.enable_resources = domain_enable_resources,
	.init		  = NULL,
	.scan_bus	  = pci_domain_scan_bus,
	.ops_pci_bus	  = pci_bus_default_ops,
};

static void enable_dev(device_t dev)
{
	static int done = 0;

	if (!done) {
		setup_bsp_ramtop();
		done = 1;
	}

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
	fam15_finalize(chip_info);
}

struct chip_operations soc_amd_stoneyridge_ops = {
	CHIP_NAME("AMD StoneyRidge SOC")
	.enable_dev = &enable_dev,
	.init = &soc_init,
	.final = &soc_final
};
