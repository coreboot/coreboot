/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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
#include <device/device.h>
#include <device/pci.h>
#include <device/pnp.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include "bcm5785.h"

static void lpc_init(device_t dev)
{
	/* Initialize the real time clock */
	cmos_init(0);

	/* Initialize isa dma */
	isa_dma_init();
}

static void bcm5785_lpc_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->base = 0xff800000;
	res->size = 0x00800000; /* 8 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

/**
 * Enable resources for children devices.
 *
 * @param dev The device whos children's resources are to be enabled.
 */
static void bcm5785_lpc_enable_childrens_resources(device_t dev)
{
	struct bus *link;
	uint32_t reg;

	reg = pci_read_config8(dev, 0x44);

	for (link = dev->link_list; link; link = link->next) {
		device_t child;
		for (child = link->children; child; child = child->sibling) {
			if (child->enabled && (child->path.type == DEVICE_PATH_PNP)) {
				struct resource *res;
				for (res = child->resource_list; res; res = res->next) {
					unsigned long base, end; // don't need long long
					if (!(res->flags & IORESOURCE_IO)) continue;
					base = res->base;
					end = resource_end(res);
					printk(BIOS_DEBUG, "bcm5785lpc decode:%s, base=0x%08lx, end=0x%08lx\n",dev_path(child),base, end);
					switch (base) {
					case 0x60: //KBC
					case 0x64:
						reg |= (1<<29); break;
					case 0x3f8: // COM1
						reg |= (1<<6); 	break;
					case 0x2f8: // COM2
						reg |= (1<<7);  break;
					case 0x378: // Parallel 1
						reg |= (1<<0); break;
					case 0x3f0: // FD0
						reg |= (1<<26); break;
					case 0x220:  // Audio 0
						reg |= (1<<14);	break;
					case 0x300:  // MIDI 0
						reg |= (1<<18);	break;
					}
				}
			}
		}
	}
	pci_write_config32(dev, 0x44, reg);


}

static void bcm5785_lpc_enable_resources(device_t dev)
{
	pci_dev_enable_resources(dev);
	bcm5785_lpc_enable_childrens_resources(dev);
}

static void lpci_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x40,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

static struct device_operations lpc_ops  = {
	.read_resources   = bcm5785_lpc_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = bcm5785_lpc_enable_resources,
	.init             = lpc_init,
	.scan_bus         = scan_lpc_bus,
//	.enable           = bcm5785_enable,
	.ops_pci          = &lops_pci,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops    = &lpc_ops,
	.vendor = PCI_VENDOR_ID_SERVERWORKS,
	.device = PCI_DEVICE_ID_SERVERWORKS_BCM5785_LPC,
};
