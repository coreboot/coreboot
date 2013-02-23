/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Yinghai Lu <yinghailu@gmail.com>
 * Copyright (C) 2009 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <device/smbus_def.h>
#include "i82801gx.h"

static void smbus_delay(void)
{
	inb(0x80);
}

static int smbus_wait_until_ready(u16 smbus_base)
{
	unsigned loops = SMBUS_TIMEOUT;
	unsigned char byte;
	do {
		smbus_delay();
		if (--loops == 0)
			break;
		byte = inb(smbus_base + SMBHSTSTAT);
	} while (byte & 1);
	return loops ? 0 : -1;
}

static int smbus_wait_until_done(u16 smbus_base)
{
	unsigned loops = SMBUS_TIMEOUT;
	unsigned char byte;
	do {
		smbus_delay();
		if (--loops == 0)
			break;
		byte = inb(smbus_base + SMBHSTSTAT);
	} while ((byte & 1) || (byte & ~((1 << 6) | (1 << 0))) == 0);
	return loops ? 0 : -1;
}

static int do_smbus_read_byte(unsigned smbus_base, unsigned device, unsigned address)
{
	unsigned char global_status_register;
	unsigned char byte;

	if (smbus_wait_until_ready(smbus_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}
	/* Setup transaction */
	/* Disable interrupts */
	outb(inb(smbus_base + SMBHSTCTL) & (~1), smbus_base + SMBHSTCTL);
	/* Set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 1, smbus_base + SMBXMITADD);
	/* Set the command/address... */
	outb(address & 0xff, smbus_base + SMBHSTCMD);
	/* Set up for a byte data read */
	outb((inb(smbus_base + SMBHSTCTL) & 0xe3) | (0x2 << 2),
	     (smbus_base + SMBHSTCTL));
	/* Clear any lingering errors, so the transaction will run */
	outb(inb(smbus_base + SMBHSTSTAT), smbus_base + SMBHSTSTAT);

	/* Clear the data byte... */
	outb(0, smbus_base + SMBHSTDAT0);

	/* Start the command */
	outb((inb(smbus_base + SMBHSTCTL) | 0x40),
	     smbus_base + SMBHSTCTL);

	/* Poll for transaction completion */
	if (smbus_wait_until_done(smbus_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}

	global_status_register = inb(smbus_base + SMBHSTSTAT);

	/* Ignore the "In Use" status... */
	global_status_register &= ~(3 << 5);

	/* Read results of transaction */
	byte = inb(smbus_base + SMBHSTDAT0);
	if (global_status_register != (1 << 1)) {
		return SMBUS_ERROR;
	}
	return byte;
}

