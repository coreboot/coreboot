/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
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
#include "soc.h"

typedef struct southbridge_intel_fsp_rangeley_config config_t;

static void sata_init(struct device *dev)
{
	u32 reg32;
	u16 reg16;
	u32 *abar;

	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	printk(BIOS_DEBUG, "SATA: Initializing...\n");

	if (config == NULL) {
		printk(BIOS_ERR, "SATA: ERROR: Device not in devicetree.cb!\n");
		return;
	}

	/* SATA configuration is handled by the FSP */

	/* Enable BARs */
	pci_write_config16(dev, PCI_COMMAND, 0x0007);

	if (config->ide_legacy_combined) {
		printk(BIOS_DEBUG, "SATA: Controller in combined mode.\n");

		/* Set the controller mode */
		reg16 = pci_read_config16(dev, SATA_MAP);
		reg16 &= ~(3 << 6);
		pci_write_config16(dev, SATA_MAP, reg16);

		/* No AHCI: clear AHCI base */
		pci_write_config32(dev, 0x24, 0x00000000);

		/* And without AHCI BAR no memory decoding */
		reg16 = pci_read_config16(dev, PCI_COMMAND);
		reg16 &= ~PCI_COMMAND_MEMORY;
		pci_write_config16(dev, PCI_COMMAND, reg16);
	} else if (config->sata_ahci) {
		printk(BIOS_DEBUG, "SATA: Controller in AHCI mode.\n");

		/* Set the controller mode */
		reg16 = pci_read_config16(dev, SATA_MAP);
		reg16 &= ~(3 << 6);
		reg16 |= (1 << 6);
		pci_write_config16(dev, SATA_MAP, reg16);

		/* Initialize AHCI memory-mapped space */
		abar = (u32 *)pci_read_config32(dev, PCI_BASE_ADDRESS_5);
		printk(BIOS_DEBUG, "ABAR: %p\n", abar);

		/* Enable AHCI Mode */
		reg32 = read32(abar + 0x01);
		reg32 |= (1 << 31);
		write32(abar + 0x01, reg32);
	} else {
		printk(BIOS_DEBUG, "SATA: Controller in plain mode.\n");
	}

	/* Spin up the drives as early as possible via the Port Enable */
	reg16 = pci_read_config16(dev, SATA_PSC);
	reg16 &= ~0x3f;
	pci_write_config16(dev, SATA_PSC, reg16);
	reg16 = pci_read_config16(dev, SATA_PSC);
	reg16 |= 0x3f;
	pci_write_config16(dev, SATA_PSC, reg16);

}

static void sata_enable(struct device *dev)
{
}

static void sata_set_subsystem(struct device *dev, unsigned vendor, unsigned device)
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
	.enable			= sata_enable,
	.scan_bus		= 0,
	.ops_pci		= &sata_pci_ops,
};

static const unsigned short pci_device_ids[] = { 0x1f20, 0x1f21, 0x1f22, 0x1f23,
                                                 0x1f30, 0x1f31, 0x1f32, 0x1f33,
                                                 0 };

static const struct pci_driver soc_sata __pci_driver = {
	.ops	 = &sata_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
