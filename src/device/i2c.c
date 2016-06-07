/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 MediaTek Inc.
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
#include <device/i2c.h>
#include <stdint.h>

#if ENV_RAMSTAGE
/* Return I2C operations for a bus */
static inline const struct i2c_bus_operations *ops_i2c_bus(struct bus *bus)
{
	if (bus && bus->dev && bus->dev->ops)
		return bus->dev->ops->ops_i2c_bus;
	return NULL;
}

int i2c_dev_find_bus(struct device *dev)
{
	const struct i2c_bus_operations *ops;
	struct bus *pbus;

	if (!dev)
		return -1;

	/* Locate parent bus with I2C controller ops */
	pbus = dev->bus;
	while (pbus && pbus->dev && !ops_i2c_bus(pbus))
		if (pbus->dev->bus != pbus)
			pbus = pbus->dev->bus;

	/* Check if this I2C controller ops implements dev_to_bus() */
	ops = ops_i2c_bus(pbus);
	if (!ops || !ops->dev_to_bus)
		return -1;

	/* Use controller ops to determine the bus number */
	return ops->dev_to_bus(pbus->dev);
}

int i2c_dev_transfer(struct device *dev, struct i2c_seg *segments, int count)
{
	int bus = i2c_dev_find_bus(dev);
	if (bus < 0)
		return -1;
	return i2c_transfer(bus, segments, count);
}

int i2c_dev_readb(struct device *dev, uint8_t reg, uint8_t *data)
{
	int bus = i2c_dev_find_bus(dev);
	if (bus < 0)
		return -1;
	return i2c_readb(bus, dev->path.i2c.device, reg, data);
}

int i2c_dev_writeb(struct device *dev, uint8_t reg, uint8_t data)
{
	int bus = i2c_dev_find_bus(dev);
	if (bus < 0)
		return -1;
	return i2c_writeb(bus, dev->path.i2c.device, reg, data);
}

int i2c_dev_read_bytes(struct device *dev, uint8_t reg, uint8_t *data, int len)
{
	int bus = i2c_dev_find_bus(dev);
	if (bus < 0)
		return -1;
	return i2c_read_bytes(bus, dev->path.i2c.device, reg, data, len);
}

int i2c_dev_read_raw(struct device *dev, uint8_t *data, int len)
{
	int bus = i2c_dev_find_bus(dev);
	if (bus < 0)
		return -1;
	return i2c_read_raw(bus, dev->path.i2c.device, data, len);
}

int i2c_dev_write_raw(struct device *dev, uint8_t *data, int len)
{
	int bus = i2c_dev_find_bus(dev);
	if (bus < 0)
		return -1;
	return i2c_write_raw(bus, dev->path.i2c.device, data, len);
}
#endif

int i2c_read_field(unsigned bus, uint8_t chip, uint8_t reg, uint8_t *data,
		   uint8_t mask, uint8_t shift)
{
	int ret;
	uint8_t buf = 0;

	ret = i2c_readb(bus, chip, reg, &buf);

	buf &= (mask << shift);
	*data = (buf >> shift);

	return ret;
}

int i2c_write_field(unsigned bus, uint8_t chip, uint8_t reg, uint8_t data,
		    uint8_t mask, uint8_t shift)
{
	int ret;
	uint8_t buf = 0;

	ret = i2c_readb(bus, chip, reg, &buf);

	buf &= ~(mask << shift);
	buf |= (data << shift);

	ret |= i2c_writeb(bus, chip, reg, buf);

	return ret;
}
