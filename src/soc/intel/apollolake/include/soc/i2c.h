/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_APOLLOLAKE_I2C_H_
#define _SOC_APOLLOLAKE_I2C_H_

#include <device/pci_def.h>
#include <soc/pci_devs.h>

/* I2C Controller Reset in MMIO private region */
#define I2C_LPSS_REG_RESET		0x204
#define I2C_LPSS_RESET_RELEASE_HC	((1 << 1) | (1 << 0))
#define I2C_LPSS_RESET_RELEASE_IDMA	(1 << 2)

/* Convert I2C bus number to PCI device and function */
static inline int i2c_bus_to_devfn(unsigned int bus)
{
	if (bus >= 0 && bus <= 3)
		return PCI_DEVFN(PCH_DEV_SLOT_SIO1, bus);
	else if (bus >= 4 && bus <= 7)
		return PCI_DEVFN(PCH_DEV_SLOT_SIO2, (bus - 4));
	else
		return -1;
}

/* Convert PCI device and function to I2C bus number */
static inline int i2c_devfn_to_bus(unsigned int devfn)
{
	if (PCI_SLOT(devfn) == PCH_DEV_SLOT_SIO1)
		return PCI_FUNC(devfn);
	else if (PCI_SLOT(devfn) == PCH_DEV_SLOT_SIO2)
		return PCI_FUNC(devfn) + 4;
	else
		return -1;
}

#endif /* _SOC_APOLLOLAKE_I2C_H_ */
