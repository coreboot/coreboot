/*
 * This software and ancillary information (herein called SOFTWARE )
 * called LinuxBIOS          is made available under the terms described
 * here.  The SOFTWARE has been approved for release with associated
 * LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
 * been authored by an employee or employees of the University of
 * California, operator of the Los Alamos National Laboratory under
 * Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
 * U.S. Government has rights to use, reproduce, and distribute this
 * SOFTWARE.  The public may copy, distribute, prepare derivative works
 * and publicly display this SOFTWARE without charge, provided that this
 * Notice and any statement of authorship are reproduced on all copies.
 * Neither the Government nor the University makes any warranty, express
 * or implied, or assumes any liability or responsibility for the use of
 * this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
 * such modified SOFTWARE should be clearly marked, so as not to confuse
 * it with the version available from LANL.
 */

/* Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
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

#include <types.h>
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include "i440bx.h"
#include "config.h"

/* Here are the ops for 440BX as a PCI domain. */
/* A PCI domain contains the I/O and memory resource address space below it. */

static void pci_domain_read_resources(struct device *dev)
{
	struct resource *resource;

	/* Initialize the system wide I/O space constraints. */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	resource->limit = 0xffffUL;
	resource->flags =
	    IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	/* Initialize the system wide memory resources constraints. */
	resource = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	resource->limit = 0xffffffffULL;
	resource->flags =
	    IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
}

static void ram_resource(struct device *dev, unsigned long index,
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
	printk(BIOS_DEBUG, "%s: add ram resoource %d bytes\n", __func__,
	       resource->size);
}

static void pci_domain_set_resources(struct device *dev)
{
	struct device *mc_dev;
	u32 tolmk;		/* Top of low mem, Kbytes. */
	int idx;
	struct northbridge_intel_i440bx_config *device_configuration =
	    dev->device_configuration;
	tolmk = device_configuration->ramsize * 1024;
	mc_dev = dev->link[0].children;
	if (mc_dev) {
		idx = 10;
		ram_resource(dev, idx++, 0, tolmk);
	}
	phase4_assign_resources(&dev->link[0]);
}

static unsigned int pci_domain_scan_bus(struct device *dev, unsigned int max)
{
	/* There is only one link on this device, and it is always link 0. */
	max = pci_scan_bus(&dev->link[0], PCI_DEVFN(0, 0), 0xff, max);
	return max;
}

/* Here are the operations for when the northbridge is running a PCI domain. */
/* See mainboard/emulation/qemu-x86 for an example of how these are used. */
struct device_operations i440bxemulation_pcidomainops = {
	.constructor		 = default_device_constructor,
	.phase3_scan		 = pci_domain_scan_bus,
	.phase4_read_resources	 = pci_domain_read_resources,
	.phase4_set_resources	 = pci_domain_set_resources,
	.phase5_enable_resources = enable_childrens_resources,
	.phase6_init		 = 0,
	.ops_pci_bus		 = &pci_cf8_conf1,

};

/* The constructor for the device. */
/* The plain PCI device uses the standard PCI operations. */
struct constructor i440bx_constructors[] = {
	{.id = {.type = DEVICE_ID_PCI_DOMAIN,
		.u = {.pci = {.vendor = 0x8086,.device = 0x7190}}},
	 &i440bxemulation_pcidomainops},
	{.id = {.type = DEVICE_ID_PCI,
		.u = {.pci = {.vendor = 0x8086,.device = 0x7190}}},
	 &default_pci_ops_bus},
	{.ops = 0},
};
