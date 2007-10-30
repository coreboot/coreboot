/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published by
 * the Free Software Foundation.
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
#include <device/pci_ops.h>
#include <device/pci_ids.h>

#define SATA_MISC_CTRL 0x45

static void sata_init(struct device *dev)
{
	u8 reg;

	printk_debug("Configuring VIA SATA controller\n");

	/* Class IDE Disk */
	reg = pci_read_config8(dev, SATA_MISC_CTRL);
	reg &= 0x7f;		/* Sub Class Write Protect off */
	pci_write_config8(dev, SATA_MISC_CTRL, reg);

	/* Change the device class to SATA from RAID. */
	pci_write_config8(dev, PCI_CLASS_DEVICE, 0x1);
	reg |= 0x80;		/* Sub Class Write Protect on */
	pci_write_config8(dev, SATA_MISC_CTRL, reg);
}

static struct device_operations sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sata_init,
	.enable = 0,
	.ops_pci = 0,
};

static struct pci_driver northbridge_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_VT6420_SATA,
};
