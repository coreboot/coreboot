/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Google
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

#include <drivers/i2c/designware/dw_i2c.h>
#include <soc/iomap.h>

const uintptr_t i2c_bus_address[] = {	I2C_BASE_ADDRESS,
					I2C_BASE_ADDRESS + I2C_DEVICE_SIZE * 1,
					I2C_BASE_ADDRESS + I2C_DEVICE_SIZE * 2,
					I2C_BASE_ADDRESS + I2C_DEVICE_SIZE * 3,
					};


uintptr_t dw_i2c_base_address(unsigned int bus)
{
	return bus < I2C_DEVICE_COUNT ? i2c_bus_address[bus] : 0;
}
