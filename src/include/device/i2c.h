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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#ifndef _DEVICE_I2C_H_
#define _DEVICE_I2C_H_

#include <stdint.h>
#include <stdlib.h>

struct i2c_seg
{
	int read;
	uint8_t chip;
	uint8_t *buf;
	int len;
};

int i2c_transfer(unsigned bus, struct i2c_seg *segments, int count);

/*
 * Read a raw chunk of data in one segment and one frame.
 *
 * [start][slave addr][r][data][stop]
 */
static inline int i2c_read_raw(unsigned bus, uint8_t chip, uint8_t *data,
			       int len)
{
	struct i2c_seg seg =
		{ .read = 1, .chip = chip, .buf = data, .len = len };
	return i2c_transfer(bus, &seg, 1);
}

/*
 * Write a raw chunk of data in one segment and one frame.
 *
 * [start][slave addr][w][data][stop]
 */
static inline int i2c_write_raw(unsigned bus, uint8_t chip, uint8_t *data,
			        int len)
{
	struct i2c_seg seg =
		{ .read = 0, .chip = chip, .buf = data, .len = len };
	return i2c_transfer(bus, &seg, 1);
}

/**
 * Read a byte with two segments in one frame
 *
 * [start][slave addr][w][register addr][start][slave addr][r][data][stop]
 */
static inline int i2c_readb(unsigned bus, uint8_t chip, uint8_t reg,
			    uint8_t *data)
{
	struct i2c_seg seg[2];

	seg[0].read = 0;
	seg[0].chip = chip;
	seg[0].buf = &reg;
	seg[0].len = 1;
	seg[1].read = 1;
	seg[1].chip = chip;
	seg[1].buf = data;
	seg[1].len = 1;

	return i2c_transfer(bus, seg, ARRAY_SIZE(seg));
}

/**
 * Write a byte with one segment in one frame.
 *
 * [start][slave addr][w][register addr][data][stop]
 */
static inline int i2c_writeb(unsigned bus, uint8_t chip, uint8_t reg,
			     uint8_t data)
{
	struct i2c_seg seg;
	uint8_t buf[] = {reg, data};

	seg.read = 0;
	seg.chip = chip;
	seg.buf = buf;
	seg.len = ARRAY_SIZE(buf);

	return i2c_transfer(bus, &seg, 1);
}

#endif	/* _DEVICE_I2C_H_ */
