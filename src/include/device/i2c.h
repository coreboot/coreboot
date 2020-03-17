/*
 * This file is part of the coreboot project.
 *
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

#endif	/* _DEVICE_I2C_H_ */
