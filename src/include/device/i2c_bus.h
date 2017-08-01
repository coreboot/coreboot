/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google, Inc.
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

#ifndef _DEVICE_I2C_BUS_H_
#define _DEVICE_I2C_BUS_H_

#include <stdint.h>
#include <device/i2c.h>
#include <device/device.h>

/* I2C bus operation for ramstage drivers */
struct i2c_bus_operations {
	/*
	 * This is an SOC specific method that can be provided to translate the
	 * 'struct device' for an I2C controller into a unique I2C bus number.
	 * Returns -1 if the bus number for this device cannot be determined.
	 */
	int (*dev_to_bus)(struct device *dev);
};

/* Return I2C bus number for provided device, -1 if not found */
int i2c_dev_find_bus(struct device *dev);

/* Variants of I2C helper functions that take a device instead of bus number */
int i2c_dev_transfer(struct device *dev, struct i2c_msg *segments, int count);
int i2c_dev_readb(struct device *dev, uint8_t reg, uint8_t *data);
int i2c_dev_writeb(struct device *dev, uint8_t reg, uint8_t data);
int i2c_dev_read_bytes(struct device *dev, uint8_t reg, uint8_t *data, int len);
int i2c_dev_read_raw(struct device *dev, uint8_t *data, int len);
int i2c_dev_write_raw(struct device *dev, uint8_t *data, int len);

#endif	/* _DEVICE_I2C_BUS_H_ */
