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
#include <device/hypertransport.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include "chip.h"
#include "northbridge.h"

static void pci_domain_read_resources(device_t dev)
{
	struct resource *resource;

	/* Initialize the system wide io space constraints */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	resource->limit = 0xffffUL;
	resource->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	/* Initialize the system wide memory resources constraints */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	resource->limit = 0xffffffffULL;
	resource->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
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

static u32 find_pci_tolm(struct bus *bus)
{
	struct resource *min = NULL;
	u32 tolm;

	search_bus_resources(bus, IORESOURCE_MEM, IORESOURCE_MEM, tolm_test, &min);
	tolm = 0xffffffffUL;
	if (min && tolm > min->base) {
		tolm = min->base;
	}

	return tolm;
}

#if HAVE_HIGH_TABLES==1
/* maximum size of high tables in KB */
#define HIGH_TABLES_SIZE 64
extern uint64_t high_tables_base, high_tables_size;
#endif

static void pci_domain_set_resources(device_t dev)
{
	device_t mc_dev;
	u32 pci_tolm;
	unsigned char reg;
	unsigned long tomk, tolmk;
	unsigned char rambits;
	int idx;

	pci_tolm = find_pci_tolm(&dev->link[0]);
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

#if HAVE_HIGH_TABLES == 1
	high_tables_base = (tolmk - HIGH_TABLES_SIZE) * 1024;
	high_tables_size = HIGH_TABLES_SIZE* 1024;
	printk_debug("tom: %lx, high_tables_base: %llx, high_tables_size: %llx\n", tomk*1024, high_tables_base, high_tables_size);
#endif

	/* Report the memory regions */
	idx = 10;

	/* TODO: Hole needed? Should this go elsewhere? */
	ram_resource(dev, idx++, 0, 640);	/* first 640k */
	ram_resource(dev, idx++, 768, (tolmk - 768));	/* leave a hole for vga */
	assign_resources(&dev->link[0]);
}

static unsigned int pci_domain_scan_bus(device_t dev, unsigned int max)
{
	max = pci_scan_bus(&dev->link[0], PCI_DEVFN(0, 0), 0xff, max);
	return max;
}

static struct device_operations pci_domain_ops = {
	.read_resources	  = pci_domain_read_resources,
	.set_resources	  = pci_domain_set_resources,
	.enable_resources = enable_childrens_resources,
	.init		  = 0,
	.scan_bus	  = pci_domain_scan_bus,
};

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(&dev->link[0]);
}

static void cpu_bus_noop(device_t dev)
{
}

static struct device_operations cpu_bus_ops = {
	.read_resources	  = cpu_bus_noop,
	.set_resources	  = cpu_bus_noop,
	.enable_resources = cpu_bus_noop,
	.init		  = cpu_bus_init,
	.scan_bus	  = 0,
};

static void enable_dev(struct device *dev)
{
	/* Our wonderful device model */
	if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
		dev->ops = &pci_domain_ops;
		pci_set_method(dev);
	} else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_via_cx700_ops = {
	CHIP_NAME("VIA CX700 Northbridge")
	.enable_dev = enable_dev
};
