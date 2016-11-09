/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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
#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/device.h>
#include <device/i2c.h>
#include <device/pci_def.h>
#include <soc/intel/common/lpss_i2c.h>
#include <soc/i2c.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include "chip.h"

static int i2c_early_init_bus(unsigned bus)
{
	ROMSTAGE_CONST struct soc_intel_apollolake_config *config;
	ROMSTAGE_CONST struct device *tree_dev;
	pci_devfn_t dev;
	int devfn;
	uintptr_t base;
	uint32_t value;
	void *reg;

	/* Find the PCI device for this bus controller */
	devfn = i2c_bus_to_devfn(bus);
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
	config = tree_dev->chip_info;
	if (!config || !config->i2c[bus].early_init) {
		printk(BIOS_ERR, "I2C%u not enabled for early init\n", bus);
		return -1;
	}

	/* Prepare early base address for access before memory */
	base = PRERAM_I2C_BASE_ADDRESS(bus);
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, base);
	pci_write_config32(dev, PCI_COMMAND,
			   PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

	/* Take device out of reset */
	reg = (void *)(base + I2C_LPSS_REG_RESET);
	value = read32(reg);
	value |= I2C_LPSS_RESET_RELEASE_HC;
	write32(reg, value);

	/* Initialize the controller */
	if (lpss_i2c_init(bus, &config->i2c[bus]) < 0) {
		printk(BIOS_ERR, "I2C%u failed to initialize\n", bus);
		return -1;
	}

	return 0;
}

uintptr_t lpss_i2c_base_address(unsigned bus)
{
	unsigned devfn;
	pci_devfn_t dev;
	uintptr_t base;

	/* Find device+function for this controller */
	devfn = i2c_bus_to_devfn(bus);
	if (devfn < 0)
		return (uintptr_t)NULL;

	/* Form a PCI address for this device */
	dev = PCI_DEV(0, PCI_SLOT(devfn), PCI_FUNC(devfn));

	/* Read the first base address for this device */
	base = ALIGN_DOWN(pci_read_config32(dev, PCI_BASE_ADDRESS_0), 16);

	/* Attempt to initialize bus if base is not set yet */
	if (!base && !i2c_early_init_bus(bus))
		base = ALIGN_DOWN(pci_read_config32(dev, PCI_BASE_ADDRESS_0),
				  16);

	return base;
}
