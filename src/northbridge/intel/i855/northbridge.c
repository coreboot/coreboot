/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Ronald G. Minnich
 * Copyright (C) 2003-2004 Eric W. Biederman
 * Copyright (C) 2006 Jon Dufresne <jon.dufresne@gmail.com>
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
#include <stdlib.h>
#include <string.h>
#include <cbmem.h>
#include <cpu/x86/cache.h>
#include <cpu/cpu.h>

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
	.device = 0x3580,
};

static void pci_domain_set_resources(device_t dev)
{
	device_t mc_dev;
	uint32_t pci_tolm;

	printk(BIOS_DEBUG, "Entered with dev vid = %x\n", dev->vendor);
	printk(BIOS_DEBUG, "Entered with dev did = %x\n", dev->device);

	pci_tolm = find_pci_tolm(dev->link_list);
	mc_dev = dev->link_list->children->sibling;
	printk(BIOS_DEBUG, "MC dev vendor = %x\n", mc_dev->vendor);
	printk(BIOS_DEBUG, "MC dev device = %x\n", mc_dev->device);

	if (mc_dev) {
		/* Figure out which areas are/should be occupied by RAM.
		 * This is all computed in kilobytes and converted to/from
		 * the memory controller right at the edges.
		 * Having different variables in different units is
		 * too confusing to get right.  Kilobytes are good up to
		 * 4 Terabytes of RAM...
		 */
		unsigned long tomk, tolmk;
		int idx;

		/* Get the value of the highest DRB. This tells the end of
		 * the physical memory.  The units are ticks of 32MB
		 * i.e. 1 means 32MB.
		 */
		tomk = (unsigned long)pci_read_config8(mc_dev, 0x43);
		tomk = tomk * 32 * 1024;
		/* add vga_mem detection */
		tomk = tomk - 16 * 1024;
		/* Compute the top of Low memory */
		tolmk = pci_tolm >> 10;
		if (tolmk >= tomk) {
			/* The PCI hole does not overlap memory
			 */
			tolmk = tomk;
		}
		/* Write the RAM configuration registers,
		 * preserving the reserved bits.
		 */

		/* Report the memory regions */
		printk(BIOS_DEBUG, "tomk = %ld\n", tomk);
		printk(BIOS_DEBUG, "tolmk = %ld\n", tolmk);

		idx = 10;
		/* avoid pam region */
		ram_resource(dev, idx++, 0, 640);
		/* ram_resource(dev, idx++, 1024, tolmk - 1024); */
		ram_resource(dev, idx++, 768, tolmk - 768);

		set_late_cbmem_top(tomk * 1024);
	}
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources   = pci_domain_read_resources,
	.set_resources    = pci_domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = pci_domain_scan_bus,
	.ops_pci_bus      = pci_bus_default_ops,
};

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(dev->link_list);
}

static struct device_operations cpu_bus_ops = {
	.read_resources   = DEVICE_NOOP,
	.set_resources    = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init             = cpu_bus_init,
	.scan_bus         = 0,
};

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	}
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_intel_i855_ops = {
	CHIP_NAME("Intel 855 Northbridge")
	.enable_dev = enable_dev,
};
