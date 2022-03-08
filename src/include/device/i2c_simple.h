/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DEVICE_I2C_SIMPLE_H_
#define _DEVICE_I2C_SIMPLE_H_

#include <commonlib/helpers.h>
#include <device/i2c.h>
#include <stdint.h>

int platform_i2c_transfer(unsigned int bus, struct i2c_msg *segments,
			  int count);

#define SOFTWARE_I2C_MAX_BUS 10		/* increase as necessary */

struct software_i2c_ops {
	void (*set_sda)(unsigned int bus, int high);
	void (*set_scl)(unsigned int bus, int high);
	int (*get_sda)(unsigned int bus);
	int (*get_scl)(unsigned int bus);
};

extern struct software_i2c_ops *software_i2c[];

int software_i2c_transfer(unsigned int bus, struct i2c_msg *segments,
			  int count);
void software_i2c_wedge_ack(unsigned int bus, u8 slave);
void software_i2c_wedge_read(unsigned int bus, u8 slave, u8 reg, int bit_count);
void software_i2c_wedge_write(unsigned int bus, u8 slave, u8 reg,
			      int bit_count);

int i2c_read_field(unsigned int bus, uint8_t slave, uint8_t reg, uint8_t *data,
		   uint8_t mask, uint8_t shift);
int i2c_write_field(unsigned int bus, uint8_t slave, uint8_t reg, uint8_t data,
		    uint8_t mask, uint8_t shift);

/*
 * software_i2c is supposed to be a debug feature. It's usually not compiled in,
 * but when it is it can be dynamically enabled at runtime for certain buses.
 * Need this ugly stub to arbitrate since I2C device drivers hardcode
 * 'i2c_transfer()' as their entry point.
 */
static inline int i2c_transfer(unsigned int bus, struct i2c_msg *segments,
			       int count)
{
	if (CONFIG(SOFTWARE_I2C))
		if (bus < SOFTWARE_I2C_MAX_BUS && software_i2c[bus])
			return software_i2c_transfer(bus, segments, count);

	return platform_i2c_transfer(bus, segments, count);
}

/*
 * Read a raw chunk of data in one segment and one frame.
 *
 * [start][slave addr][r][data][stop]
 */
static inline int i2c_read_raw(unsigned int bus, uint8_t slave, uint8_t *data,
			       int len)
{
	struct i2c_msg seg = {
		.flags = I2C_M_RD, .slave = slave, .buf = data, .len = len
	};

	return i2c_transfer(bus, &seg, 1);
}

/*
 * Write a raw chunk of data in one segment and one frame.
 *
 * [start][slave addr][w][data][stop]
 */
static inline int i2c_write_raw(unsigned int bus, uint8_t slave, uint8_t *data,
				int len)
{
	struct i2c_msg seg = {
		.flags = 0, .slave = slave, .buf = data, .len = len
	};

	return i2c_transfer(bus, &seg, 1);
}

/**
 * Read multi-bytes with two segments in one frame
 *
 * [start][slave addr][w][register addr][start][slave addr][r][data...][stop]
 */
static inline int i2c_read_bytes(unsigned int bus, uint8_t slave, uint8_t reg,
				 uint8_t *data, int len)
{
	struct i2c_msg seg[2];

	seg[0].flags = 0;
	seg[0].slave = slave;
	seg[0].buf   = &reg;
	seg[0].len   = 1;
	seg[1].flags = I2C_M_RD;
	seg[1].slave = slave;
	seg[1].buf   = data;
	seg[1].len   = len;

	return i2c_transfer(bus, seg, ARRAY_SIZE(seg));
}

/**
 * Read a byte with two segments in one frame
 *
 * [start][slave addr][w][register addr][start][slave addr][r][data][stop]
 */
static inline int i2c_readb(unsigned int bus, uint8_t slave, uint8_t reg,
			    uint8_t *data)
{
	struct i2c_msg seg[2];

	seg[0].flags = 0;
	seg[0].slave = slave;
	seg[0].buf   = &reg;
	seg[0].len   = 1;
	seg[1].flags = I2C_M_RD;
	seg[1].slave = slave;
	seg[1].buf   = data;
	seg[1].len   = 1;

	return i2c_transfer(bus, seg, ARRAY_SIZE(seg));
}

/**
 * Write a byte with one segment in one frame.
 *
 * [start][slave addr][w][register addr][data][stop]
 */
static inline int i2c_writeb(unsigned int bus, uint8_t slave, uint8_t reg,
			     uint8_t data)
{
	struct i2c_msg seg;
	uint8_t buf[] = {reg, data};

	seg.flags = 0;
	seg.slave = slave;
	seg.buf   = buf;
	seg.len   = ARRAY_SIZE(buf);

	return i2c_transfer(bus, &seg, 1);
}

/**
 * Read multi-bytes from an I2C device with two bytes register address/offset
 * with two segments in one frame
 *
 * [start][slave addr][w][register high addr][register low addr]
 * [start][slave addr][r][data...][stop]
 */
static inline int i2c_2ba_read_bytes(unsigned int bus, uint8_t slave, uint16_t offset,
				     uint8_t *data, int len)
{
	struct i2c_msg seg[2];
	uint8_t eeprom_offset[2];

	eeprom_offset[0] = offset >> 8;
	eeprom_offset[1] = offset & 0xff;

	seg[0].flags = 0;
	seg[0].slave = slave;
	seg[0].buf   = eeprom_offset;
	seg[0].len   = sizeof(eeprom_offset);
	seg[1].flags = I2C_M_RD;
	seg[1].slave = slave;
	seg[1].buf   = data;
	seg[1].len   = len;

	return i2c_transfer(bus, seg, ARRAY_SIZE(seg));
}

#endif	/* _DEVICE_I2C_SIMPLE_H_ */
