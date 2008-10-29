/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 coresystems GmbH
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
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82801gx.h"

static void usb_init(struct device *dev)
{
	u32 reg32;
	u8 reg8;

	/* USB Specification says the device must be Bus Master */
	printk_debug("UHCI: Setting up controller.. ");

	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 | PCI_COMMAND_MASTER);

	reg8 = pci_read_config8(dev, 0xca);
	reg8 |= (1 << 0);
	pci_write_config8(dev, 0xca, reg8);

	printk_debug("done.\n");
}

static struct device_operations usb_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= usb_init,
	.scan_bus		= 0,
	.enable			= i82801gx_enable,
};

/* 82801GB/GR/GDH/GBM/GHM/GU (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH/ICH7-U) */
static const struct pci_driver i82801gb_usb1 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801GB_USB1,
};

static const struct pci_driver i82801gb_usb2 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801GB_USB2,
};

static const struct pci_driver i82801gb_usb3 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801GB_USB3,
};

static const struct pci_driver i82801gb_usb4 __pci_driver = {
	.ops	= &usb_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= PCI_DEVICE_ID_INTEL_82801GB_USB4,
};


