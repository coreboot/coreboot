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

/**
 * struct i2c_msg - an I2C transaction segment beginning with START
 * @addr: Slave address, either seven or ten bits.  When this is a ten
 *	bit address, I2C_M_TEN must be set in @flags.
 * @flags: I2C_M_RD is handled by all adapters.
 * @len: Number of data bytes in @buf being read from or written to the
 *	I2C slave address.  For read transactions where I2C_M_RECV_LEN
 *	is set, the caller guarantees that this buffer can hold up to
 *	32 bytes in addition to the initial length byte sent by the
 *	slave (plus, if used, the SMBus PEC).
 * @buf: The buffer into which data is read, or from which it's written.
 *
 * An i2c_msg is the low level representation of one segment of an I2C
 * transaction.  It is visible to drivers in the @i2c_transfer() procedure.
 *
 * All I2C adapters implement the standard rules for I2C transactions. Each
 * transaction begins with a START.  That is followed by the slave address,
 * and a bit encoding read versus write.  Then follow all the data bytes,
 * possibly including a byte with SMBus PEC.  The transfer terminates with
 * a NAK, or when all those bytes have been transferred and ACKed.  If this
 * is the last message in a group, it is followed by a STOP.  Otherwise it
 * is followed by the next @i2c_msg transaction segment, beginning with a
 * (repeated) START.
 */
struct i2c_msg {
	uint16_t flags;
#define I2C_M_RD		0x0001	/* read data, from slave to master */
#define I2C_M_TEN		0x0010	/* this is a ten bit chip address */
#define I2C_M_RECV_LEN		0x0400	/* length will be first received byte */
#define I2C_M_NOSTART		0x4000	/* don't send a repeated START */
	uint16_t slave;		/* slave address			*/
	uint16_t len;		/* msg length				*/
	uint8_t *buf;		/* pointer to msg data			*/
};

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
 * but when it is it can be dynamically enabled at runtime for certain busses.
 * Need this ugly stub to arbitrate since I2C device drivers hardcode
 * 'i2c_transfer()' as their entry point.
 */
static inline int i2c_transfer(unsigned int bus, struct i2c_msg *segments,
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
int i2c_dev_transfer(struct device *dev, struct i2c_msg *segments, int count);
int i2c_dev_readb(struct device *dev, uint8_t reg, uint8_t *data);
int i2c_dev_writeb(struct device *dev, uint8_t reg, uint8_t data);
int i2c_dev_read_bytes(struct device *dev, uint8_t reg, uint8_t *data, int len);
int i2c_dev_read_raw(struct device *dev, uint8_t *data, int len);
int i2c_dev_write_raw(struct device *dev, uint8_t *data, int len);

#endif	/* _DEVICE_I2C_H_ */
