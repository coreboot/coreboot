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

#include <console/console.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <intelblocks/lpss_i2c.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include "chip.h"

const struct lpss_i2c_bus_config *i2c_get_soc_cfg(unsigned int bus,
					const struct device *dev)
{
	const struct soc_intel_apollolake_config *config;
	if (!dev || !dev->chip_info) {
		printk(BIOS_ERR, "%s: Could not find SoC devicetree config!\n",
		       __func__);
		return NULL;
	}

	config = dev->chip_info;

	return &config->i2c[bus];
}

uintptr_t i2c_get_soc_early_base(unsigned int bus)
{
	return PRERAM_I2C_BASE_ADDRESS(bus);
}

/* Convert I2C bus number to PCI device and function */
int i2c_soc_bus_to_devfn(unsigned int bus)
{
	if (bus >= 0 && bus <= 3)
		return PCI_DEVFN(PCH_DEV_SLOT_SIO1, bus);
	else if (bus >= 4 && bus <= 7)
		return PCI_DEVFN(PCH_DEV_SLOT_SIO2, (bus - 4));
	else
		return -1;
}

/* Convert PCI device and function to I2C bus number */
int i2c_soc_devfn_to_bus(unsigned int devfn)
{
	if (PCI_SLOT(devfn) == PCH_DEV_SLOT_SIO1)
		return PCI_FUNC(devfn);
	else if (PCI_SLOT(devfn) == PCH_DEV_SLOT_SIO2)
		return PCI_FUNC(devfn) + 4;
	else
		return -1;
}
