/*
 * This file is part of the LinuxBIOS project.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "i82801xx.h"

/* TODO: Set dynamically, if the user only wants one SATA channel or none
 * at all.
 */
static void sata_init(struct device *dev)
{
	/* SATA configuration */
	pci_write_config8(dev, 0x04, 0x07);
	pci_write_config8(dev, 0x09, 0x8f);

	/* Set timmings */
	pci_write_config16(dev, 0x40, 0x0a307);
	pci_write_config16(dev, 0x42, 0x0a307);

	/* Sync DMA */
	pci_write_config16(dev, 0x48, 0x000f);
	pci_write_config16(dev, 0x4a, 0x1111);

	/* 66 MHz */
	pci_write_config16(dev, 0x54, 0xf00f);

	/* Combine IDE - SATA configuration */
	pci_write_config8(dev, 0x90, 0x0);

	/* Port 0 & 1 enable */
	pci_write_config8(dev, 0x92, 0x33);

	/* Initialize SATA. */
	pci_write_config16(dev, 0xa0, 0x0018);
	pci_write_config32(dev, 0xa4, 0x00000264);
	pci_write_config16(dev, 0xa0, 0x0040);
	pci_write_config32(dev, 0xa4, 0x00220043);
}

static struct device_operations sata_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= sata_init,
	.scan_bus		= 0,
	.enable			= i82801xx_enable,
};

/* 82801EB */
static const struct pci_driver i82801eb_sata_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x24d1,
};

/* 82801ER */
static const struct pci_driver i82801er_sata_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x24df,
};
