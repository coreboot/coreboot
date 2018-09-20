/*
 * This file is part of the coreboot project.
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
#include <cpu/cpu.h>
#include <stdlib.h>
#include <string.h>
#include "e7505.h"
#include <cbmem.h>
#include <arch/acpi.h>

unsigned long acpi_fill_mcfg(unsigned long current)
{
	/* Just a dummy */
	return current;
}

static void mch_domain_read_resources(struct device *dev)
{
	int idx;
	unsigned long tomk, tolmk;
	unsigned long remapbasek, remaplimitk;
	const unsigned long basek_4G = 4 * (GiB / KiB);
	struct device *mc_dev;

	pci_domain_read_resources(dev);

	mc_dev = dev_find_slot(0, PCI_DEVFN(0x0, 0));
	if (!mc_dev)
		die("Could not find MCH device\n");

	tolmk = pci_read_config16(mc_dev, TOLM) >> 11;
	tolmk <<= 17;

	tomk = pci_read_config8(mc_dev, DRB_ROW_7);
	tomk <<= 16;

	/* Remapped region with a 64 MiB granularity in register
	   definition. Limit is inclusive, so add one. */
	remapbasek = pci_read_config16(mc_dev, REMAPBASE) & 0x3ff;
	remapbasek <<= 16;

	remaplimitk = pci_read_config16(mc_dev, REMAPLIMIT) & 0x3ff;
	remaplimitk += 1;
	remaplimitk <<= 16;

	/* Report the memory regions */
	idx = 10;
	ram_resource(dev, idx++, 0, 640);
	ram_resource(dev, idx++, 768, tolmk - 768);

	if (tomk > basek_4G)
		ram_resource(dev, idx++, basek_4G, tomk - basek_4G);

	if (remaplimitk > remapbasek)
		ram_resource(dev, idx++, remapbasek, remaplimitk - remapbasek);
}

static void mch_domain_set_resources(struct device *dev)
{
	assign_resources(dev->link_list);
}

static void intel_set_subsystem(struct device *dev, unsigned int vendor,
				unsigned int device)
{
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations intel_pci_ops = {
	.set_subsystem = intel_set_subsystem,
};

static struct device_operations pci_domain_ops = {
	.read_resources   = mch_domain_read_resources,
	.set_resources    = mch_domain_set_resources,
	.enable_resources = NULL,
	.init             = NULL,
	.scan_bus         = pci_domain_scan_bus,
	.ops_pci          = &intel_pci_ops,
};

static void cpu_bus_init(struct device *dev)
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

struct chip_operations northbridge_intel_e7505_ops = {
	CHIP_NAME("Intel E7505 Northbridge")
	.enable_dev = enable_dev,
};
