/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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

#include <arch/io.h>
#include <device/smbus_def.h>
#include <timer.h>
#include "smbuslib.h"

static int smbus_wait_till_ready(u16 smbus_base)
{
	struct stopwatch sw;
	unsigned char byte;

	stopwatch_init_msecs_expire(&sw, SMBUS_TIMEOUT);
	do {
		byte = inb(smbus_base + SMBHSTSTAT);
		if (!(byte & 1))
			return 0;
	} while (!stopwatch_expired(&sw));
	return -1;
}

static int smbus_wait_till_done(u16 smbus_base)
{
	struct stopwatch sw;
	unsigned char byte;

	stopwatch_init_msecs_expire(&sw, SMBUS_TIMEOUT);
	do {
		byte = inb(smbus_base + SMBHSTSTAT);
		if (!((byte & 1) || (byte & ~((1 << 6) | (1 << 0))) == 0))
			return 0;
	} while (!stopwatch_expired(&sw));
	return -1;
}

int smbus_read8(unsigned int smbus_base, unsigned int device,
	unsigned int address)
{
	unsigned char global_status_register;
	unsigned char byte;

	if (smbus_wait_till_ready(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;

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
	if (smbus_wait_till_done(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;

	global_status_register = inb(smbus_base + SMBHSTSTAT);

	/* Ignore the "In Use" status... */
	global_status_register &= ~(3 << 5);

	/* Read results of transaction */
	byte = inb(smbus_base + SMBHSTDAT0);
	if (global_status_register != (1 << 1))
		return SMBUS_ERROR;

	return byte;
}

int smbus_write8(unsigned int smbus_base, unsigned int device,
			unsigned int address, unsigned int data)
{
	unsigned char global_status_register;

	if (smbus_wait_till_ready(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;

	/* Setup transaction */
	/* Disable interrupts */
	outb(inb(smbus_base + SMBHSTCTL) & (~1), smbus_base + SMBHSTCTL);
	/* Set the device I'm talking too */
	outb(((device & 0x7f) << 1) & ~0x01, smbus_base + SMBXMITADD);
	/* Set the command/address... */
	outb(address & 0xff, smbus_base + SMBHSTCMD);
	/* Set up for a byte data read */
	outb((inb(smbus_base + SMBHSTCTL) & 0xe3) | (0x2 << 2),
	     (smbus_base + SMBHSTCTL));
	/* Clear any lingering errors, so the transaction will run */
	outb(inb(smbus_base + SMBHSTSTAT), smbus_base + SMBHSTSTAT);

	/* Clear the data byte... */
	outb(data, smbus_base + SMBHSTDAT0);

	/* Start the command */
	outb((inb(smbus_base + SMBHSTCTL) | 0x40),
	     smbus_base + SMBHSTCTL);

	/* Poll for transaction completion */
	if (smbus_wait_till_done(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;

	global_status_register = inb(smbus_base + SMBHSTSTAT);

	/* Ignore the "In Use" status... */
	global_status_register &= ~(3 << 5);

	/* Read results of transaction */
	if (global_status_register != (1 << 1))
		return SMBUS_ERROR;

	return 0;
}
