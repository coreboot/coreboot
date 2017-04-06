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
#include <device/device.h>
#include <device/i2c.h>
#include <device/pci_def.h>
#include <intelblocks/lpss.h>
#include <soc/intel/common/lpss_i2c.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/bootblock.h>
#include "chip.h"

uintptr_t lpss_i2c_base_address(unsigned int bus)
{
	int devfn;
	pci_devfn_t dev;

	/* Find device+function for this controller */
	devfn = i2c_bus_to_devfn(bus);
	if (devfn < 0)
		return 0;

	/* Form a PCI address for this device */
	dev = PCI_DEV(0, PCI_SLOT(devfn), PCI_FUNC(devfn));

	/* Read the first base address for this device */
	return ALIGN_DOWN(pci_read_config32(dev, PCI_BASE_ADDRESS_0), 16);
}

static void i2c_early_init_bus(unsigned int bus)
{
	ROMSTAGE_CONST struct soc_intel_skylake_config *config;
	ROMSTAGE_CONST struct device *tree_dev;
	pci_devfn_t dev;
	int devfn;
	uintptr_t base;

	/* Find the PCI device for this bus controller */
	devfn = i2c_bus_to_devfn(bus);
	if (devfn < 0)
		return;

	/* Look up the controller device in the devicetree */
	dev = PCI_DEV(0, PCI_SLOT(devfn), PCI_FUNC(devfn));
	tree_dev = dev_find_slot(0, devfn);
	if (!tree_dev || !tree_dev->enabled)
		return;

	/* Skip if not enabled for early init */
	config = tree_dev->chip_info;
	if (!config)
		return;
	if (!config->i2c[bus].early_init)
		return;

	/* Prepare early base address for access before memory */
	base = EARLY_I2C_BASE(bus);
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, base);
	pci_write_config32(dev, PCI_COMMAND,
			   PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

	/* Take device out of reset */
	lpss_reset_release(base);

	/* Initialize the controller */
	lpss_i2c_init(bus, &config->i2c[bus]);
}

void i2c_early_init(void)
{
	int bus;

	/* Initialize I2C controllers that are enabled in devicetree */
	for (bus = 0; bus < SKYLAKE_I2C_DEV_MAX; bus++)
		i2c_early_init_bus(bus);
}
