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
#include "pch.h"

static void pci_init(struct device *dev)
{
	u16 reg16;
	u8 reg8;

	printk(BIOS_DEBUG, "PCI init.\n");
	/* Enable Bus Master */
	reg16 = pci_read_config16(dev, PCI_COMMAND);
	reg16 |= PCI_COMMAND_MASTER;
	pci_write_config16(dev, PCI_COMMAND, reg16);

	/* This device has no interrupt */
	pci_write_config8(dev, INTR, 0xff);

	/* disable parity error response and SERR */
	reg16 = pci_read_config16(dev, BCTRL);
	reg16 &= ~(1 << 0);
	reg16 &= ~(1 << 1);
	pci_write_config16(dev, BCTRL, reg16);

	/* Master Latency Count must be set to 0x04! */
	reg8 = pci_read_config8(dev, SMLT);
	reg8 &= 0x07;
	reg8 |= (0x04 << 3);
	pci_write_config8(dev, SMLT, reg8);

	/* Will this improve throughput of bus masters? */
	pci_write_config8(dev, PCI_MIN_GNT, 0x06);

	/* Clear errors in status registers */
	reg16 = pci_read_config16(dev, PSTS);
	//reg16 |= 0xf900;
	pci_write_config16(dev, PSTS, reg16);

	reg16 = pci_read_config16(dev, SECSTS);
	// reg16 |= 0xf900;
	pci_write_config16(dev, SECSTS, reg16);
}

static void ich_pci_dev_enable_resources(struct device *dev)
{
	const struct pci_operations *ops;
	uint16_t command;

	/* Set the subsystem vendor and device id for mainboard devices */
	ops = ops_pci(dev);
	if (dev->on_mainboard && ops && ops->set_subsystem) {
		printk(BIOS_DEBUG, "%s subsystem <- %02x/%02x\n",
			dev_path(dev),
			CONFIG_MAINBOARD_PCI_SUBSYSTEM_VENDOR_ID,
			CONFIG_MAINBOARD_PCI_SUBSYSTEM_DEVICE_ID);
		ops->set_subsystem(dev,
			CONFIG_MAINBOARD_PCI_SUBSYSTEM_VENDOR_ID,
			CONFIG_MAINBOARD_PCI_SUBSYSTEM_DEVICE_ID);
	}

	command = pci_read_config16(dev, PCI_COMMAND);
	command |= dev->command;
	printk(BIOS_DEBUG, "%s cmd <- %02x\n", dev_path(dev), command);
	pci_write_config16(dev, PCI_COMMAND, command);
}

static void ich_pci_bus_enable_resources(struct device *dev)
{
	uint16_t ctrl;
	/* enable IO in command register if there is VGA card
	 * connected with (even it does not claim IO resource)
	 */
	if (dev->link_list->bridge_ctrl & PCI_BRIDGE_CTL_VGA)
		dev->command |= PCI_COMMAND_IO;
	ctrl = pci_read_config16(dev, PCI_BRIDGE_CONTROL);
	ctrl |= dev->link_list->bridge_ctrl;
	ctrl |= (PCI_BRIDGE_CTL_PARITY + PCI_BRIDGE_CTL_SERR); /* error check */
	printk(BIOS_DEBUG, "%s bridge ctrl <- %04x\n", dev_path(dev), ctrl);
	pci_write_config16(dev, PCI_BRIDGE_CONTROL, ctrl);

	/* This is the reason we need our own pci_bus_enable_resources */
	ich_pci_dev_enable_resources(dev);
}

static void set_subsystem(struct device *dev, unsigned vendor, unsigned device)
{
	/* NOTE: This is not the default position! */
	if (!vendor || !device) {
		pci_write_config32(dev, 0x54,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, 0x54,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations pci_ops = {
	.set_subsystem = set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= ich_pci_bus_enable_resources,
	.init			= pci_init,
	.scan_bus		= pci_scan_bridge,
	.ops_pci		= &pci_ops,
};

static const struct pci_driver pch_pci __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x2448,
};
