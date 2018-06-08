/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#include <device/smbus_def.h>

#define SMBHSTSTAT 0x0
#define SMBSLVSTAT 0x1
#define SMBHSTCTRL 0x2
#define SMBHSTCMD  0x3
#define SMBHSTADDR 0x4
#define SMBHSTDAT0 0x5
#define SMBHSTDAT1 0x6
#define SMBHSTBLKDAT 0x7

#define SMBSLVCTRL 0x8
#define SMBSLVCMD_SHADOW 0x9
#define SMBSLVEVT 0xa
#define SMBSLVDAT 0xc


/* Between 1-10 seconds, We should never timeout normally
 * Longer than this is just painful when a timeout condition occurs.
 */
#define SMBUS_TIMEOUT (100*1000*10)

static inline void smbus_delay(void)
{
	outb(0x80, 0x80);
}

static int smbus_wait_until_ready(unsigned smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;
		val = inb(smbus_io_base + SMBHSTSTAT);
		val &= 0x1f;
		if (val == 0) { // ready now
			return 0;
		}
		outb(val, smbus_io_base + SMBHSTSTAT);
	} while (--loops);
	return -2; // time out
}

static int smbus_wait_until_done(unsigned smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;

		val = inb(smbus_io_base + SMBHSTSTAT);
		val &= 0x1f; // mask off reserved bits
		if (val & 0x1c) {
			return -5; // error
		}
		if (val == 0x02) {
			outb(val, smbus_io_base + SMBHSTSTAT); // clear status
			return 0; //
		}
	} while (--loops);
	return -3; // timeout
}

static int do_smbus_recv_byte(unsigned smbus_io_base, unsigned device)
{
	uint8_t byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return -2; // not ready
	}

	/* set the device I'm talking to */
	outb(((device & 0x7f) << 1)|1 , smbus_io_base + SMBHSTADDR);

	byte = inb(smbus_io_base + SMBHSTCTRL);
	byte &= 0xe3; // Clear [4:2]
	byte |= (1<<2) | (1<<6); // Byte data read/write command, start the command
	outb(byte, smbus_io_base + SMBHSTCTRL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3; // timeout or error
	}

	/* read results of transaction */
	byte = inb(smbus_io_base + SMBHSTCMD);

	return byte;
}

static int do_smbus_send_byte(unsigned smbus_io_base, unsigned device, unsigned char val)
{
	uint8_t byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return -2; // not ready
	}

	/* set the command... */
	outb(val, smbus_io_base + SMBHSTCMD);

	/* set the device I'm talking to */
	outb(((device & 0x7f) << 1)|0 , smbus_io_base + SMBHSTADDR);

	byte = inb(smbus_io_base + SMBHSTCTRL);
	byte &= 0xe3; // Clear [4:2]
	byte |= (1<<2) | (1<<6); // Byte data read/write command, start the command
	outb(byte, smbus_io_base + SMBHSTCTRL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3; // timeout or error
	}

	return 0;
}

static int do_smbus_read_byte(unsigned smbus_io_base, unsigned device, unsigned address)
{
	uint8_t byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return -2; // not ready
	}

	/* set the command/address... */
	outb(address & 0xff, smbus_io_base + SMBHSTCMD);

	/* set the device I'm talking to */
	outb(((device & 0x7f) << 1)|1 , smbus_io_base + SMBHSTADDR);

	byte = inb(smbus_io_base + SMBHSTCTRL);
	byte &= 0xe3; // Clear [4:2]
	byte |= (1<<3) | (1<<6); // Byte data read/write command, start the command
	outb(byte, smbus_io_base + SMBHSTCTRL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3; // timeout or error
	}

	/* read results of transaction */
	byte = inb(smbus_io_base + SMBHSTDAT0);

	return byte;
}

static int do_smbus_write_byte(unsigned smbus_io_base, unsigned device, unsigned address, unsigned char val)
{
	uint8_t byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return -2; // not ready
	}

	/* set the command/address... */
	outb(address & 0xff, smbus_io_base + SMBHSTCMD);

	/* set the device I'm talking to */
	outb(((device & 0x7f) << 1)|0 , smbus_io_base + SMBHSTADDR);

	/* output value */
	outb(val, smbus_io_base + SMBHSTDAT0);

	byte = inb(smbus_io_base + SMBHSTCTRL);
	byte &= 0xe3; // Clear [4:2]
	byte |= (1<<3) | (1<<6); // Byte data read/write command, start the command
	outb(byte, smbus_io_base + SMBHSTCTRL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3; // timeout or error
	}

	return 0;
}
