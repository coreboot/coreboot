/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <arch/io.h>
#include <baytrail/pci_devs.h>
#include <baytrail/ramstage.h>
#include <baytrail/sata.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include "chip.h"

typedef struct soc_intel_baytrail_config config_t;

static inline void sir_write(struct device *dev, int idx, u32 value)
{
	pci_write_config32(dev, SATA_SIRI, idx);
	pci_write_config32(dev, SATA_SIRD, value);
}

static void sata_init(struct device *dev)
{
	config_t *config = dev->chip_info;
	u32 reg32;
	u16 reg16;
	u8  reg8;

	printk(BIOS_DEBUG, "SATA: Initializing...\n");

	if (config == NULL) {
		printk(BIOS_ERR, "SATA: ERROR: Device not in devicetree.cb!\n");
		return;
	}

	if (!config->sata_ahci) {
		/* Set legacy or native decoding mode */
		if (config->ide_legacy_combined) {
			reg8 = pci_read_config8(dev, 0x09);
			reg8 &= ~0x5;
			pci_write_config8(dev, 0x09, reg8);
		} else {
			reg8 = pci_read_config8(dev, 0x09);
			reg8 |= 0x5;
			pci_write_config8(dev, 0x09, reg8);
		}

		/* Set capabilities pointer */
		pci_write_config8(dev, 0x34, 0x70);
		reg16 = pci_read_config16(dev, 0x70);
		reg16 &= ~0xFF00;
		pci_write_config16(dev, 0x70, reg16);
	}

	/* Primary timing - decode enable */
	reg16 = pci_read_config16(dev, 0x40);
	reg16 |= 1 << 15;
	pci_write_config16(dev, 0x40, reg16);

	/* Secondary timing - decode enable */
	reg16 = pci_read_config16(dev, 0x42);
	reg16 |= 1 << 15;
	pci_write_config16(dev, 0x42, reg16);

	/* Port mapping enables */
	reg16 = pci_read_config16(dev, 0x90);
	reg16 |= (config->sata_port_map ^ 0x3) << 8;
	pci_write_config16(dev, 0x90, reg16);

	/* Port control enables */
	reg16 = pci_read_config16(dev, 0x92);
	reg16 &= ~0x003f;
	reg16 |= config->sata_port_map;
	pci_write_config16(dev, 0x92, reg16);

	if (config->sata_ahci) {
		u32 abar = pci_read_config32(dev, PCI_BASE_ADDRESS_5);

		/* Enable CR memory space decoding */
		reg16 = pci_read_config16(dev, 0x04);
		reg16 |= 0x2;
		pci_write_config16(dev, 0x04, reg16);

		/* Set capability register */
		reg32 = read32(abar + 0x00);
		reg32 |= 0x0c046000;  // set PSC+SSC+SALP+SSS+SAM
		reg32 &= ~0x00f20060; // clear SXS+EMS+PMS+gen bits
		reg32 |= (0x3 << 20); // Gen3 SATA
		write32(abar + 0x00, reg32);

		/* Ports enabled */
		reg32 = read32(abar + 0x0c);
		reg32 &= (u32)(~0x3f);
		reg32 |= config->sata_port_map;
		write32(abar + 0xc, reg32);
		/* Two extra reads to latch */
		read32(abar + 0x0c);
		read32(abar + 0x0c);

		/* Set cap2 - Support devslp */
		reg32 = (1 << 5) | (1 << 4) | (1 << 3);
		write32(abar + 0x24, reg32);

		/* Set PxCMD registers */
		reg32 = read32(abar + 0x118);
		reg32 &= ~((1 << 27) | (1 << 26) | (1 << 22) | (1 << 21) |
			(1 << 19) | (1 << 18) | (1 << 1));
		reg32 |= 2;
		write32(abar + 0x118, reg32);

		reg32 = read32(abar + 0x198);
		reg32 &= ~((1 << 27) | (1 << 26) | (1 << 22) | (1 << 21) |
			(1 << 19) | (1 << 18) | (1 << 1));
		reg32 |= 2;
		write32(abar + 0x198, reg32);

		/* Clear reset features */
		write32(abar + 0xc8, 0);

		/* Enable interrupts */
		reg8 = read8(abar + 0x04);
		reg8 |= 0x02;
		write8(abar + 0x04, reg8);

	} else {
		/* TODO(shawnn): Configure IDE SATA speed regs */
	}

	/* 1.4 us delay after configuring port / enable bits */
	udelay(2);

	/* Enable clock for ports */
	reg32 = pci_read_config32(dev, 0x94);
	reg32 |= 0x3f << 24;
	pci_write_config32(dev, 0x94, reg32);
	reg32 &= (config->sata_port_map ^ 0x3) << 24;
	pci_write_config32(dev, 0x94, reg32);

	/* Lock SataGc register */
	reg32 = (0x1 << 31) | (0x7 << 12);
	pci_write_config32(dev, 0x98, reg32);
}

static void sata_enable(device_t dev)
{
	config_t *config = dev->chip_info;
	u8  reg8;
	u16 reg16;
	u32 reg32;

	southcluster_enable_dev(dev);
	if (!config)
		return;

	/* Port mapping -- mask off SPD + SMS + SC bits, then re-set */
	reg16 = pci_read_config16(dev, 0x90);
	reg16 &= ~0x03e0;
	reg16 |= (config->sata_port_map ^ 0x3) << 8;
	if(config->sata_ahci)
		reg16 |= 0x60;
	pci_write_config16(dev, 0x90, reg16);

	/* Set reg 0x94 before starting configuration */
	reg32 = pci_read_config32(dev, 0x94);
	reg32 &= (u32)(~0x1ff);
	reg32 |= 0x183;
	pci_write_config32(dev, 0x94, reg32);

	/* Set ORM bit */
	reg16 = pci_read_config16(dev, 0x92);
	reg16 |= (1 << 15);
	pci_write_config16(dev, 0x92, reg16);

	/* R_PCH_SATA_TM2 - Undocumented in EDS, set according to ref. code */
	reg32 = pci_read_config32(dev, 0x98);
	reg32 &= (u32)~(0x1f80 | (1 << 6) | (1 << 5));
	reg32 |= (1 << 29) | (1 << 25) | (1 << 23) | (1 << 22) |
		 (1 << 20) | (1 << 19) | (1 << 18) | (1 << 9) | (1 << 5);
	pci_write_config32(dev, 0x98, reg32);

	/* CMD reg - set bus master enable (BME) */
	reg8 = pci_read_config8(dev, 0x04);
	reg8 |= (1 << 2);
	pci_write_config8(dev, 0x04, reg8);

	/* "Test mode registers" */
	sir_write(dev, 0x70, 0x00288301);
	sir_write(dev, 0x54, 0x00000300);
	sir_write(dev, 0x58, 0x50000000);
	/* "OOB Detection Margin */
	sir_write(dev, 0x6c, 0x130C0603);
	/* "Gasket Control" */
	sir_write(dev, 0xf4, 0);

	/* PCS - Enable requested SATA ports */
	reg8 = pci_read_config8(dev, 0x92);
	reg8 &= ~0x03;
	reg8 |= config->sata_port_map;
	pci_write_config8(dev, 0x92, reg8);
}

static struct device_operations sata_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= sata_init,
	.enable			= sata_enable,
	.scan_bus		= NULL,
	.ops_pci		= &soc_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	IDE1_DEVID, IDE2_DEVID,			/* IDE */
	AHCI1_DEVID, AHCI2_DEVID,		/* AHCI */
	0,
};

static const struct pci_driver baytrail_sata __pci_driver = {
	.ops     = &sata_ops,
	.vendor  = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
