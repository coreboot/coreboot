/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 Ron Minnich, Advanced Computing Lab, LANL
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2008 Patrick Georgi <patrick@georgi-clan.de>
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

#include <types.h>
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <io.h>
#include "i440bx.h"
#include <statictree.h>

/* Here are the ops for 440BX as a PCI domain. */

static int inb_cmos(int port)
{
	outb(port, 0x70);
	return inb(0x71);
}

static void no_op(struct device *dev)
{
}

static void i440bx_read_resources(struct device *dev)
{
	struct resource *res;

	/* Hole for VGA (0xA0000-0xAFFFF) graphics and text mode
	 * graphics (0xB8000-0xBFFFF) */
	res = new_resource(dev, 1);
	res->base = 0xA0000UL;
	res->size = 0x20000UL;
	res->limit = 0xBFFFUL;
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

}

static void i440bx_ram_resources(struct device *dev)
{
	u32 tolmk;		/* Top of low mem, Kbytes. */
	int idx;

	/* Read the large mem memory descriptor.  If that value is <16 MB, read
	 * the more detailed small mem descriptor.  All values are in kbytes.
	 */
	tolmk = ((inb_cmos(0x35)<<8) |inb_cmos(0x34)) * 64;
	if (tolmk <= 16 * 1024) {
		tolmk = (inb_cmos(0x31)<<8) |inb_cmos(0x30);
	}

	printk(BIOS_WARNING, "Using CMOS settings of %d kB RAM.\n", tolmk);
	idx = 10;

	/* 0 .. 640 kB */
	ram_resource(dev, idx++, 0, 640);

	/* 768 kB .. Systop (in KB) */
	ram_resource(dev, idx++, 768, tolmk - 768);
}

static void i440bx_set_resources(struct device *dev)
{
	/* If there were any NB-specific resources that were not part of the
	 * domain, they would get set here.
	 */

	pci_set_resources(dev);

	/* Add RAM resources. They are not part of resource allocation. */
	i440bx_ram_resources(dev);

	/* If RAM values need to be set, do it here. */
}

static void i440bx_domain_read_resources(struct device *dev)
{
	struct resource *res;

	pci_domain_read_resources(dev);

	/* Reserve space for the IOAPIC.  This should be in the Southbridge,
	 * but I couldn't tell which device to put it in. */
	res = new_resource(dev, 2);
	res->base = 0xfec00000UL;
	res->size = 0x100000UL;
	res->limit = 0xffffffffUL;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED |
		     IORESOURCE_ASSIGNED;

	/* Reserve space for the LAPIC.  There's one in every processor, but
	 * the space only needs to be reserved once, so we do it here. */
	res = new_resource(dev, 3);
	res->base = 0xfee00000UL;
	res->size = 0x10000UL;
	res->limit = 0xffffffffUL;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED |
		     IORESOURCE_ASSIGNED;
}

static void i440bx_domain_set_resources(struct device *dev)
{
	struct resource *res;

	/* If the domain needs these resources set in BARs, do it here. */

	/* Domain I/O resource. */
	res = probe_resource(dev,0);
	if (res)
		res->flags |= IORESOURCE_STORED;

	/* Domain Memory resource. */
	res = probe_resource(dev,1);
	if (res)
		res->flags |= IORESOURCE_STORED;

	phase4_set_resources(&dev->link[0]);
}

/* Here are the operations for when the northbridge is running a PCI domain. */
/* See mainboard/emulation/qemu-x86 for an example of how these are used. */
struct device_operations i440bx_domain = {
	.id = {.type = DEVICE_ID_PCI_DOMAIN,
		{.pci_domain = {.vendor = 0x8086,.device = 0x7190}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = pci_domain_scan_bus,
	.phase4_read_resources	 = i440bx_domain_read_resources,
	.phase4_set_resources	 = i440bx_domain_set_resources,
	.phase5_enable_resources = enable_childrens_resources,
	.phase6_init		 = no_op,
	.ops_pci_bus		 = &pci_cf8_conf1,

};

/* Here are the operations for the northbridge. */
struct device_operations i440bx_northbridge = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = 0x8086,.device = 0x1237}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = NULL,
	.phase4_read_resources	 = i440bx_read_resources,
	.phase4_set_resources	 = i440bx_set_resources,
	.phase5_enable_resources = no_op,
	.phase6_init		 = no_op,
	.ops_pci_bus		 = &pci_cf8_conf1,

};
