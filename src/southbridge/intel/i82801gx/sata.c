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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
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
	u16 reg16;
	u32 *ahci_bar;

	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	printk(BIOS_DEBUG, "i82801gx_sata: initializing...\n");

	if (config == NULL) {
		printk(BIOS_ERR, "i82801gx_sata: error: device not in devicetree.cb!\n");
		return;
	}

	/* SATA configuration */

	/* Enable BARs */
	pci_write_config16(dev, PCI_COMMAND, 0x0007);

	if (config->ide_legacy_combined) {
		printk(BIOS_DEBUG, "SATA controller in combined mode.\n");
		/* No AHCI: clear AHCI base */
		pci_write_config32(dev, 0x24, 0x00000000);
		/* And without AHCI BAR no memory decoding */
		reg16 = pci_read_config16(dev, PCI_COMMAND);
		reg16 &= ~PCI_COMMAND_MEMORY;
		pci_write_config16(dev, PCI_COMMAND, reg16);

		pci_write_config8(dev, 0x09, 0x80);

		/* Set timings */
		pci_write_config16(dev, IDE_TIM_PRI, IDE_DECODE_ENABLE |
				IDE_ISP_5_CLOCKS | IDE_RCT_4_CLOCKS);
		pci_write_config16(dev, IDE_TIM_SEC, IDE_DECODE_ENABLE |
				IDE_ISP_3_CLOCKS | IDE_RCT_1_CLOCKS |
				IDE_PPE0 | IDE_IE0 | IDE_TIME0);

		/* Sync DMA */
		pci_write_config16(dev, IDE_SDMA_CNT, IDE_SSDE0);
		pci_write_config16(dev, IDE_SDMA_TIM, 0x0200);

		/* Set IDE I/O Configuration */
		reg32 = SIG_MODE_PRI_NORMAL | FAST_PCB1 | FAST_PCB0 | PCB1 | PCB0;
		pci_write_config32(dev, IDE_CONFIG, reg32);

		/* Combine IDE - SATA configuration */
		pci_write_config8(dev, 0x90, 0x02);

		/* Port 0 & 1 enable */
		pci_write_config8(dev, 0x92, 0x0f);

		/* SATA Initialization register */
		pci_write_config32(dev, 0x94, 0x5a000180);
	} else if(config->sata_ahci) {
		printk(BIOS_DEBUG, "SATA controller in AHCI mode.\n");
		/* Allow both Legacy and Native mode */
		pci_write_config8(dev, 0x09, 0x8f);

		/* Set Interrupt Line */
		/* Interrupt Pin is set by D31IP.PIP */
		pci_write_config8(dev, INTR_LN, 0x0a);

		/* Set timings */
		pci_write_config16(dev, IDE_TIM_PRI, IDE_DECODE_ENABLE |
				IDE_ISP_3_CLOCKS | IDE_RCT_1_CLOCKS |
				IDE_PPE0 | IDE_IE0 | IDE_TIME0);
		pci_write_config16(dev, IDE_TIM_SEC, IDE_DECODE_ENABLE |
				IDE_ISP_5_CLOCKS | IDE_RCT_4_CLOCKS);

		/* Sync DMA */
		pci_write_config16(dev, IDE_SDMA_CNT, IDE_PSDE0);
		pci_write_config16(dev, IDE_SDMA_TIM, 0x0001);

		/* Set IDE I/O Configuration */
		reg32 = SIG_MODE_PRI_NORMAL | FAST_PCB1 | FAST_PCB0 | PCB1 | PCB0;
		pci_write_config32(dev, IDE_CONFIG, reg32);

		/* Set Sata Controller Mode. */
		pci_write_config8(dev, 0x90, 0x40); // 40=AHCI

		/* In ACHI mode, bit[3:0] must always be set
		 * (Port status is controlled through AHCI BAR)
		 */
		pci_write_config8(dev, 0x92, 0x0f);

		ahci_bar = (u32 *)(pci_read_config32(dev, 0x27) & ~0x3ff);
		ahci_bar[3] = config->sata_ports_implemented;

		/* SATA Initialization register */
		pci_write_config32(dev, 0x94, 0x1a000180);
	} else {
		printk(BIOS_DEBUG, "SATA controller in plain mode.\n");
		/* Set Sata Controller Mode. No Mapping(?) */
		pci_write_config8(dev, 0x90, 0x00);

		/* No AHCI: clear AHCI base */
		pci_write_config32(dev, 0x24, 0x00000000);

		/* And without AHCI BAR no memory decoding */
		reg16 = pci_read_config16(dev, PCI_COMMAND);
		reg16 &= ~PCI_COMMAND_MEMORY;
		pci_write_config16(dev, PCI_COMMAND, reg16);

		/* Native mode capable on both primary and secondary (0xa)
		 * or'ed with enabled (0x50) = 0xf
		 */
		pci_write_config8(dev, 0x09, 0x8f);

		/* Set Interrupt Line */
		/* Interrupt Pin is set by D31IP.PIP */
		pci_write_config8(dev, INTR_LN, 0xff);

		/* Set timings */
		pci_write_config16(dev, IDE_TIM_PRI, IDE_DECODE_ENABLE |
				IDE_ISP_3_CLOCKS | IDE_RCT_1_CLOCKS |
				IDE_PPE0 | IDE_IE0 | IDE_TIME0);
		pci_write_config16(dev, IDE_TIM_SEC, IDE_DECODE_ENABLE |
				IDE_SITRE | IDE_ISP_3_CLOCKS |
				IDE_RCT_1_CLOCKS | IDE_IE0 | IDE_TIME0);

		/* Sync DMA */
		pci_write_config16(dev, IDE_SDMA_CNT, IDE_SSDE0 | IDE_PSDE0);
		pci_write_config16(dev, IDE_SDMA_TIM, 0x0201);

		/* Set IDE I/O Configuration */
		reg32 = SIG_MODE_PRI_NORMAL | FAST_PCB1 | FAST_PCB0 | PCB1 | PCB0;
		pci_write_config32(dev, IDE_CONFIG, reg32);

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

	/* Sata Initialization Register */
	reg32 = pci_read_config32(dev, 0x94);
	reg32 |= (1 << 30); // due to some bug
	pci_write_config32(dev, 0x94, reg32);
}

static void sata_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations sata_pci_ops = {
	.set_subsystem    = sata_set_subsystem,
};

static struct device_operations sata_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= sata_init,
	.scan_bus		= 0,
	.enable			= i82801gx_enable,
	.ops_pci		= &sata_pci_ops,
};

static const unsigned short sata_ids[] = {
	0x27c0, /* Desktop Non-AHCI and Non-RAID Mode: 82801GB/GR/GDH (ICH7/ICH7R/ICH7DH) */
	0x27c4, /* Mobile Non-AHCI and Non-RAID Mode: 82801GBM/GHM (ICH7-M/ICH7-M DH) */
	/* NOTE: Any of the below are not properly supported yet. */
	0x27c1, /* Desktop AHCI Mode: 82801GB/GR/GDH (ICH7/ICH7R/ICH7DH) */
	0x27c3, /* Desktop RAID mode: 82801GB/GR/GDH (ICH7/ICH7R/ICH7DH) */
	0x27c5, /* Mobile AHCI Mode: 82801GBM/GHM (ICH7-M/ICH7-M DH) */
	0x27c6, /* ICH7M DH Raid Mode: 82801GHM (ICH7-M DH) */
	0
};

static const struct pci_driver i82801gx_sata_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.devices= sata_ids,
};
