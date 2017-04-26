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
#include <intelblocks/lpss_i2c.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include "chip.h"

const struct lpss_i2c_bus_config *i2c_get_soc_cfg(unsigned int bus,
					const struct device *dev)
{
	const struct soc_intel_skylake_config *config;
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
	return EARLY_I2C_BASE(bus);
}

int i2c_soc_devfn_to_bus(unsigned int devfn)
{
	switch (devfn) {
	case PCH_DEVFN_I2C0: return 0;
	case PCH_DEVFN_I2C1: return 1;
	case PCH_DEVFN_I2C2: return 2;
	case PCH_DEVFN_I2C3: return 3;
	case PCH_DEVFN_I2C4: return 4;
	case PCH_DEVFN_I2C5: return 5;
	}
	return -1;
}

int i2c_soc_bus_to_devfn(unsigned int bus)
{
	switch (bus) {
	case 0: return PCH_DEVFN_I2C0;
	case 1: return PCH_DEVFN_I2C1;
	case 2: return PCH_DEVFN_I2C2;
	case 3: return PCH_DEVFN_I2C3;
	case 4: return PCH_DEVFN_I2C4;
	case 5: return PCH_DEVFN_I2C5;
	}
	return -1;
}
