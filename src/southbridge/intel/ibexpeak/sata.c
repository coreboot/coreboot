/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2013 Vladimir Serbinenko
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
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "pch.h"
#include <pc80/mc146818rtc.h>
#include <acpi/sata.h>

typedef struct southbridge_intel_ibexpeak_config config_t;

static inline u32 sir_read(struct device *dev, int idx)
{
	pci_write_config32(dev, SATA_SIRI, idx);
	return pci_read_config32(dev, SATA_SIRD);
}

static inline void sir_write(struct device *dev, int idx, u32 value)
{
	pci_write_config32(dev, SATA_SIRI, idx);
	pci_write_config32(dev, SATA_SIRD, value);
}

static void sata_init(struct device *dev)
{
	u32 reg32;
	u16 reg16;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;
	u8 sata_mode;

	printk(BIOS_DEBUG, "SATA: Initializing...\n");

	if (config == NULL) {
		printk(BIOS_ERR, "SATA: ERROR: Device not in devicetree.cb!\n");
		return;
	}

	if (get_option(&sata_mode, "sata_mode") != CB_SUCCESS)
		/* Default to AHCI */
		sata_mode = 0;

	/* SATA configuration */

	/* Enable BARs */
	pci_write_config16(dev, PCI_COMMAND, 0x0007);

	if (sata_mode == 0) {
		/* AHCI */
		u32 *abar;

		printk(BIOS_DEBUG, "SATA: Controller in AHCI mode.\n");

		/* Set Interrupt Line */
		/* Interrupt Pin is set by D31IP.PIP */
		pci_write_config8(dev, INTR_LN, 0x0b);

		/* Set timings */
		pci_write_config16(dev, IDE_TIM_PRI, IDE_DECODE_ENABLE |
				   IDE_ISP_5_CLOCKS | IDE_RCT_4_CLOCKS);
		pci_write_config16(dev, IDE_TIM_SEC, IDE_DECODE_ENABLE |
				   IDE_ISP_5_CLOCKS | IDE_RCT_4_CLOCKS);

		/* Sync DMA */
		pci_write_config16(dev, IDE_SDMA_CNT, 0);
		pci_write_config16(dev, IDE_SDMA_TIM, 0);

		/* Set IDE I/O Configuration */
		reg32 = SIG_MODE_PRI_NORMAL;	// | FAST_PCB1 | FAST_PCB0 | PCB1 | PCB0;
		pci_write_config32(dev, IDE_CONFIG, reg32);

		/* for AHCI, Port Enable is managed in memory mapped space */
		reg16 = pci_read_config16(dev, 0x92);
		reg16 &= ~0x3f;	/* 6 ports SKU + ORM */
		reg16 |= 0x8100 | config->sata_port_map;
		pci_write_config16(dev, 0x92, reg16);

		/* SATA Initialization register */
		pci_write_config32(dev, 0x94,
				   ((config->
				     sata_port_map ^ 0x3f) << 24) | 0x183 |
				   0x40000000);
		pci_write_config32(dev, 0x98, 0x00590200);

		/* Initialize AHCI memory-mapped space */
		abar = (u32 *)pci_read_config32(dev, PCI_BASE_ADDRESS_5);
		printk(BIOS_DEBUG, "ABAR: %p\n", abar);
		/* CAP (HBA Capabilities) : enable power management */
		reg32 = read32(abar + 0x00);
		reg32 |= 0x0c006000;	// set PSC+SSC+SALP+SSS
		reg32 &= ~0x00020060;	// clear SXS+EMS+PMS
		/* Set ISS, if available */
		if (config->sata_interface_speed_support) {
			reg32 &= ~0x00f00000;
			reg32 |= (config->sata_interface_speed_support & 0x03)
			    << 20;
		}
		write32(abar + 0x00, reg32);
		/* PI (Ports implemented) */
		write32(abar + 0x03, config->sata_port_map);
		(void)read32(abar + 0x03);	/* Read back 1 */
		(void)read32(abar + 0x03);	/* Read back 2 */
		/* CAP2 (HBA Capabilities Extended) */
		reg32 = read32(abar + 0x09);
		reg32 &= ~0x00000002;
		write32(abar + 0x09, reg32);
		/* VSP (Vendor Specific Register */
		reg32 = read32(abar + 0x28);
		reg32 &= ~0x00000005;
		write32(abar + 0x28, reg32);
	} else {
		/* IDE */
		printk(BIOS_DEBUG, "SATA: Controller in plain mode.\n");

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
				   IDE_ISP_5_CLOCKS | IDE_RCT_4_CLOCKS);
		pci_write_config16(dev, IDE_TIM_SEC, IDE_DECODE_ENABLE |
				   IDE_ISP_5_CLOCKS | IDE_RCT_4_CLOCKS);

		/* Sync DMA */
		pci_write_config16(dev, IDE_SDMA_CNT, 0);
		pci_write_config16(dev, IDE_SDMA_TIM, 0);

		/* Set IDE I/O Configuration */
		reg32 = SIG_MODE_PRI_NORMAL;
		pci_write_config32(dev, IDE_CONFIG, reg32);

		/* Port enable */
		reg16 = pci_read_config16(dev, 0x92);
		reg16 &= ~0x3f;
		reg16 |= config->sata_port_map;
		pci_write_config16(dev, 0x92, reg16);

		/* SATA Initialization register */
		pci_write_config32(dev, 0x94,
				   ((config->
				     sata_port_map ^ 0x3f) << 24) | 0x183);
	}

	/* Set Gen3 Transmitter settings if needed */
	if (config->sata_port0_gen3_tx)
		pch_iobp_update(SATA_IOBP_SP0G3IR, 0,
				config->sata_port0_gen3_tx);

	if (config->sata_port1_gen3_tx)
		pch_iobp_update(SATA_IOBP_SP1G3IR, 0,
				config->sata_port1_gen3_tx);

	/* Additional Programming Requirements */
	sir_write(dev, 0x04, 0x00000000);
	sir_write(dev, 0x28, 0x0a000033);
	reg32 = sir_read(dev, 0x54);
	reg32 &= 0xff000000;
	reg32 |= 0x555555;
	sir_write(dev, 0x54, reg32);
	sir_write(dev, 0x64, 0xcccccccc);
	reg32 = sir_read(dev, 0x68);
	reg32 &= 0xffff0000;
	reg32 |= 0xcccc;
	sir_write(dev, 0x68, reg32);
	reg32 = sir_read(dev, 0x78);
	reg32 &= 0x0000ffff;
	reg32 |= 0x88880000;
	sir_write(dev, 0x78, reg32);
	sir_write(dev, 0x84, 0x001c7000);
	sir_write(dev, 0x88, 0x88888888);
	sir_write(dev, 0xa0, 0x001c7000);
	// a4
	sir_write(dev, 0xc4, 0x0c0c0c0c);
	sir_write(dev, 0xc8, 0x0c0c0c0c);
	sir_write(dev, 0xd4, 0x10000000);

	pch_iobp_update(0xea004001, 0x3fffffff, 0xc0000000);
	pch_iobp_update(0xea00408a, 0xfffffcff, 0x00000100);
}

