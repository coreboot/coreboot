/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

typedef struct southbridge_intel_i82801gx_config config_t;

static void sata_init(struct device *dev)
{
	u32 reg32;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	printk_debug("i82801gx_sata: initializing...\n");
	/* SATA configuration */

	/* Enable BARs */
	pci_write_config16(dev, 0x04, 0x0007);

	if (config->ide_legacy_combined) {
		pci_write_config8(dev, 0x09, 0x80);

		/* Set timings */
		pci_write_config16(dev, IDE_TIM_PRI, 0x8000);
		pci_write_config16(dev, IDE_TIM_SEC, 0xa307);

		/* Sync DMA */
		pci_write_config16(dev, 0x48, 0x0004);
		pci_write_config16(dev, 0x4a, 0x0200);

		/* Combine IDE - SATA configuration */
		pci_write_config8(dev, 0x90, 0x02);

		/* Port 0 & 1 enable */
		pci_write_config8(dev, 0x92, 0x0f);

		/* SATA Initialization register */
		pci_write_config32(dev, 0x94, 0x40000180);
	} else if(config->sata_ahci) {
		/* Allow both Legacy and Native mode */
		pci_write_config8(dev, 0x09, 0x8f);

		/* Set Interrupt Line */
		/* Interrupt Pin is set by D31IP.PIP */
		pci_write_config8(dev, INTR_LN, 0x0a);

		/* Set timings */
		pci_write_config16(dev, IDE_TIM_PRI, 0xa307);
		pci_write_config16(dev, IDE_TIM_SEC, 0x8000);

		/* Sync DMA */
		pci_write_config16(dev, 0x48, 0x0001);
		pci_write_config16(dev, 0x4a, 0x0001);

		/* Set IDE I/O Configuration */
		reg32 = SIG_MODE_NORMAL | FAST_PCB1 | FAST_PCB0 | PCB1 | PCB0;
		pci_write_config32(dev, IDE_CONFIG, reg32);

		/* Set Sata Controller Mode. */
		pci_write_config8(dev, 0x90, 0xc0); // WTF - Reserved?

		/* Port 0 & 1 enable */
		pci_write_config8(dev, 0x92, 0x0f);

		/* SATA Initialization register */
		pci_write_config32(dev, 0x94, 0x1a000180);
	} else {
		/* Native mode capable on both primary and secondary (0xa)
		 * or'ed with enabled (0x50) = 0xf
		 */
		pci_write_config8(dev, 0x09, 0x8f);

		/* Set Interrupt Line */
		/* Interrupt Pin is set by D31IP.PIP */
		pci_write_config8(dev, INTR_LN, 0xff);
	
		/* Set timings */
		pci_write_config16(dev, IDE_TIM_PRI, 0xa307);
		pci_write_config16(dev, IDE_TIM_SEC, 0xe303);
	
		/* Sync DMA */
		pci_write_config16(dev, 0x48, 0x0005);
		pci_write_config16(dev, 0x4a, 0x0201);
	
		/* Set IDE I/O Configuration */
		reg32 = SIG_MODE_NORMAL | FAST_PCB1 | FAST_PCB0 | PCB1 | PCB0;
		pci_write_config32(dev, IDE_CONFIG, reg32);
	
		/* Set Sata Controller Mode. */
		pci_write_config8(dev, 0x90, 0x02);
	
		/* Port 0 & 1 enable XXX */
		pci_write_config8(dev, 0x92, 0x15);
	
		/* SATA Initialization register */
		pci_write_config32(dev, 0x94, 0x1a000180);
	}

	/* All configurations need this SATA initialization sequence */
	pci_write_config8(dev, 0xa0, 0x40);
	pci_write_config8(dev, 0xa6, 0x22);
	pci_write_config8(dev, 0xa0, 0x78);
	pci_write_config8(dev, 0xa6, 0x22);
	pci_write_config8(dev, 0xa0, 0x88);
	reg32 = pci_read_config32(dev, 0xa4);
	reg32 &= 0xc0c0c0c0;
	reg32 |= 0x1b1b1212;
	pci_write_config32(dev, 0xa4, reg32);
	pci_write_config8(dev, 0xa0, 0x8c);
	reg32 = pci_read_config32(dev, 0xa4);
	reg32 &= 0xc0c0ff00;
	reg32 |= 0x121200aa;
	pci_write_config32(dev, 0xa4, reg32);
	pci_write_config8(dev, 0xa0, 0x00);

	pci_write_config8(dev, PCI_INTERRUPT_LINE, 0);
}

static struct device_operations sata_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= sata_init,
	.scan_bus		= 0,
	.enable			= i82801gx_enable,
};

/* Desktop Non-AHCI and Non-RAID Mode */
/* 82801GB/GR/GDH (ICH7/ICH7R/ICH7DH) */
static const struct pci_driver i82801gx_sata_normal_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27c0,
};

/* NOTE: Any of the below are not properly supported yet. */

/* Desktop AHCI Mode */
/* 82801GB/GR/GDH (ICH7/ICH7R/ICH7DH) */
static const struct pci_driver i82801gx_sata_ahci_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27c1,
};

/* Desktop RAID mode */
/* 82801GB/GR/GDH (ICH7/ICH7R/ICH7DH) */
static const struct pci_driver i82801gx_sata_raid_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27c3,
};

/* Mobile Non-AHCI and Non-RAID Mode */
/* 82801GBM/GHM (ICH7-M/ICH7-M DH) */
static const struct pci_driver i82801gx_sata_mobile_normal_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27c4,
};

/* Mobile AHCI Mode */
/* 82801GBM/GHM (ICH7-M/ICH7-M DH) */
static const struct pci_driver i82801gx_sata_mobile_ahci_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27c5,
};

/* ICH7M DH Raid Mode */
/* 82801GHM (ICH7-M DH) */
static const struct pci_driver i82801gx_sata_ich7dh_raid_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27c6,
};
