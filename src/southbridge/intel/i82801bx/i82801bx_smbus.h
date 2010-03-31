/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Yinghai Lu <yinghailu@gmail.com>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <device/smbus_def.h>

static void smbus_delay(void)
{
	inb(0x80);
}

static int smbus_wait_until_ready(void)
{
	unsigned loops = SMBUS_TIMEOUT;
	unsigned char byte;
	do {
		smbus_delay();
		if (--loops == 0)
			break;
		byte = inb(SMBUS_IO_BASE + SMBHSTSTAT);
	} while (byte & 1);
	return loops ? 0 : -1;
}

static int smbus_wait_until_done(void)
{
	unsigned loops = SMBUS_TIMEOUT;
	unsigned char byte;
	do {
		smbus_delay();
		if (--loops == 0)
			break;
		byte = inb(SMBUS_IO_BASE + SMBHSTSTAT);
	} while ((byte & 1) || (byte & ~((1 << 6) | (1 << 0))) == 0);
	return loops ? 0 : -1;
}

static int smbus_wait_until_blk_done(void)
{
	unsigned loops = SMBUS_TIMEOUT;
	unsigned char byte;
	do {
		smbus_delay();
		if (--loops == 0)
			break;
		byte = inb(SMBUS_IO_BASE + SMBHSTSTAT);
	} while ((byte & (1 << 7)) == 0);
	return loops ? 0 : -1;
}

static int do_smbus_read_byte(unsigned device, unsigned address)
{
	unsigned char global_status_register;
	unsigned char byte;

	if (smbus_wait_until_ready() < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}
	/* Setup transaction */
	/* Disable interrupts */
	outb(inb(SMBUS_IO_BASE + SMBHSTCTL) & (~1), SMBUS_IO_BASE + SMBHSTCTL);
	/* Set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 1, SMBUS_IO_BASE + SMBXMITADD);
	/* Set the command/address... */
	outb(address & 0xff, SMBUS_IO_BASE + SMBHSTCMD);
	/* Set up for a byte data read */
	outb((inb(SMBUS_IO_BASE + SMBHSTCTL) & 0xe3) | (0x2 << 2),
	     (SMBUS_IO_BASE + SMBHSTCTL));
	/* Clear any lingering errors, so the transaction will run */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);

	/* Clear the data byte... */
	outb(0, SMBUS_IO_BASE + SMBHSTDAT0);

	/* Start the command */
	outb((inb(SMBUS_IO_BASE + SMBHSTCTL) | 0x40),
	     SMBUS_IO_BASE + SMBHSTCTL);

	/* Poll for transaction completion */
	if (smbus_wait_until_done() < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}

	global_status_register = inb(SMBUS_IO_BASE + SMBHSTSTAT);

	/* Ignore the "In Use" status... */
	global_status_register &= ~(3 << 5);

	/* Read results of transaction */
	byte = inb(SMBUS_IO_BASE + SMBHSTDAT0);
	if (global_status_register != (1 << 1)) {
		return SMBUS_ERROR;
	}
	return byte;
}

/* This function is neither used nor tested by me (Corey Osgood), the author 
(Yinghai) probably tested/used it on i82801er */
static int do_smbus_write_block(unsigned device, unsigned length, unsigned cmd,
				unsigned data1, unsigned data2)
{
#warning "do_smbus_write_block is commented out"
	print_err("Untested smbus_write_block called\n");
#if 0
	unsigned char global_control_register;
	unsigned char global_status_register;
	unsigned char byte;
	unsigned char stat;
	int i;

	/* Clear the PM timeout flags, SECOND_TO_STS */
	outw(inw(0x0400 + 0x66), 0x0400 + 0x66);

	if (smbus_wait_until_ready(SMBUS_IO_BASE) < 0) {
		return -2;
	}

	/* Setup transaction */
	/* Obtain ownership */
	outb(inb(SMBUS_IO_BASE + SMBHSTSTAT), SMBUS_IO_BASE + SMBHSTSTAT);
	for (stat = 0; (stat & 0x40) == 0;) {
		stat = inb(SMBUS_IO_BASE + SMBHSTSTAT);
	}
	/* Clear the done bit */
	outb(0x80, SMBUS_IO_BASE + SMBHSTSTAT);
	/* Disable interrupts */
	outb(inb(SMBUS_IO_BASE + SMBHSTCTL) & (~1), SMBUS_IO_BASE + SMBHSTCTL);

	/* Set the device I'm talking too */
	outb(((device & 0x7f) << 1), SMBUS_IO_BASE + SMBXMITADD);

	/* Set the command address */
	outb(cmd & 0xff, SMBUS_IO_BASE + SMBHSTCMD);

	/* Set the block length */
	outb(length & 0xff, SMBUS_IO_BASE + SMBHSTDAT0);

	/* Try sending out the first byte of data here */
	byte = (data1 >> (0)) & 0x0ff;
	outb(byte, SMBUS_IO_BASE + SMBBLKDAT);
	/* Issue a block write command */
	outb((inb(SMBUS_IO_BASE + SMBHSTCTL) & 0xe3) | (0x5 << 2) | 0x40,
	     SMBUS_IO_BASE + SMBHSTCTL);

	for (i = 0; i < length; i++) {

		/* Poll for transaction completion */
		if (smbus_wait_until_blk_done(SMBUS_IO_BASE) < 0) {
			return -3;
		}

		/* Load the next byte */
		if (i > 3)
			byte = (data2 >> (i % 4)) & 0x0ff;
		else
			byte = (data1 >> (i)) & 0x0ff;
		outb(byte, SMBUS_IO_BASE + SMBBLKDAT);

		/* Clear the done bit */
		outb(inb(SMBUS_IO_BASE + SMBHSTSTAT),
		     SMBUS_IO_BASE + SMBHSTSTAT);
	}

	print_debug("SMBUS Block complete\n");
	return 0;
#endif
}