static void sata_enable(struct device *dev)
{
	/* Get the chip configuration */
	config_t *config = dev->chip_info;
	u16 map = 0;
	u8 sata_mode;

	if (!config)
		return;

	if (get_option(&sata_mode, "sata_mode") != CB_SUCCESS)
		sata_mode = 0;

	/*
	 * Set SATA controller mode early so the resource allocator can
	 * properly assign IO/Memory resources for the controller.
	 */
	if (sata_mode == 0)
		map = 0x0060;

	map |= (config->sata_port_map ^ 0x3f) << 8;

	pci_write_config16(dev, 0x90, map);
}

static void sata_set_subsystem(struct device *dev, unsigned vendor,
			       unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   ((device & 0xffff) << 16) | (vendor &
								0xffff));
	}
}

static void sata_fill_ssdt(struct device *dev)
{
	config_t *config = dev->chip_info;
	generate_sata_ssdt_ports("\\_SB_.PCI0.SATA", config->sata_port_map);
}

static struct pci_operations sata_pci_ops = {
	.set_subsystem = sata_set_subsystem,
};

static struct device_operations sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sata_init,
	.enable = sata_enable,
	.acpi_fill_ssdt_generator = sata_fill_ssdt,
	.scan_bus = 0,
	.ops_pci = &sata_pci_ops,
};

static const unsigned short pci_device_ids[] = { 0x3b28, 0x3b29, 0x3b2e, 0 };

static const struct pci_driver pch_sata __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
