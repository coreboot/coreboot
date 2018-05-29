/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82801gx.h"
#include <device/pci_ehci.h>
#include <arch/io.h>

static void usb_ehci_init(struct device *dev)
{
	struct resource *res;
	u8 *base;
	u32 reg32;
	u8 reg8;

	printk(BIOS_DEBUG, "EHCI: Setting up controller.. ");
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER;
	reg32 |= PCI_COMMAND_SERR;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	reg32 = pci_read_config32(dev, 0xdc);
	reg32 |= (1 << 31) | (1 << 27);
	pci_write_config32(dev, 0xdc, reg32);

	reg32 = pci_read_config32(dev, 0xfc);
	reg32 &= ~(3 << 2);
	reg32 |= (2 << 2) | (1 << 29) | (1 << 17);
	pci_write_config32(dev, 0xfc, reg32);

	/* Clear any pending port changes */
	res = find_resource(dev, 0x10);
	base = res2mmio(res, 0, 0);
	reg32 = read32(base + 0x24) | (1 << 2);
	write32(base + 0x24, reg32);

	/* workaround */
	reg8 = pci_read_config8(dev, 0x84);
	reg8 |= (1 << 4);
	pci_write_config8(dev, 0x84, reg8);

	printk(BIOS_DEBUG, "done.\n");
}

static void usb_ehci_set_subsystem(struct device *dev, unsigned int vendor,
				   unsigned int device)
{
	u8 access_cntl;

	access_cntl = pci_read_config8(dev, 0x80);

	/* Enable writes to protected registers. */
	pci_write_config8(dev, 0x80, access_cntl | 1);

	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}

	/* Restore protection. */
	pci_write_config8(dev, 0x80, access_cntl);
}

static struct pci_operations lops_pci = {
	.set_subsystem	= &usb_ehci_set_subsystem,
};

static struct device_operations usb_ehci_ops = {
	.read_resources		= pci_ehci_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_ehci_init,
	.scan_bus		= 0,
	.enable			= i82801gx_enable,
	.ops_pci		= &lops_pci,
};

/* 82801GB/GR/GDH/GBM/GHM/GU (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH/ICH7-U) */
static const struct pci_driver i82801gx_usb_ehci __pci_driver = {
	.ops	= &usb_ehci_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27cc,
};
