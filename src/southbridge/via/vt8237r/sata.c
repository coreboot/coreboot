/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007, 2008 Rudolf Marek <r.marek@assembler.cz>
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
#include <device/pci_ids.h>

#define SATA_MISC_CTRL 0x45

static void sata_i_init(struct device *dev)
{
	u8 reg;

	printk(BIOS_DEBUG, "Configuring VIA SATA controller\n");

	/* Class IDE Disk */
	reg = pci_read_config8(dev, SATA_MISC_CTRL);
	reg &= 0x7f;		/* Sub Class Write Protect off */
	pci_write_config8(dev, SATA_MISC_CTRL, reg);

	/* Change the device class to SATA from RAID. */
	pci_write_config8(dev, PCI_CLASS_DEVICE, 0x1);
	reg |= 0x80;		/* Sub Class Write Protect on */
	pci_write_config8(dev, SATA_MISC_CTRL, reg);

	return;
}

static void sata_ii_init(struct device *dev)
{
	u8 reg;

	sata_i_init(dev);

	/*
	 * Analog black magic, you may or may not need to adjust 0x60-0x6f,
	 * depends on PCB.
	 */

	/*
	 * Analog PHY - gen1
	 * CDR bandwidth [6:5] = 3
	 * Squelch Window Select [4:3] = 1
	 * CDR Charge Pump [2:0] = 1
	 */

	pci_write_config8(dev, 0x64, 0x49);

	/* Adjust driver current source value to 9. */
	reg = pci_read_config8(dev, 0x65);
	reg &= 0xf0;
	reg |= 0x9;
	pci_write_config8(dev, 0x65, reg);

	/* Set all manual termination 50ohm bits [2:0] and enable [4]. */
	reg = pci_read_config8(dev, 0x6a);
	reg |= 0xf;
	pci_write_config8(dev, 0x6a, reg);

	/*
	 * Analog PHY - gen2
	 * CDR bandwidth [5:4] = 2
	 * Pre / De-emphasis Level [7:6] controls bits [3:2], rest in 0x6e
	 * CDR Charge Pump [2:0] = 1
	 */

	reg = pci_read_config8(dev, 0x6f);
	reg &= 0x08;
	reg |= 0x61;
	pci_write_config8(dev, 0x6f, reg);

	/* Check if staggered spinup is supported. */
	reg = pci_read_config8(dev, 0x83);
	if ((reg & 0x8) == 0) {
		/* Start OOB sequence on both drives. */
		reg |= 0x30;
		pci_write_config8(dev, 0x83, reg);
	}
}


static void vt8237_set_subsystem(struct device *dev, unsigned vendor,
				 unsigned device)
{
	pci_write_config16(dev, 0xd4, vendor);
	pci_write_config16(dev, 0xd6, device);
}

static struct pci_operations lops_pci = {
	.set_subsystem = vt8237_set_subsystem,
};

static const struct device_operations sata_i_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= sata_i_init,
	.enable			= 0,
	.ops_pci		= &lops_pci,
};

static const struct device_operations sata_ii_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= sata_ii_init,
	.enable			= 0,
	.ops_pci		= &lops_pci,
};

static const struct pci_driver northbridge_driver_ii __pci_driver = {
	.ops	= &sata_ii_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_VT8237_SATA,
};

static const struct pci_driver northbridge_driver_i_a __pci_driver = {
	.ops	= &sata_i_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_VT8237A_SATA,
};

static const struct pci_driver northbridge_driver_i __pci_driver = {
	.ops	= &sata_i_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_VT6420_SATA,
};
