/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Corey Osgood <corey@slightlyhackish.com>
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
#include "northbridge.h"
#include "i82810.h"

static void northbridge_init(device_t dev)
{
	printk_spew("Northbridge init\n");
}

static struct device_operations northbridge_operations = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= northbridge_init,
	.enable			= 0,
	.ops_pci		= 0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops	= &northbridge_operations,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x7120,
};

#define BRIDGE_IO_MASK (IORESOURCE_IO | IORESOURCE_MEM)

static void pci_domain_read_resources(device_t dev)
{
	struct resource *resource;
	unsigned reg;

	/* Initialize the system wide io space constraints */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	resource->base = 0x400;
	resource->limit = 0xffffUL;
	resource->flags =
	    IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	/* Initialize the system wide memory resources constraints */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	resource->limit = 0xffffffffULL;
	resource->flags =
	    IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
}

static void ram_resource(device_t dev, unsigned long index,
			 unsigned long basek, unsigned long sizek)
{
	struct resource *resource;

	if (!sizek) {
		return;
	}
	resource = new_resource(dev, index);
	resource->base = ((resource_t) basek) << 10;
	resource->size = ((resource_t) sizek) << 10;
	resource->flags = IORESOURCE_MEM | IORESOURCE_CACHEABLE |
	    IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}

static void tolm_test(void *gp, struct device *dev, struct resource *new)
{
	struct resource **best_p = gp;
	struct resource *best;
	best = *best_p;
	if (!best || (best->base > new->base)) {
		best = new;
	}
	*best_p = best;
}

static uint32_t find_pci_tolm(struct bus *bus)
{
	struct resource *min;
	uint32_t tolm;
	min = 0;
	search_bus_resources(bus, IORESOURCE_MEM, IORESOURCE_MEM, tolm_test,
			     &min);
	tolm = 0xffffffffUL;
	if (min && tolm > min->base) {
		tolm = min->base;
	}
	return tolm;
}

/* Table which returns the RAM size in MB when fed the DRP[7:4] or [3:0] value.
 * Note that 2 is a value which the DRP should never be programmed to.
 * Some size values appear twice, due to single-sided vs dual-sided banks.
 */
static int translate_i82810_to_mb[] = {
/* DRP	0  1 (2) 3   4   5   6   7   8   9   A   B   C    D    E    F */
/* MB */0, 8, 0, 16, 16, 24, 32, 32, 48, 64, 64, 96, 128, 128, 192, 256,
};

#if HAVE_HIGH_TABLES==1
#define HIGH_TABLES_SIZE 64	// maximum size of high tables in KB
extern uint64_t high_tables_base, high_tables_size;
#endif

static void pci_domain_set_resources(device_t dev)
{
	device_t mc_dev;
	uint32_t pci_tolm;

	pci_tolm = find_pci_tolm(&dev->link[0]);
	mc_dev = dev->link[0].children;

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
		int drp_value;

		/* First get the value for DIMM 0. */
		drp_value = pci_read_config8(mc_dev, DRP);
		/* Translate it to MB and add to tomk. */
		tomk = (unsigned long)(translate_i82810_to_mb[drp_value & 0xf]);
		/* Now do the same for DIMM 1. */
		drp_value = drp_value >> 4;	// >>= 4; //? mess with later
		tomk += (unsigned long)(translate_i82810_to_mb[drp_value]);

		printk_debug("Setting RAM size to %d MB\n", tomk);

		/* Convert tomk from MB to KB. */
		tomk = tomk << 10;

		/* Compute the top of Low memory. */
		tolmk = pci_tolm >> 10;
		if (tolmk >= tomk) {
			/* The PCI hole does does not overlap the memory. */
			tolmk = tomk;
		}

		/* Report the memory regions. */
		idx = 10;
		ram_resource(dev, idx++, 0, 640);
		ram_resource(dev, idx++, 1024, tolmk - 1024);

#if HAVE_HIGH_TABLES==1
		/* Leave some space for ACPI, PIRQ and MP tables */
		high_tables_base = (tomk - HIGH_TABLES_SIZE) * 1024;
		high_tables_size = HIGH_TABLES_SIZE * 1024;
#endif
	}
	assign_resources(&dev->link[0]);
}

static unsigned int pci_domain_scan_bus(device_t dev, unsigned int max)
{
	max = pci_scan_bus(&dev->link[0], PCI_DEVFN(0, 0), 0xff, max);
	return max;
}

static struct device_operations pci_domain_ops = {
	.read_resources		= pci_domain_read_resources,
	.set_resources		= pci_domain_set_resources,
	.enable_resources	= enable_childrens_resources,
	.init			= 0,
	.scan_bus		= pci_domain_scan_bus,
};

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(&dev->link[0]);
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
	struct device_path path;

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
