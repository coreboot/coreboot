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
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include <cpu/x86/cache.h>
#include "chip.h"

#define BRIDGE_IO_MASK (IORESOURCE_IO | IORESOURCE_MEM)

static void pci_domain_read_resources(device_t dev)
{
        struct resource *resource;
        unsigned reg;

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
        resource->base  = ((resource_t)basek) << 10;
        resource->size  = ((resource_t)sizek) << 10;
        resource->flags =  IORESOURCE_MEM | IORESOURCE_CACHEABLE | \
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
	search_bus_resources(bus, IORESOURCE_MEM, IORESOURCE_MEM, tolm_test, &min);
	tolm = 0xffffffffUL;
	if (min && tolm > min->base) {
		tolm = min->base;
	}
	return tolm;
}

static void pci_domain_set_resources(device_t dev)
{
	device_t mc_dev;
        uint32_t pci_tolm;
        
        printk_debug("Entered with dev vid = %x\n", dev->vendor);
	printk_debug("Entered with dev did = %x\n", dev->device);

        pci_tolm = find_pci_tolm(&dev->link[0]);	
	mc_dev = dev->link[0].children->sibling;
	printk_debug("MC dev vendor = %x\n", mc_dev->vendor);
	printk_debug("MC dev device = %x\n", mc_dev->device);
	
	if (mc_dev) {
		/* Figure out which areas are/should be occupied by RAM.
		 * This is all computed in kilobytes and converted to/from
		 * the memory controller right at the edges.
		 * Having different variables in different units is
		 * too confusing to get right.  Kilobytes are good up to
		 * 4 Terabytes of RAM...
		 */
		uint16_t tolm_r, vga_mem;
		unsigned long tomk, tolmk;
		unsigned long remapbasek, remaplimitk;
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
		/* Write the ram configuration registers,
		 * preserving the reserved bits.
		 */
		 
		/* Report the memory regions */
		printk_debug("tomk = %d\n", tomk);
		printk_debug("tolmk = %d\n", tolmk);

		idx = 10;
		/* avoid pam region */
		ram_resource(dev, idx++, 0, 640);
		/* ram_resource(dev, idx++, 1024, tolmk - 1024); */
		ram_resource(dev, idx++, 768, tolmk - 768);

	}
	assign_resources(&dev->link[0]);
}

static unsigned int pci_domain_scan_bus(device_t dev, unsigned int max)
{
        max = pci_scan_bus(&dev->link[0], PCI_DEVFN(0, 0), 0xff, max);
        return max;
}

static struct device_operations pci_domain_ops = {
        .read_resources   = pci_domain_read_resources,
        .set_resources    = pci_domain_set_resources,
        .enable_resources = enable_childrens_resources,
        .init             = 0,
        .scan_bus         = pci_domain_scan_bus,
};  

static void cpu_bus_init(device_t dev)
{
        initialize_cpus(&dev->link[0]);
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

static void enable_dev(struct device *dev)
{
        struct device_path path;

        /* Set the operations if it is a special bus type */
        if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
                dev->ops = &pci_domain_ops;
		pci_set_method(dev);
        }
        else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER) {
                dev->ops = &cpu_bus_ops;
        }
}

struct chip_operations northbridge_intel_i855gme_ops = {
        CHIP_NAME("Intel 855GME Northbridge")
	.enable_dev = enable_dev,
};
