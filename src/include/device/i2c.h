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

#ifndef _DEVICE_I2C_H_
#define _DEVICE_I2C_H_

#include <stdint.h>
#include <stdlib.h>

enum i2c_speed {
	I2C_SPEED_STANDARD	= 100000,
	I2C_SPEED_FAST		= 400000,
	I2C_SPEED_FAST_PLUS	= 1000000,
	I2C_SPEED_HIGH		= 3400000,
	I2C_SPEED_FAST_ULTRA	= 5000000,
};

enum i2c_address_mode {
	I2C_MODE_7_BIT,
	I2C_MODE_10_BIT
};

struct i2c_seg
{
	int read;
	uint8_t chip;
	uint8_t *buf;
	int len;
};

int platform_i2c_transfer(unsigned int bus, struct i2c_seg *segments,
	int count);

#define SOFTWARE_I2C_MAX_BUS 10		/* increase as necessary */

struct software_i2c_ops {
	void (*set_sda)(unsigned int bus, int high);
	void (*set_scl)(unsigned int bus, int high);
	int (*get_sda)(unsigned int bus);
	int (*get_scl)(unsigned int bus);
};

extern struct software_i2c_ops *software_i2c[];

int software_i2c_transfer(unsigned int bus, struct i2c_seg *segments,
	int count);
void software_i2c_wedge_ack(unsigned int bus, u8 chip);
void software_i2c_wedge_read(unsigned int bus, u8 chip, u8 reg, int bit_count);
void software_i2c_wedge_write(unsigned int bus, u8 chip, u8 reg, int bit_count);

int i2c_read_field(unsigned int bus, uint8_t chip, uint8_t reg, uint8_t *data,
		   uint8_t mask, uint8_t shift);
int i2c_write_field(unsigned int bus, uint8_t chip, uint8_t reg, uint8_t data,
		    uint8_t mask, uint8_t shift);

/*
 * software_i2c is supposed to be a debug feature. It's usually not compiled in,
 * but when it is it can be dynamically enabled at runtime for certain busses.
 * Need this ugly stub to arbitrate since I2C device drivers hardcode
 * 'i2c_transfer()' as their entry point.
 */
static inline int i2c_transfer(unsigned int bus, struct i2c_seg *segments,
			       int count)
{
	if (CONFIG_SOFTWARE_I2C)
		if (bus < SOFTWARE_I2C_MAX_BUS && software_i2c[bus])
			return software_i2c_transfer(bus, segments, count);

	return platform_i2c_transfer(bus, segments, count);
}

/*
 * Read a raw chunk of data in one segment and one frame.
 *
 * [start][slave addr][r][data][stop]
 */
static inline int i2c_read_raw(unsigned int bus, uint8_t chip, uint8_t *data,
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
static inline int i2c_write_raw(unsigned int bus, uint8_t chip, uint8_t *data,
				int len)
{
	struct i2c_seg seg =
		{ .read = 0, .chip = chip, .buf = data, .len = len };
	return i2c_transfer(bus, &seg, 1);
}

/**
 * Read multi-bytes with two segments in one frame
 *
 * [start][slave addr][w][register addr][start][slave addr][r][data...][stop]
 */
static inline int i2c_read_bytes(unsigned int bus, uint8_t chip, uint8_t reg,
				 uint8_t *data, int len)
{
	struct i2c_seg seg[2];

	seg[0].read = 0;
	seg[0].chip = chip;
	seg[0].buf  = &reg;
	seg[0].len  = 1;
	seg[1].read = 1;
	seg[1].chip = chip;
	seg[1].buf  = data;
	seg[1].len  = len;

	return i2c_transfer(bus, seg, ARRAY_SIZE(seg));
}

/**
 * Read a byte with two segments in one frame
 *
 * [start][slave addr][w][register addr][start][slave addr][r][data][stop]
 */
static inline int i2c_readb(unsigned int bus, uint8_t chip, uint8_t reg,
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
static inline int i2c_writeb(unsigned int bus, uint8_t chip, uint8_t reg,
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

/* I2C bus operation for ramstage drivers */
struct device;
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
int i2c_dev_transfer(struct device *dev, struct i2c_seg *segments, int count);
int i2c_dev_readb(struct device *dev, uint8_t reg, uint8_t *data);
int i2c_dev_writeb(struct device *dev, uint8_t reg, uint8_t data);
int i2c_dev_read_bytes(struct device *dev, uint8_t reg, uint8_t *data, int len);
int i2c_dev_read_raw(struct device *dev, uint8_t *data, int len);
int i2c_dev_write_raw(struct device *dev, uint8_t *data, int len);

#endif	/* _DEVICE_I2C_H_ */
