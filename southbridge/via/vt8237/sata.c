/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007, 2008 Rudolf Marek <r.marek@assembler.cz>
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

#include <console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "vt8237.h"

/* TODO: use phase2_fixup to disable SATA */
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
}

/* VT8237R is SATA, VT8237S is SATAII */
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

	/* TODO: Move to DTS */

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

struct device_operations vt8237r_sata = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_VIA,
				.device = PCI_DEVICE_ID_VIA_VT8237R_SATA}}},
	.constructor			= default_device_constructor,
	.phase3_scan			= 0,
	//.phase4_enable_disable		= vt8237_enable,
	//.phase4_read_resources		= pci_dev_read_resources,
	//.phase4_set_resources		= pci_dev_set_resources,
	//.phase5_enable_resources	= pci_dev_enable_resources,
	.phase6_init			= sata_i_init,
};

struct device_operations vt8237s_sata = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_VIA,
				.device = PCI_DEVICE_ID_VIA_VT8237S_SATA}}},
	.constructor			= default_device_constructor,
	.phase2_fixup			= 0,
	.phase3_scan			= 0,
	//.phase4_enable_disable		= vt8237_enable,
	//.phase4_read_resources		= pci_dev_read_resources,
	//.phase4_set_resources		= pci_dev_set_resources,
	//.phase5_enable_resources	= pci_dev_enable_resources,
	.phase6_init			= sata_ii_init,
};
