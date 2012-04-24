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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "pch.h"

typedef struct southbridge_intel_bd82x6x_config config_t;

static void sata_init(struct device *dev)
{
	u32 reg32;
	u16 reg16;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	printk(BIOS_DEBUG, "pch_sata: initializing...\n");

	if (config == NULL) {
		printk(BIOS_ERR, "pch_sata: error: device not in devicetree.cb!\n");
		return;
	}

	/* SATA configuration */

	/* Enable BARs */
	pci_write_config16(dev, PCI_COMMAND, 0x0007);

	if (config->ide_legacy_combined) {
		printk(BIOS_DEBUG, "SATA controller in combined mode.\n");
		/* Combine IDE - SATA configuration */
		pci_write_config16(dev, 0x90, 0x0000);

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

		/* Port enable */
		reg16 = pci_read_config16(dev, 0x92);
		reg16 &= ~0x3f;
		reg16 |= config->sata_port_map;
		pci_write_config16(dev, 0x92, reg16);

		/* SATA Initialization register */
		pci_write_config32(dev, 0x94,
			   ((config->sata_port_map ^ 0x3f) << 24) | 0x183);
	} else if(config->sata_ahci) {
		u32 abar;

		printk(BIOS_DEBUG, "SATA controller in AHCI mode.\n");
		/* Set Sata Controller Mode. */
		pci_write_config16(dev, 0x90, 0x0060 |
				   ((config->sata_port_map ^ 0x3f) << 8));

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

		/* for AHCI, Port Enable is managed in memory mapped space */
		reg16 = pci_read_config16(dev, 0x92);
		reg16 &= ~0x3f; /* 6 ports SKU + ORM */
		reg16 |= 0x8000 | config->sata_port_map;
		pci_write_config16(dev, 0x92, reg16);

		/* SATA Initialization register */
		pci_write_config32(dev, 0x94,
			   ((config->sata_port_map ^ 0x3f) << 24) | 0x183);

		/* Initialize AHCI memory-mapped space */
		abar = pci_read_config32(dev, PCI_BASE_ADDRESS_5);
		printk(BIOS_DEBUG, "ABAR: %08X\n", abar);
		/* CAP (HBA Capabilities) : enable power management */
		reg32 = read32(abar + 0x00);
		reg32 |= 0x0c006000;  // set PSC+SSC+SALP+SSS
		reg32 &= ~0x00020060; // clear SXS+EMS+PMS
		write32(abar + 0x00, reg32);
		/* PI (Ports implemented) */
		write32(abar + 0x0c, config->sata_port_map);
		(void) read32(abar + 0x0c); /* Read back 1 */
		(void) read32(abar + 0x0c); /* Read back 2 */
		/* CAP2 (HBA Capabilities Extended)*/
		reg32 = read32(abar + 0x24);
		reg32 &= ~0x00000002;
		write32(abar + 0x24, reg32);
		/* VSP (Vendor Specific Register */
		reg32 = read32(abar + 0xa0);
		reg32 &= ~0x00000005;
		write32(abar + 0xa0, reg32);
	} else {
		printk(BIOS_DEBUG, "SATA controller in plain mode.\n");
		/* Set Sata Controller Mode. No Mapping(?) */
		pci_write_config16(dev, 0x90, 0x0000);

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

		/* Port enable */
		reg16 = pci_read_config16(dev, 0x92);
		reg16 &= ~0x3f;
		reg16 |= config->sata_port_map;
		pci_write_config16(dev, 0x92, reg16);

		/* SATA Initialization register */
		pci_write_config32(dev, 0x94,
			   ((config->sata_port_map ^ 0x3f) << 24) | 0x183);
	}
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
	.ops_pci		= &sata_pci_ops,
};

/* Non-AHCI and Non-RAID Mode */
static const struct pci_driver pch_sata_normal_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1c00,
};
static const struct pci_driver pch_sata_mobile_normal_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1c01,
};

/* AHCI Mode */
static const struct pci_driver pch_sata_ahci_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1c02,
};
static const struct pci_driver pch_sata_mobile_ahci_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1c03,
};

/* PantherPoint */
static const struct pci_driver ppt_sata_desktop_normal_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1e00,
};

static const struct pci_driver ppt_sata_mobile_normal_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1e01,
};

static const struct pci_driver ppt_sata_desktop_ahci_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1e02,
};

static const struct pci_driver ppt_sata_mobile_ahci_driver __pci_driver = {
	.ops	= &sata_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x1e03,
};

