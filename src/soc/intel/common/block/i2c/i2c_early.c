/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
 * Copyright 2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/i2c.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <intelblocks/lpss.h>
#include <intelblocks/lpss_i2c.h>
#include "lpss_i2c.h"

static int lpss_i2c_early_init_bus(unsigned int bus)
{
	const struct lpss_i2c_bus_config *config;
	const struct device *tree_dev;
	pci_devfn_t dev;
	int devfn;
	uintptr_t base;

	/* Find the PCI device for this bus controller */
	devfn = i2c_soc_bus_to_devfn(bus);
	if (devfn < 0) {
		printk(BIOS_ERR, "I2C%u device not found\n", bus);
		return -1;
	}

	/* Look up the controller device in the devicetree */
	dev = PCI_DEV(0, PCI_SLOT(devfn), PCI_FUNC(devfn));
	tree_dev = dev_find_slot(0, devfn);
	if (!tree_dev || !tree_dev->enabled) {
		printk(BIOS_ERR, "I2C%u device not enabled\n", bus);
		return -1;
	}

	/* Skip if not enabled for early init */
	config = i2c_get_soc_cfg(bus, tree_dev);
	if (!config || !config->early_init) {
		printk(BIOS_DEBUG, "I2C%u not enabled for early init\n", bus);
		return -1;
	}

	/* Prepare early base address for access before memory */
	base = i2c_get_soc_early_base(bus);
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, base);
	pci_write_config32(dev, PCI_COMMAND,
			   PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

	/* Take device out of reset */
	lpss_reset_release(base);

	/* Initialize the controller */
	if (lpss_i2c_init(bus, config) < 0) {
		printk(BIOS_ERR, "I2C%u failed to initialize\n", bus);
		return -1;
	}

	return 0;
}

uintptr_t lpss_i2c_base_address(unsigned int bus)
{
	int devfn;
	pci_devfn_t dev;
	uintptr_t base;

	/* Find device+function for this controller */
	devfn = i2c_soc_bus_to_devfn(bus);
	if (devfn < 0)
		return (uintptr_t)NULL;

	/* Form a PCI address for this device */
	dev = PCI_DEV(0, PCI_SLOT(devfn), PCI_FUNC(devfn));

	/* Read the first base address for this device */
	base = ALIGN_DOWN(pci_read_config32(dev, PCI_BASE_ADDRESS_0), 16);

	/* Attempt to initialize bus if base is not set yet */
	if (!base && !lpss_i2c_early_init_bus(bus))
		base = ALIGN_DOWN(pci_read_config32(dev, PCI_BASE_ADDRESS_0),
				  16);
	return base;
}
