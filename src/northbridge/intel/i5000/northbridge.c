/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
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
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <arch/acpi.h>
#include <cbmem.h>

static void intel_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static void mc_read_resources(device_t dev)
{
	struct resource *resource;
	uint32_t hecbase, amsize, tolm;
	uint64_t ambase, memsize;
	int idx = 0;

	device_t dev16_0 = dev_find_slot(0, PCI_DEVFN(16, 0));
	device_t dev16_1 = dev_find_slot(0, PCI_DEVFN(16, 1));

	pci_dev_read_resources(dev);

	tolm = pci_read_config16(dev_find_slot(0, PCI_DEVFN(16, 1)), 0x6c) << 16;
	hecbase = pci_read_config16(dev16_0, 0x64) >> 12;
	hecbase &= 0xffff;

	ambase = ((u64)pci_read_config32(dev16_0, 0x48) |
		  (u64)pci_read_config32(dev16_0, 0x4c) << 32);

	amsize = pci_read_config32(dev16_0, 0x50);
	ambase &= 0x000000ffffff0000;

	printk(BIOS_DEBUG, "TOLM: 0x%08x AMBASE: 0x%016llx\n", tolm, ambase);

	/* Report the memory regions */
	ram_resource(dev, idx++, 0, 640);
	ram_resource(dev, idx++, 768, ((tolm >> 10) - 768));

	memsize = MAX(pci_read_config16(dev16_1, 0x80) & ~3,
		      pci_read_config16(dev16_1, 0x84) & ~3);
	memsize = MAX(memsize, pci_read_config16(dev16_1, 0x88) & ~3);

	memsize <<= 24;
	printk(BIOS_INFO, "MEMSIZE: %08llx\n", memsize);
	if (memsize > 0xd0000000) {
		memsize -= 0xd0000000;
		printk(BIOS_INFO, "high memory: %lldMB\n", memsize / 1048576);
		ram_resource(dev, idx++, 4096 * 1024, memsize / 1024);
	}

	if (hecbase) {
		printk(BIOS_DEBUG, "Adding PCIe config bar at 0x%016llx\n", (u64)hecbase << 28);
		resource = new_resource(dev, idx++);
		resource->base = (resource_t)(uint64_t)hecbase << 28;
		resource->size = (resource_t)256 * 1024 * 1024;
		resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
		    IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
	}

	resource = new_resource(dev, idx++);
	resource->base = (resource_t)(uint64_t)0xffe00000;
	resource->size = (resource_t)0x200000;
	resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
		IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	if (ambase && amsize) {
		resource = new_resource(dev, idx++);
		resource->base = (resource_t)ambase;
		resource->size = (resource_t)amsize;
		resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
			IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
	}

	/* add resource for 0xfe6xxxxx range. This range is used by i5000 for
	   various fixed address registers (BOFL, SPAD, SPADS */
	resource = new_resource(dev, idx++);
	resource->base = (resource_t)0xfe600000;
	resource->size = (resource_t)0x00100000;
	resource->flags = IORESOURCE_MEM | IORESOURCE_RESERVE |
	    IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;

	set_top_of_ram(tolm);
}

static struct pci_operations intel_pci_ops = {
	.set_subsystem    = intel_set_subsystem,
};

static struct device_operations mc_ops = {
	.read_resources   = mc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.scan_bus         = 0,
	.ops_pci          = &intel_pci_ops,
};

static const unsigned short nb_ids[] = {
	0x25c0,	/* 5000X */
	0x25d0, /* 5000Z */
	0x25d4, /* 5000V */
	0x25d8, /* 5000P */
	0};

static const struct pci_driver mc_driver __pci_driver = {
	.ops    = &mc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = nb_ids,
};

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(dev->link_list);
}

static void cpu_bus_noop(device_t dev)
{
}
static struct device_operations cpu_bus_ops = {
	.read_resources   = cpu_bus_noop,
	.set_resources    = cpu_bus_noop,
	.enable_resources = cpu_bus_noop,
	.init             = cpu_bus_init,
	.scan_bus         = 0,
};
static void pci_domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources   = pci_domain_read_resources,
	.set_resources    = pci_domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = pci_domain_scan_bus,
	.ops_pci_bus	  = pci_bus_default_ops,
};

static void enable_dev(device_t dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_intel_i5000_ops = {
	CHIP_NAME("Intel i5000 Northbridge")
	.enable_dev = enable_dev,
};
