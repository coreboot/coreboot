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
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82801gx.h"

typedef struct southbridge_intel_i82801gx_config config_t;

static void ide_init(struct device *dev)
{
	u16 ideTimingConfig;
	u32 reg32;
	u32 enable_primary, enable_secondary;

	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	printk(BIOS_DEBUG, "i82801gx_ide: initializing...");
	if (config == NULL) {
		printk(BIOS_ERR, "\ni82801gx_ide: Not mentioned in devicetree.cb!\n");
		// Trying to set somewhat safe defaults instead of bailing out.
		enable_primary = enable_secondary = 1;
	} else {
		enable_primary = config->ide_enable_primary;
		enable_secondary = config->ide_enable_secondary;
	}

	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND, reg32 | PCI_COMMAND_IO | PCI_COMMAND_MASTER);

	/* Native Capable, but not enabled. */
	pci_write_config8(dev, 0x09, 0x8a);

	ideTimingConfig = pci_read_config16(dev, IDE_TIM_PRI);
	ideTimingConfig &= ~IDE_DECODE_ENABLE;
	ideTimingConfig |= IDE_SITRE;
	if (enable_primary) {
		/* Enable primary IDE interface. */
		ideTimingConfig |= IDE_DECODE_ENABLE;
		ideTimingConfig |= (2 << 12); // ISP = 3 clocks
		ideTimingConfig |= (3 << 8); // RCT = 1 clock
		ideTimingConfig |= (1 << 1); // IE0
		ideTimingConfig |= (1 << 0); // TIME0
		printk(BIOS_DEBUG, " IDE0");
	}
	pci_write_config16(dev, IDE_TIM_PRI, ideTimingConfig);

	ideTimingConfig = pci_read_config16(dev, IDE_TIM_SEC);
	ideTimingConfig &= ~IDE_DECODE_ENABLE;
	ideTimingConfig |= IDE_SITRE;
	if (enable_secondary) {
		/* Enable secondary IDE interface. */
		ideTimingConfig |= IDE_DECODE_ENABLE;
		ideTimingConfig |= (2 << 12); // ISP = 3 clocks
		ideTimingConfig |= (3 << 8); // RCT = 1 clock
		ideTimingConfig |= (1 << 1); // IE0
		ideTimingConfig |= (1 << 0); // TIME0
		printk(BIOS_DEBUG, " IDE1");
	}
	pci_write_config16(dev, IDE_TIM_SEC, ideTimingConfig);

	/* Set IDE I/O Configuration */
	reg32 = 0;
	/* FIXME: only set FAST_* for ata/100, only ?CBx for ata/66 */
	if (enable_primary)
		reg32 |= SIG_MODE_PRI_NORMAL | FAST_PCB0 | PCB0 | FAST_PCB1 | PCB1;
	if (enable_secondary)
		reg32 |= SIG_MODE_SEC_NORMAL | FAST_SCB0 | SCB0 | FAST_SCB1 | SCB1;
	pci_write_config32(dev, IDE_CONFIG, reg32);

	/* Set Interrupt Line */
	/* Interrupt Pin is set by D31IP.PIP */
	pci_write_config32(dev, INTR_LN, 0xff); /* Int 15 */

	printk(BIOS_DEBUG, "\n");
}

static void ide_set_subsystem(struct device *dev, unsigned int vendor,
			      unsigned int device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations ide_pci_ops = {
	.set_subsystem    = ide_set_subsystem,
};

static struct device_operations ide_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ide_init,
	.scan_bus		= 0,
	.enable			= i82801gx_enable,
	.ops_pci		= &ide_pci_ops,
};

/* 82801GB/GR/GDH/GBM/GHM/GU (ICH7/ICH7R/ICH7DH/ICH7-M/ICH7-M DH/ICH7-U) */
static const struct pci_driver i82801gx_ide __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27df,
};
