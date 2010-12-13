/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Corey Osgood <corey.osgood@gmail.com>
 * Copyright (C) 2010 Joseph Smith <joe@settoplinux.org>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include <cpu/cpu.h>
#include "chip.h"
#include <boot/tables.h>
#include "northbridge.h"
#include "i82810.h"

static void northbridge_init(device_t dev)
{
	printk(BIOS_SPEW, "Northbridge init\n");
}

static struct device_operations northbridge_operations = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= northbridge_init,
	.enable			= 0,
	.ops_pci		= 0,
};

/* Intel 82810/82810-DC100 */
static const struct pci_driver i810_northbridge_driver __pci_driver = {
	.ops	= &northbridge_operations,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x7120,
};

/* Intel 82810E */
static const struct pci_driver i810e_northbridge_driver __pci_driver = {
	.ops    = &northbridge_operations,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = 0x7124,
};

/* IGD UMA memory */
uint64_t uma_memory_base=0, uma_memory_size=0;

int add_northbridge_resources(struct lb_memory *mem)
{
	printk(BIOS_DEBUG, "Adding IGD UMA memory area\n");
	lb_add_memory_range(mem, LB_MEM_RESERVED,
		uma_memory_base, uma_memory_size);

	return 0;
}

/* Table which returns the RAM size in MB when fed the DRP[7:4] or [3:0] value.
 * Note that 2 is a value which the DRP should never be programmed to.
 * Some size values appear twice, due to single-sided vs dual-sided banks.
 */
static int translate_i82810_to_mb[] = {
/* DRP	0  1 (2) 3   4   5   6   7   8   9   A   B   C    D    E    F */
/* MB */0, 8, 0, 16, 16, 24, 32, 32, 48, 64, 64, 96, 128, 128, 192, 256,
};

#if CONFIG_WRITE_HIGH_TABLES==1
#include <cbmem.h>
#endif

static void pci_domain_set_resources(device_t dev)
{
	device_t mc_dev;
	uint32_t pci_tolm;
	int igd_memory = 0;

	pci_tolm = find_pci_tolm(dev->link_list);
	mc_dev = dev->link_list->children;
	if (!mc_dev)
		return;

	unsigned long tomk, tolmk;
	int idx, drp_value;
	u8 reg8;

	reg8 = pci_read_config8(mc_dev, SMRAM);
	reg8 &= 0xc0;

	switch (reg8) {
		case 0xc0:
			igd_memory = 1024;
			printk(BIOS_DEBUG, "%dKB IGD UMA\n", igd_memory);
			break;
		case 0x80:
			igd_memory = 512;
			printk(BIOS_DEBUG, "%dKB IGD UMA\n", igd_memory);
			break;
		default:
			igd_memory = 0;
			printk(BIOS_DEBUG, "No IGD UMA Memory\n");
			break;
	}

	/* Get the value for DIMM 0 and translate it to MB. */
	drp_value = pci_read_config8(mc_dev, DRP);
	tomk = (unsigned long)(translate_i82810_to_mb[drp_value & 0x0f]);
	/* Get the value for DIMM 1 and translate it to MB. */
	drp_value = drp_value >> 4;
	tomk += (unsigned long)(translate_i82810_to_mb[drp_value]);
	/* Convert tomk from MB to KB. */
	tomk = tomk << 10;
	tomk -= igd_memory;

	/* For reserving UMA memory in the memory map */
	uma_memory_base = tomk * 1024ULL;
	uma_memory_size = igd_memory * 1024ULL;
	printk(BIOS_DEBUG, "Available memory: %ldKB\n", tomk);

	/* Compute the top of low memory. */
	tolmk = pci_tolm >> 10;
	if (tolmk >= tomk) {
		/* The PCI hole does does not overlap the memory. */
		tolmk = tomk;
	}

	/* Report the memory regions. */
	idx = 10;
	ram_resource(dev, idx++, 0, 640);
	ram_resource(dev, idx++, 768, tolmk - 768);

#if CONFIG_WRITE_HIGH_TABLES==1
	/* Leave some space for ACPI, PIRQ and MP tables */
	high_tables_base = (tomk * 1024) - HIGH_MEMORY_SIZE;
	high_tables_size = HIGH_MEMORY_SIZE;
#endif
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources		= pci_domain_read_resources,
	.set_resources		= pci_domain_set_resources,
	.enable_resources	= NULL,
	.init			= NULL,
	.scan_bus		= pci_domain_scan_bus,
};

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(dev->link_list);
}

static void cpu_bus_noop(device_t dev)
{
}

static struct device_operations cpu_bus_ops = {
	.read_resources		= cpu_bus_noop,
	.set_resources		= cpu_bus_noop,
	.enable_resources	= cpu_bus_noop,
	.init			= cpu_bus_init,
	.scan_bus		= 0,
};

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
		dev->ops = &pci_domain_ops;
		pci_set_method(dev);
	} else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_intel_i82810_ops = {
	CHIP_NAME("Intel 82810 Northbridge")
	.enable_dev = enable_dev,
};
