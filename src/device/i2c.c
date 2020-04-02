/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
