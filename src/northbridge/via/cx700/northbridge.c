/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/hypertransport.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cbmem.h>
#include <arch/acpi.h>

static void pci_domain_set_resources(device_t dev)
{
	device_t mc_dev;
	u32 pci_tolm;
	unsigned char reg;
	unsigned long tomk, tolmk;
	unsigned char rambits;
	int idx;

	pci_tolm = find_pci_tolm(dev->link_list);
	mc_dev = dev_find_device(PCI_VENDOR_ID_VIA, 0x3324, 0);

	rambits = pci_read_config8(mc_dev, 0x88);
	rambits >>= 2;

	/* Get memory size and frame buffer from northbridge's registers.
	 *
	 * If register contains an invalid value we set frame buffer size to a
	 * default of 32M, but that probably won't happen.
	 */
	reg = pci_read_config8(mc_dev, 0xa1);
	reg &= 0x70;
	reg = reg >> 4;

	/* TOP 1M SMM Memory */
	if (reg == 0x0 || reg == 0x6 || reg == 0x7)
		tomk = (((rambits << 6) - 32 - 1) * 1024);	// Set frame buffer 32M for default
	else
		tomk = (((rambits << 6) - (4 << reg) - 1) * 1024);

	/* Compute the top of Low memory */
	tolmk = pci_tolm >> 10;
	if (tolmk >= tomk) {
		/* The PCI hole does does not overlap the memory. */
		tolmk = tomk;
		tolmk -= 1024;	// TOP 1M SM Memory
	}

	set_late_cbmem_top(tolmk * 1024);

	/* Report the memory regions */
	idx = 10;

	/* TODO: Hole needed? Should this go elsewhere? */
	ram_resource(dev, idx++, 0, 640);	/* first 640k */
	ram_resource(dev, idx++, 768, (tolmk - 768));	/* leave a hole for vga */
	assign_resources(dev->link_list);
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	device_t dev;
	u64 mmcfg;

	dev = dev_find_device(0x1106, 0x324b, 0);	// 0:0x13.0
	if (!dev)
		return current;

	// MMCFG not supported or not enabled.
	if ((pci_read_config8(dev, 0x40) & 0xC0) != 0xC0)
		return current;

	mmcfg = ((u64) pci_read_config8(dev, 0x41)) << 28;
	if (!mmcfg)
		return current;

	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *) current, mmcfg, 0x0, 0x0, 0xff);

	return current;
}

static struct device_operations pci_domain_ops = {
	.read_resources	  = pci_domain_read_resources,
	.set_resources	  = pci_domain_set_resources,
	.enable_resources = NULL,
	.init		  = NULL,
	.scan_bus	  = pci_domain_scan_bus,
	.write_acpi_tables = acpi_write_hpet,
};

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(dev->link_list);
}

static struct device_operations cpu_bus_ops = {
	.read_resources	  = DEVICE_NOOP,
	.set_resources	  = DEVICE_NOOP,
	.enable_resources = DEVICE_NOOP,
	.init		  = cpu_bus_init,
	.scan_bus	  = 0,
};

static void enable_dev(struct device *dev)
{
	/* Our wonderful device model */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_via_cx700_ops = {
	CHIP_NAME("VIA CX700 Northbridge")
	.enable_dev = enable_dev
};
