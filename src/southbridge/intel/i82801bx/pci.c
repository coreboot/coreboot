/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Tyan Computer
 * (Written by Yinghai Lu <yinghailu@gmail.com> for Tyan Computer)
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82801bx.h"

static void pci_init(struct device *dev)
{
	u16 reg16;

	/* Clear system errors */
	reg16 = pci_read_config16(dev, PCI_STATUS);
	reg16 |= 0xf900;	/* Clear possible errors */
	pci_write_config16(dev, PCI_STATUS, reg16);

	reg16 = pci_read_config16(dev, SECSTS);
	reg16 |= 0xf800;	/* Clear possible errors */
	pci_write_config16(dev, SECSTS, reg16);
}

static struct device_operations pci_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.init			= pci_init,
	.scan_bus		= pci_scan_bridge,
};

/* 82801BA/BAM (ICH2/ICH2-M) */
static const struct pci_driver i82801misc_pci __pci_driver = {
	.ops	= &pci_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x244e,
};
