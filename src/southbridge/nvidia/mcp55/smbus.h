/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/smbus_def.h>

#define SMBHSTSTAT	0x1
#define SMBHSTPRTCL	0x0
#define SMBHSTCMD	0x3
#define SMBXMITADD	0x2
#define SMBHSTDAT0	0x4
#define SMBHSTDAT1	0x5

/* Between 1-10 seconds, We should never timeout normally
 * Longer than this is just painful when a timeout condition occurs.
 */
#define SMBUS_TIMEOUT	(100*1000*10)

static inline void smbus_delay(void)
{
	outb(0x80, 0x80);
}

static int smbus_wait_until_done(unsigned smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;
		smbus_delay();

		val = inb(smbus_io_base + SMBHSTSTAT);
		if ((val & 0xff) != 0) {
			return 0;
		}
	} while (--loops);
	return -3;
}
static int do_smbus_recv_byte(unsigned smbus_io_base, unsigned device)
{
	unsigned char global_status_register;
	unsigned char byte;

	/* set the device I'm talking to */
	outb(((device & 0x7f) << 1)|1 , smbus_io_base + SMBXMITADD);
	smbus_delay();

	/* byte data recv */
	outb(0x05, smbus_io_base + SMBHSTPRTCL);
	smbus_delay();

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;
	}

	global_status_register = inb(smbus_io_base + SMBHSTSTAT) & 0x80; /* lose check */

	/* read results of transaction */
	byte = inb(smbus_io_base + SMBHSTCMD);

	if (global_status_register != 0x80) { // lose check, otherwise it should be 0
		return -1;
	}
	return byte;
}
static int do_smbus_send_byte(unsigned smbus_io_base, unsigned device, unsigned char val)
{
	unsigned global_status_register;

	outb(val, smbus_io_base + SMBHSTDAT0);
	smbus_delay();

	/* set the command... */
	outb(val, smbus_io_base + SMBHSTCMD);
	smbus_delay();

	/* set the device I'm talking to */
	outb(((device & 0x7f) << 1) | 0, smbus_io_base + SMBXMITADD);
	smbus_delay();

	/* set up for a byte data write */
	outb(0x04, smbus_io_base + SMBHSTPRTCL);
	smbus_delay();

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;
	}
	global_status_register = inb(smbus_io_base + SMBHSTSTAT) & 0x80; /* lose check */

	if (global_status_register != 0x80) {
		return -1;
	}
	return 0;
}
static int do_smbus_read_byte(unsigned smbus_io_base, unsigned device, unsigned address)
{
	unsigned char global_status_register;
	unsigned char byte;

	/* set the device I'm talking to */
	outb(((device & 0x7f) << 1)|1 , smbus_io_base + SMBXMITADD);
	smbus_delay();
	/* set the command/address... */
	outb(address & 0xff, smbus_io_base + SMBHSTCMD);
	smbus_delay();
	/* byte data read */
	outb(0x07, smbus_io_base + SMBHSTPRTCL);
	smbus_delay();

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;
	}

	global_status_register = inb(smbus_io_base + SMBHSTSTAT) & 0x80; /* lose check */

	/* read results of transaction */
	byte = inb(smbus_io_base + SMBHSTDAT0);

	if (global_status_register != 0x80) { // lose check, otherwise it should be 0
		return -1;
	}
	return byte;
}


static int do_smbus_write_byte(unsigned smbus_io_base, unsigned device, unsigned address, unsigned char val)
{
	unsigned global_status_register;

	outb(val, smbus_io_base + SMBHSTDAT0);
	smbus_delay();

	/* set the device I'm talking to */
	outb(((device & 0x7f) << 1) | 0, smbus_io_base + SMBXMITADD);
	smbus_delay();

	outb(address & 0xff, smbus_io_base + SMBHSTCMD);
	smbus_delay();

	/* set up for a byte data write */
	outb(0x06, smbus_io_base + SMBHSTPRTCL);
	smbus_delay();

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;
	}
	global_status_register = inb(smbus_io_base + SMBHSTSTAT) & 0x80; /* lose check */

	if (global_status_register != 0x80) {
		return -1;
	}
	return 0;
}
