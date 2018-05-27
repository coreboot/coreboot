/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 - 2009 coresystems GmbH
 * Copyright (C) 2014 - 2017 Intel Corporation.
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
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include <soc/pci_devs.h>
#include <soc/ramstage.h>
#include <soc/sata.h>

#include "chip.h"

static void sata_init(struct device *dev)
{
	u32 reg32;
	u16 reg16;
	u32 abar;

	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	printk(BIOS_DEBUG, "SATA: Initializing...\n");

	if (config == NULL) {
		printk(BIOS_ERR, "SATA: ERROR: Device not in devicetree.cb!\n");
		return;
	}

	/* SATA configuration is handled by the FSP */

	/* Enable BARs */
	pci_write_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER |
						     PCI_COMMAND_MEMORY |
						     PCI_COMMAND_IO);

	printk(BIOS_DEBUG, "SATA: Controller in AHCI mode.\n");

	/* Set the controller mode */
	reg16 = pci_read_config16(dev, SATA_MAP);
	reg16 &= ~(3 << 6);
	reg16 |= SATA_MAP_AHCI;
	pci_write_config16(dev, SATA_MAP, reg16);

	/* Initialize AHCI memory-mapped space */
	abar = pci_read_config32(dev, PCI_BASE_ADDRESS_5);
	printk(BIOS_DEBUG, "ABAR: %08X\n", abar);

	/* Enable AHCI Mode */
	reg32 = read32((void *)(abar + 0x04));
	reg32 |= (1 << 31);
	write32((void *)(abar + 0x04), reg32);
}

static void sata_enable(struct device *dev) { /* TODO */ }

static struct device_operations sata_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = sata_init,
	.enable = sata_enable,
	.scan_bus = 0,
	.ops_pci = &soc_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	AHCI_DEVID,  /* DVN SATA AHCI */
	AHCI2_DEVID, /* DVN SATA2 AHCI */
	0
};

static const struct pci_driver soc_sata __pci_driver = {
	.ops = &sata_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
