/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

/* IDE specific bits */
#define IDE_MODE_REG		0x09
#define IDE0_NATIVE_MODE	(1 << 0)
#define IDE1_NATIVE_MODE	(1 << 2)

/* These are default addresses */
#define IDE0_DATA_ADDR		0x1f0
#define IDE0_CONTROL_ADDR	0x3f4
#define IDE1_DATA_ADDR		0x170
#define IDE1_CONTROL_ADDR	0x370

#define BUS_MASTER_ADDR		0xfc00

#define CHANNEL_ENABLE_REG	0x40
#define ENABLE_IDE0		(1 << 0)
#define ENABLE_IDE1		(1 << 1)

/* TODO: better user configuration */
#define DISABLE_SATA 0

static void sata_init(struct device *dev)
{
	u8 reg8;

	printk(BIOS_DEBUG, "Configuring VIA SATA & EIDE Controller\n");

	/* Class IDE Disk, instead of RAID controller */
	reg8 = pci_read_config8(dev, 0x45);
	reg8 &= 0x7f;		/* Sub Class Write Protect off */
	pci_write_config8(dev, 0x45, reg8);
	pci_write_config8(dev, 0x0a, 0x01);
	reg8 |= 0x80;		/* Sub Class Write Protect on */
	pci_write_config8(dev, 0x45, reg8);

#if defined(DISABLE_SATA) && (DISABLE_SATA == 1)
	printk(BIOS_INFO, "Disabling SATA (Primary Channel)\n");
	/* Disable SATA channels */
	pci_write_config8(dev, 0x40, 0x00);
#else
	pci_write_config8(dev, 0x40, 0x43);
#endif

	reg8 = pci_read_config8(dev, 0x6a);
	reg8 |= 0x8;		/* Mode Select set to Manual Mode */
	reg8 &= ~7;
	reg8 |= 0x2;		/* Manual setting to 50 ohm */

	pci_write_config8(dev, 0x6a, reg8);

	reg8 = pci_read_config8(dev, 0x6b);
	reg8 &= ~7;
	reg8 |= 0x01;		/* Autocomp of Termination */
	pci_write_config8(dev, 0x6b, reg8);

	/* Enable EIDE (secondary channel) even if SATA disabled */
	reg8 = pci_read_config8(dev, 0xc0);
	reg8 |= 0x1;
	pci_write_config8(dev, 0xc0, reg8);

	// Enable bus mastering, memory space acces, io space access
	pci_write_config16(dev, 0x04, 0x0007);

	/* Set SATA base ports. */
	pci_write_config32(dev, 0x10, 0x01f1);
	pci_write_config32(dev, 0x14, 0x03f5);
	/* Set EIDE base ports. */
	pci_write_config32(dev, 0x18, 0x0171);
	pci_write_config32(dev, 0x1c, 0x0375);

	/* SATA/EIDE Bus Master mode base address */
	pci_write_config32(dev, 0x20, BUS_MASTER_ADDR | 1);

	/* Enable read/write prefetch buffers */
	reg8 = pci_read_config8(dev, 0xc1);
	reg8 |= 0x30;
	pci_write_config8(dev, 0xc1, reg8);

	/* Set FIFO thresholds like */
	pci_write_config8(dev, 0xc3, 0x1);	/* FIFO flushed when 1/2 full */

	/* EIDE Sector Size */
	pci_write_config16(dev, 0xe8, 0x200);

	/* Some Miscellaneous Control */
	pci_write_config8(dev, 0x44, 0x7);
	pci_write_config8(dev, 0x45, 0xaf);
	pci_write_config8(dev, 0x46, 0x8);

	/* EIDE Configuration */
	reg8 = pci_read_config8(dev, 0xc4);
	reg8 |= 0x10;
	pci_write_config8(dev, 0xc4, reg8);

	pci_write_config8(dev, 0xc5, 0xc);

	/* Interrupt Line */
	reg8 = pci_read_config8(dev, 0x45);
	reg8 &= ~(1 << 4);	/* Interrupt Line Write Protect off */
	pci_write_config8(dev, 0x45, reg8);

	pci_write_config8(dev, 0x3c, 0x0e);	/* Interrupt */

	/* Set the drive timing control */
	pci_write_config16(dev, 0x48, 0x5d5d);

	/* Enable only compatibility mode. */
	reg8 = pci_read_config8(dev, 0x42);
	reg8 &= ~0xa0;
	pci_write_config8(dev, 0x42, reg8);
	reg8 = pci_read_config8(dev, 0x42);
	printk(BIOS_DEBUG, "Reg 0x42 read back as 0x%x\n", reg8);

	/* Support Staggered Spin-Up */
	reg8 = pci_read_config8(dev, 0xb9);
	if ((reg8 & 0x8) == 0) {
		printk(BIOS_DEBUG, "start OOB sequence on both drives\n");
		reg8 |= 0x30;
		pci_write_config8(dev, 0xb9, reg8);
	}
}

static struct device_operations sata_ops = {
	.read_resources	  = pci_dev_read_resources,
	.set_resources	  = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = sata_init,
	.enable		  = 0,
	.ops_pci	  = 0,
};

/* When the SATA controller is in IDE mode, the Device ID is 0x5324 */
static const struct pci_driver northbridge_driver __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = 0x5324,
};
