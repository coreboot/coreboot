/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Rudolf Marek <r.marek@assembler.cz>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/i8259.h>
#include <stdlib.h>

static const unsigned char enetIrqs[4] = { 10, 0, 0, 0 };
static const unsigned char usbIrqs[4] = { 15, 14, 0, 0 };

static void pci_routing_fixup(struct device *dev)
{
	pci_assign_irqs(0, 0x8, enetIrqs);
	pci_assign_irqs(0, 0xa, usbIrqs);
}

static void r8610_init(struct device *dev)
{
	device_t nb_dev;
	u32 tmp;

	printk(BIOS_DEBUG, "r8610 init\n");

	/* clear DMA? */
	outb(0x4, 0x8);
	outb(0x4, 0x10);

	outb(0xfc, 0x61);

	/* Set serial base */
	pci_write_config32(dev, 0x54, 0x3f8);
	/* serial IRQ disable, LPC disable, COM2 goes to LPC, internal UART for COM1 */
	pci_write_config32(dev, 0x50, 0x84101012);

	/* Enable internal Port92, enable chipselect for flash */
	tmp = pci_read_config32(dev, 0x40);
	pci_write_config32(dev, 0x40, tmp | 0x07FF0600);

	/* buffer strength SB pins */
	pci_write_config32(dev, 0x5c, 0x2315);

	/*  EHCI 14, OHCI 15, MAC1 disable, MAC0 10, INTD 9, INTC 9, INTB 12, INTA INT10 */
	pci_write_config32(dev, 0x58, 0xdf0311b3);

	/* USB PHY control */
	nb_dev = dev_find_device(PCI_VENDOR_ID_RDC,
				 PCI_DEVICE_ID_RDC_R8610_NB, 0);

	tmp = pci_read_config32(nb_dev, 0xc0);
	tmp |= 0x40000;
	pci_write_config32(nb_dev, 0xc0, tmp);

	setup_i8259();
}

static void r8610_read_resources(device_t dev)
{
	struct resource *res;

	pci_dev_read_resources(dev);

	res = new_resource(dev, 1);
	res->base = 0x0UL;
	res->size = 0x1000UL;
	res->limit = 0xffffUL;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* Reserve space for flash */
	res = new_resource(dev, 2);
	res->base = 0xff800000;
	res->size = 8*1024*1024;
	res->limit = 0xffffffffUL;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED | IORESOURCE_STORED |
		     IORESOURCE_ASSIGNED;
}

static void southbridge_init(struct device *dev)
{
	r8610_init(dev);
	pci_routing_fixup(dev);
}

static struct device_operations r8610_sb_ops = {
	.read_resources   = r8610_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = &southbridge_init,
	.scan_bus	  = scan_static_bus,
	.enable           = 0,
	.ops_pci          = 0,
};

static const struct pci_driver lpc_driver __pci_driver = {
	.ops	= &r8610_sb_ops,
	.vendor = PCI_VENDOR_ID_RDC,
	.device = PCI_DEVICE_ID_RDC_R8610_SB,
};

