/*
 * This file is part of the coreboot project.
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

#include <device/i2c_simple.h>
#include <stdint.h>

int i2c_read_field(unsigned int bus, uint8_t chip, uint8_t reg, uint8_t *data,
		   uint8_t mask, uint8_t shift)
{
	int ret;
	uint8_t buf = 0;

	ret = i2c_readb(bus, chip, reg, &buf);

	buf &= (mask << shift);
	*data = (buf >> shift);

	return ret;
}

int i2c_write_field(unsigned int bus, uint8_t chip, uint8_t reg, uint8_t data,
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
