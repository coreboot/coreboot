/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
 * Copyright (C) 2017-2018 Intel Corporation.
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

#include <device/device.h>
#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/pci_devs.h>

int dw_i2c_soc_devfn_to_bus(unsigned int devfn)
{
	switch (devfn) {
	case PCH_DEVFN_I2C0:
		return 0;
	case PCH_DEVFN_I2C1:
		return 1;
	case PCH_DEVFN_I2C2:
		return 2;
	case PCH_DEVFN_I2C3:
		return 3;
	case PCH_DEVFN_I2C4:
		return 4;
	case PCH_DEVFN_I2C5:
		return 5;
	}
	return -1;
}

int dw_i2c_soc_bus_to_devfn(unsigned int bus)
{
	switch (bus) {
	case 0:
		return PCH_DEVFN_I2C0;
	case 1:
		return PCH_DEVFN_I2C1;
	case 2:
		return PCH_DEVFN_I2C2;
	case 3:
		return PCH_DEVFN_I2C3;
	case 4:
		return PCH_DEVFN_I2C4;
	case 5:
		return PCH_DEVFN_I2C5;
	}
	return -1;
}
