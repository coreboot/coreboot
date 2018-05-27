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

static void pci_domain_set_resources(struct device *dev)
{
	struct device *mc_dev;
	uint32_t pci_tolm;

	pci_tolm = find_pci_tolm(dev->link_list);
	mc_dev = dev->link_list->children;
	if (mc_dev) {
		/* Figure out which areas are/should be occupied by RAM.
		 * This is all computed in kilobytes and converted to/from
		 * the memory controller right at the edges.
		 * Having different variables in different units is
		 * too confusing to get right.  Kilobytes are good up to
		 * 4 Terabytes of RAM...
		 */
		uint16_t tolm_r, remapbase_r, remaplimit_r;
		unsigned long tomk, tolmk;
		unsigned long remapbasek, remaplimitk;
		int idx;

		/* Get the value of the highest DRB. This tells the end of
		 * the physical memory.  The units are ticks of 64MB
		 * i.e. 1 means 64MB.
		 */
		tomk = ((unsigned long)pci_read_config8(mc_dev, DRB_ROW_7)) << 16;
		/* Compute the top of Low memory */
		tolmk = pci_tolm >> 10;
		if (tolmk >= tomk) {
			/* The PCI hole does not overlap memory
			 * we won't use the remap window.
			 */
			tolmk = tomk;
			remapbasek   = 0x3ff << 16;
			remaplimitk  = 0 << 16;
		}
		else {
			/* The PCI memory hole overlaps memory
			 * setup the remap window.
			 */
			/* Find the bottom of the remap window
			 * is it above 4G?
			 */
			remapbasek = 4*1024*1024;
			if (tomk > remapbasek) {
				remapbasek = tomk;
			}
			/* Find the limit of the remap window */
			remaplimitk = (remapbasek + (4*1024*1024 - tolmk) - (1 << 16));
		}
		/* Write the RAM configuration registers,
		 * preserving the reserved bits.
		 */
		tolm_r = pci_read_config16(mc_dev, TOLM);
		tolm_r = ((tolmk >> 17) << 11) | (tolm_r & 0x7ff);
		pci_write_config16(mc_dev, TOLM, tolm_r);

		remapbase_r = pci_read_config16(mc_dev, REMAPBASE);
		remapbase_r = (remapbasek >> 16) | (remapbase_r & 0xfc00);
		pci_write_config16(mc_dev, REMAPBASE, remapbase_r);

		remaplimit_r = pci_read_config16(mc_dev, REMAPLIMIT);
		remaplimit_r = (remaplimitk >> 16) | (remaplimit_r & 0xfc00);
		pci_write_config16(mc_dev, REMAPLIMIT, remaplimit_r);

		/* Report the memory regions */
		idx = 10;
		ram_resource(dev, idx++, 0, 640);
		ram_resource(dev, idx++, 768, tolmk - 768);
		if (tomk > 4*1024*1024) {
			ram_resource(dev, idx++, 4096*1024, tomk - 4*1024*1024);
		}
		if (remaplimitk >= remapbasek) {
			ram_resource(dev, idx++, remapbasek,
				(remaplimitk + 64*1024) - remapbasek);
		}

		set_late_cbmem_top(tolmk * 1024);
	}
	assign_resources(dev->link_list);
}

static void intel_set_subsystem(struct device *dev,
				unsigned vendor, unsigned device)
{
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations intel_pci_ops = {
	.set_subsystem = intel_set_subsystem,
};

static struct device_operations pci_domain_ops = {
	.read_resources   = pci_domain_read_resources,
	.set_resources    = pci_domain_set_resources,
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
