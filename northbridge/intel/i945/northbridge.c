/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
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

#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>
#include "i945.h"

static void i945_ram_resource(struct device * dev, unsigned long index, unsigned long basek,
			 unsigned long sizek)
{
	struct resource *resource;

	resource = new_resource(dev, index);
	resource->base = ((resource_t) basek) << 10;
	resource->size = ((resource_t) sizek) << 10;
	resource->flags = IORESOURCE_MEM | IORESOURCE_CACHEABLE |
	    IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED;
}

static void I945_pci_domain_read_resources(struct device * dev)
{
	struct resource *resource;

	/* Initialize the system wide io space constraints */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	resource->base = 0;
	resource->size = 0;
	resource->align = 0;
	resource->gran = 0;
	resource->limit = 0xffffUL;
	resource->flags =
	    IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	/* Initialize the system wide memory resources constraints */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	resource->base = 0;
	resource->size = 0;
	resource->align = 0;
	resource->gran = 0;
	resource->limit = 0xffffffffUL;
	resource->flags =
	    IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
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
	struct resource *min;
	u32 tolm;
	min = 0;
	search_bus_resources(bus, IORESOURCE_MEM, IORESOURCE_MEM, tolm_test,
			     &min);
	tolm = 0xffffffffUL;
	if (min && tolm > min->base) {
		tolm = min->base;
	}
	return tolm;
}

static void I945_pci_domain_set_resources(struct device * dev)
{
	u32 pci_tolm;
	u8 tolud, reg8;
	u16 reg16;
	unsigned long long tomk, tolmk;

	pci_tolm = find_pci_tolm(&dev->link[0]);

	printk(BIOS_SPEW, "Base of stolen memory: 0x%08x\n",
		    pci_read_config32(dev_find_slot(0, PCI_DEVFN(2, 0)), 0x5c));

	tolud = pci_read_config8(dev_find_slot(0, PCI_DEVFN(0, 0)), 0x9c);
	printk(BIOS_SPEW, "Top of Low Used DRAM: 0x%08x\n", tolud << 24);

	tomk = tolud << 14;

	/* Note: subtract IGD device and TSEG */
	reg8 = pci_read_config8(dev_find_slot(0, PCI_DEVFN(0, 0)), 0x9e);
	if (reg8 & 1) {
		int tseg_size = 0;
		printk(BIOS_DEBUG, "TSEG decoded, subtracting ");
		reg8 >>= 1;
		reg8 &= 3;
		switch (reg8) {
		case 0:
			tseg_size = 1024;
			break;	
		case 1:
			tseg_size = 2048;
			break;	
		case 2:
			tseg_size = 8192;
			break;	
		}

		printk(BIOS_DEBUG, "%dM\n", tseg_size >> 10);
		tomk -= tseg_size;
	}

	reg16 = pci_read_config16(dev_find_slot(0, PCI_DEVFN(0, 0)), GGC);
	if (!(reg16 & 2)) {
		int uma_size = 0;
		printk(BIOS_DEBUG, "IGD decoded, subtracting ");
		reg16 >>= 4;
		reg16 &= 7;
		switch (reg16) {
		case 1:
			uma_size = 1024;
			break;
		case 3:
			uma_size = 8192;
			break;
		}

		printk(BIOS_DEBUG, "%dM UMA\n", uma_size >> 10);
		tomk -= uma_size;
	}

	/* The following needs to be 2 lines, otherwise the second
	 * number is always 0
	 */
	printk(BIOS_INFO, "Available memory: %lldK", tomk);
	printk(BIOS_INFO, " (%lldM)\n", (tomk >> 10));

	tolmk = tomk;

	/* Report the memory regions */
	i945_ram_resource(dev, 3, 0, 640);
	i945_ram_resource(dev, 4, 768, (tolmk - 768));
	if (tomk > 4 * 1024 * 1024) {
		i945_ram_resource(dev, 5, 4096 * 1024, tomk - 4 * 1024 * 1024);
	}

	phase4_set_resources(&dev->link[0]);
}

static unsigned int i945_pci_domain_scan_bus(struct device * dev, unsigned int max)
{
	max = pci_scan_bus(&dev->link[0], 0, 0xff, max);
	/* TODO We could determine how many PCIe busses we need in
	 * the bar. For now that number is hardcoded to a max of 64.
	 */
	return max;
}

#warning get number of 945 pci domain ops
struct device_operations i945_pci_domain_ops = {
	.id = {.type = DEVICE_ID_PCI,
	       {.pci = {.vendor = PCI_VENDOR_ID_INTEL,
			.device = 0x6789}}},
	.constructor		 = default_device_constructor,
	.reset_bus		 = pci_bus_reset,
	.phase3_scan		 = i945_pci_domain_scan_bus,
	.phase4_read_resources	 = I945_pci_domain_read_resources,
	.phase4_set_resources	 = I945_pci_domain_set_resources,
	.phase5_enable_resources = enable_childrens_resources,
	.phase6_init		 = NULL,
	.ops_pci		 = &pci_dev_ops_pci,
	.ops_pci_bus		 = &pci_cf8_conf1,	/* Do we want to use the memory mapped space here? */
};

static void mc_read_resources(struct device * dev)
{
	struct resource *resource;

	pci_dev_read_resources(dev);

#warning After we fix the allocator we need to fix this!
	/* So, this is one of the big mysteries in the coreboot resource
	 * allocator. This resource should make sure that the address space
	 * of the PCIe memory mapped config space bar. But it does not.
	 */

	/* We use 0xcf as an unused index for our PCIe bar so that we find it again */
	resource = new_resource(dev, 0xcf);
	resource->base = DEFAULT_PCIEXBAR;
	resource->size = 64 * 1024 * 1024;	/* 64MB hard coded PCIe config space */
	resource->flags =
	    IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED |
	    IORESOURCE_ASSIGNED;

	printk(BIOS_DEBUG, "Adding PCIe enhanced config space BAR 0x%08llx-0x%08llx.\n",
	       resource->base, resource->base + resource->size);
}

static void mc_set_resources(struct device * dev)
{
	struct resource *resource, *last;

	/* Report the PCIe BAR */
	last = &dev->resource[dev->resources];
	resource = find_resource(dev, 0xcf);
	if (resource) {
		report_resource_stored(dev, resource, "<mmconfig>");
	}

	/* And call the normal set_resources */
	pci_set_resources(dev);
}

static void i945_set_subsystem(struct device * dev, u16 vendor, u16 device)
{
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
			   ((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations intel_pci_ops = {
	.set_subsystem    = i945_set_subsystem,
};

struct device_operations i945_mc_ops = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_INTEL,
			      .device = 0x27a0}}},
	.constructor		 = default_device_constructor,
	.phase4_read_resources	 = mc_read_resources,
	.phase4_set_resources	 = mc_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = NULL,
	.ops_pci		 = &intel_pci_ops,
};

#warning need to write initialize_cpus
static void cpu_bus_init(struct device * dev)
{
	/* what to do here? 
	initialize_cpus(&dev->link[0]);
	*/
}

static void cpu_bus_noop(struct device * dev)
{
}

#warning get a number of the 945 mc
struct device_operations i945_cpu_bus_ops = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_INTEL,
			      .device = 0x1233}}},
	.constructor		 = default_device_constructor,
	.phase4_read_resources	 = cpu_bus_noop,
	.phase4_set_resources	 = cpu_bus_noop,
	.phase5_enable_resources = cpu_bus_noop,
	.phase6_init		 = cpu_bus_init,
	.ops_pci		 = &intel_pci_ops,
};
