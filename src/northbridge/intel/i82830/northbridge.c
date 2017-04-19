/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2010 Joseph Smith <joe@settoplinux.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cbmem.h>
#include <cpu/cpu.h>
#include <stdlib.h>
#include <string.h>
#include "i82830.h"

static void northbridge_init(device_t dev)
{
	printk(BIOS_SPEW, "Northbridge init\n");
}

static struct device_operations northbridge_operations = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = northbridge_init,
	.enable = 0,
	.ops_pci = 0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops = &northbridge_operations,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x3575,
};

static void pci_domain_set_resources(device_t dev)
{
	device_t mc_dev;
	int igd_memory = 0;
	uint64_t uma_memory_base = 0, uma_memory_size = 0;

	mc_dev = dev->link_list->children;
	if (!mc_dev)
		return;

	unsigned long tomk, tomk_stolen;
	int idx;

	if (CONFIG_VIDEO_MB == 512) {
		igd_memory = (CONFIG_VIDEO_MB);
		printk(BIOS_DEBUG, "%dKB IGD UMA\n", igd_memory >> 10);
	} else {
		igd_memory = (CONFIG_VIDEO_MB * 1024);
		printk(BIOS_DEBUG, "%dMB IGD UMA\n", igd_memory >> 10);
	}

	/* Get the value of the highest DRB. This tells the end of
	 * the physical memory. The units are ticks of 32MB
	 * i.e. 1 means 32MB.
	 */
	tomk = ((unsigned long)pci_read_config8(mc_dev, DRB + 3)) << 15;
	tomk_stolen = tomk - igd_memory;

	/* For reserving UMA memory in the memory map */
	uma_memory_base = tomk_stolen * 1024ULL;
	uma_memory_size = igd_memory * 1024ULL;
	printk(BIOS_DEBUG, "Available memory: %ldKB\n", tomk_stolen);

	/* Report the memory regions. */
	idx = 10;
	ram_resource(dev, idx++, 0, 640);
	ram_resource(dev, idx++, 768, tomk - 768);
	uma_resource(dev, idx++, uma_memory_base >> 10, uma_memory_size >> 10);

	assign_resources(dev->link_list);

	set_late_cbmem_top(tomk_stolen * 1024);
}

static struct device_operations pci_domain_ops = {
	.read_resources		= pci_domain_read_resources,
	.set_resources		= pci_domain_set_resources,
	.enable_resources	= NULL,
	.init			= NULL,
	.scan_bus		= pci_domain_scan_bus,
	.ops_pci_bus	= pci_bus_default_ops,
};

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(dev->link_list);
}

static struct device_operations cpu_bus_ops = {
	.read_resources		= DEVICE_NOOP,
	.set_resources		= DEVICE_NOOP,
	.enable_resources	= DEVICE_NOOP,
	.init			= cpu_bus_init,
	.scan_bus		= 0,
};

static void enable_dev(struct device *dev)
{
	struct device_path;

	/* Set the operations if it is a special bus type. */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_intel_i82830_ops = {
	CHIP_NAME("Intel 82830 Northbridge")
	.enable_dev = enable_dev,
};
