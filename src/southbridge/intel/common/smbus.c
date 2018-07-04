/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Yinghai Lu <yinghailu@gmail.com>
 * Copyright (C) 2009 coresystems GmbH
 * Copyright (C) 2013 Vladimir Serbinenko
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
#include <console/console.h>
#include <device/smbus_def.h>
#include <stdlib.h>
#include "smbus.h"


#if IS_ENABLED(CONFIG_DEBUG_SMBUS)
#define dprintk(args...) printk(BIOS_DEBUG, ##args)
#else
#define dprintk(args...) do {} while (0)
#endif

/* I801 command constants */
#define I801_QUICK		(0 << 2)
#define I801_BYTE		(1 << 2)
#define I801_BYTE_DATA		(2 << 2)
#define I801_WORD_DATA		(3 << 2)
#define I801_BLOCK_DATA		(5 << 2)
#define I801_I2C_BLOCK_DATA	(6 << 2) /* ICH5 and later */

/* I801 Host Control register bits */
#define SMBHSTCNT_INTREN	(1 << 0)
#define SMBHSTCNT_KILL		(1 << 1)
#define SMBHSTCNT_LAST_BYTE	(1 << 5)
#define SMBHSTCNT_START		(1 << 6)
#define SMBHSTCNT_PEC_EN	(1 << 7) /* ICH3 and later */

/* I801 Hosts Status register bits */
#define SMBHSTSTS_BYTE_DONE	(1 << 7)
#define SMBHSTSTS_INUSE_STS	(1 << 6)
#define SMBHSTSTS_SMBALERT_STS	(1 << 5)
#define SMBHSTSTS_FAILED	(1 << 4)
#define SMBHSTSTS_BUS_ERR	(1 << 3)
#define SMBHSTSTS_DEV_ERR	(1 << 2)
#define SMBHSTSTS_INTR		(1 << 1)
#define SMBHSTSTS_HOST_BUSY	(1 << 0)

#define SMBUS_TIMEOUT		(10 * 1000 * 100)
#define SMBUS_BLOCK_MAXLEN	32

static void smbus_delay(void)
{
	inb(0x80);
}

static int smbus_wait_until_ready(u16 smbus_base)
{
	unsigned int loops = SMBUS_TIMEOUT;
	unsigned char byte;
	do {
		smbus_delay();
		if (--loops == 0)
			break;
		byte = inb(smbus_base + SMBHSTSTAT);
	} while (byte & SMBHSTSTS_HOST_BUSY);
	return loops ? 0 : -1;
}

static int smbus_wait_until_done(u16 smbus_base)
{
	unsigned int loops = SMBUS_TIMEOUT;
	unsigned char byte;
	do {
		smbus_delay();
		if (--loops == 0)
			break;
		byte = inb(smbus_base + SMBHSTSTAT);
	} while ((byte & SMBHSTSTS_HOST_BUSY)
		|| (byte & ~(SMBHSTSTS_INUSE_STS | SMBHSTSTS_HOST_BUSY)) == 0);
	return loops ? 0 : -1;
}

static int smbus_wait_until_active(u16 smbus_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned char val;
		smbus_delay();
		val = inb(smbus_base + SMBHSTSTAT);
		if ((val & SMBHSTSTS_HOST_BUSY)) {
			break;
		}
	} while (--loops);
	return loops ? 0 : -1;
}

int do_smbus_read_byte(unsigned int smbus_base, u8 device,
		unsigned int address)
{
	unsigned char status;
	unsigned char byte;

	if (smbus_wait_until_ready(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	/* Set up transaction */
	/* Disable interrupts */
	outb(inb(smbus_base + SMBHSTCTL) & ~SMBHSTCNT_INTREN,
		smbus_base + SMBHSTCTL);
	/* Set the device I'm talking to */
	outb(((device & 0x7f) << 1) | 1, smbus_base + SMBXMITADD);
	/* Set the command/address... */
	outb(address & 0xff, smbus_base + SMBHSTCMD);
	/* Set up for a byte data read */
	outb((inb(smbus_base + SMBHSTCTL) & 0xe3) | I801_BYTE_DATA,
	     (smbus_base + SMBHSTCTL));
	/* Clear any lingering errors, so the transaction will run */
	outb(inb(smbus_base + SMBHSTSTAT), smbus_base + SMBHSTSTAT);

	/* Clear the data byte... */
	outb(0, smbus_base + SMBHSTDAT0);

	/* Start the command */
	outb((inb(smbus_base + SMBHSTCTL) | SMBHSTCNT_START),
	     smbus_base + SMBHSTCTL);

	/* poll for it to start */
	if (smbus_wait_until_active(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_ACTIVE_TIMEOUT;

	/* Poll for transaction completion */
	if (smbus_wait_until_done(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;

	status = inb(smbus_base + SMBHSTSTAT);

	/* Ignore the "In Use" status... */
	status &= ~(SMBHSTSTS_SMBALERT_STS | SMBHSTSTS_INUSE_STS);

	/* Read results of transaction */
	byte = inb(smbus_base + SMBHSTDAT0);
	if (status != SMBHSTSTS_INTR)
		return SMBUS_ERROR;
	return byte;
}

int do_smbus_write_byte(unsigned int smbus_base, u8 device,
			unsigned int address, unsigned int data)
{
	unsigned char status;

	if (smbus_wait_until_ready(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;

	/* Set up transaction */
	/* Disable interrupts */
	outb(inb(smbus_base + SMBHSTCTL) & ~SMBHSTCNT_INTREN,
		smbus_base + SMBHSTCTL);
	/* Set the device I'm talking to */
	outb(((device & 0x7f) << 1) & ~0x01, smbus_base + SMBXMITADD);
	/* Set the command/address... */
	outb(address & 0xff, smbus_base + SMBHSTCMD);
	/* Set up for a byte data read */
	outb((inb(smbus_base + SMBHSTCTL) & 0xe3) | I801_BYTE_DATA,
	     (smbus_base + SMBHSTCTL));
	/* Clear any lingering errors, so the transaction will run */
	outb(inb(smbus_base + SMBHSTSTAT), smbus_base + SMBHSTSTAT);

	/* Clear the data byte... */
	outb(data, smbus_base + SMBHSTDAT0);

	/* Start the command */
	outb((inb(smbus_base + SMBHSTCTL) | SMBHSTCNT_START),
	     smbus_base + SMBHSTCTL);

	/* poll for it to start */
	if (smbus_wait_until_active(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_ACTIVE_TIMEOUT;

	/* Poll for transaction completion */
	if (smbus_wait_until_done(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;

	status = inb(smbus_base + SMBHSTSTAT);

	/* Ignore the "In Use" status... */
	status &= ~(SMBHSTSTS_SMBALERT_STS | SMBHSTSTS_INUSE_STS);

	/* Read results of transaction */
	if (status != SMBHSTSTS_INTR)
		return SMBUS_ERROR;

	return 0;
}

int do_smbus_block_read(unsigned int smbus_base, u8 device, u8 cmd,
			unsigned int max_bytes, u8 *buf)
{
	u8 status;
	int slave_bytes;
	int bytes_read = 0;
	unsigned int loops = SMBUS_TIMEOUT;
	if (smbus_wait_until_ready(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;

	max_bytes = MIN(SMBUS_BLOCK_MAXLEN, max_bytes);

	/* Set up transaction */
	/* Disable interrupts */
	outb(inb(smbus_base + SMBHSTCTL) & ~SMBHSTCNT_INTREN,
		smbus_base + SMBHSTCTL);
	/* Set the device I'm talking to */
	outb(((device & 0x7f) << 1) | 1, smbus_base + SMBXMITADD);
	/* Set the command/address... */
	outb(cmd & 0xff, smbus_base + SMBHSTCMD);
	/* Set up for a block data read */
	outb((inb(smbus_base + SMBHSTCTL) & 0xc3) | I801_BLOCK_DATA,
	     (smbus_base + SMBHSTCTL));
	/* Clear any lingering errors, so the transaction will run */
	outb(inb(smbus_base + SMBHSTSTAT), smbus_base + SMBHSTSTAT);

	/* Reset number of bytes to transfer so we notice later it
	 * was really updated with the transaction. */
	outb(0, smbus_base + SMBHSTDAT0);

	/* Start the command */
	outb((inb(smbus_base + SMBHSTCTL) | SMBHSTCNT_START),
	     smbus_base + SMBHSTCTL);

	/* poll for it to start */
	if (smbus_wait_until_active(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_ACTIVE_TIMEOUT;

	/* Poll for transaction completion */
	do {
		loops--;
		status = inb(smbus_base + SMBHSTSTAT);
		if (status & (SMBHSTSTS_FAILED | /* FAILED */
				SMBHSTSTS_BUS_ERR | /* BUS ERR */
				SMBHSTSTS_DEV_ERR)) /* DEV ERR */
			return SMBUS_ERROR;

		if (status & SMBHSTSTS_BYTE_DONE) { /* Byte done */

			if (bytes_read < max_bytes) {
				*buf++ = inb(smbus_base + SMBBLKDAT);
				bytes_read++;
			}

			/* Engine internally completes the transaction
			 * and clears HOST_BUSY flag once the byte count
			 * from slave is reached.
			 */
			outb(status, smbus_base + SMBHSTSTAT);
		}
	} while ((status & SMBHSTSTS_HOST_BUSY) && loops);

	/* Post-check we received complete message. */
	slave_bytes = inb(smbus_base + SMBHSTDAT0);

	dprintk("%s: status = %02x, len = %d / %d, loops = %d\n",
		__func__, status, bytes_read, slave_bytes, loops);

	if (bytes_read < slave_bytes)
		return SMBUS_ERROR;

	return bytes_read;
}

int do_smbus_block_write(unsigned int smbus_base, u8 device, u8 cmd,
			const unsigned int bytes, const u8 *buf)
{
	u8 status;
	int bytes_sent = 0;
	unsigned int loops = SMBUS_TIMEOUT;

	if (bytes > SMBUS_BLOCK_MAXLEN)
		return SMBUS_ERROR;

	if (smbus_wait_until_ready(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;

	/* Set up transaction */
	/* Disable interrupts */
	outb(inb(smbus_base + SMBHSTCTL) & ~SMBHSTCNT_INTREN,
		smbus_base + SMBHSTCTL);
	/* Set the device I'm talking to */
	outb(((device & 0x7f) << 1) & ~0x01, smbus_base + SMBXMITADD);
	/* Set the command/address... */
	outb(cmd & 0xff, smbus_base + SMBHSTCMD);
	/* Set up for a block data write */
	outb((inb(smbus_base + SMBHSTCTL) & 0xc3) | I801_BLOCK_DATA,
	     (smbus_base + SMBHSTCTL));
	/* Clear any lingering errors, so the transaction will run */
	outb(inb(smbus_base + SMBHSTSTAT), smbus_base + SMBHSTSTAT);

	/* set number of bytes to transfer */
	outb(bytes, smbus_base + SMBHSTDAT0);

	/* Send first byte from buffer, bytes_sent increments after
	 * hardware acknowledges it.
	 */
	outb(*buf++, smbus_base + SMBBLKDAT);

	/* Start the command */
	outb((inb(smbus_base + SMBHSTCTL) | SMBHSTCNT_START),
	     smbus_base + SMBHSTCTL);

	/* poll for it to start */
	if (smbus_wait_until_active(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_ACTIVE_TIMEOUT;

	/* Poll for transaction completion */
	do {
		loops--;
		status = inb(smbus_base + SMBHSTSTAT);
		if (status & (SMBHSTSTS_FAILED | /* FAILED */
				SMBHSTSTS_BUS_ERR | /* BUS ERR */
				SMBHSTSTS_DEV_ERR)) /* DEV ERR */
			return SMBUS_ERROR;

		if (status & SMBHSTSTS_BYTE_DONE) {
			bytes_sent++;
			if (bytes_sent < bytes)
				outb(*buf++, smbus_base + SMBBLKDAT);

			/* Engine internally completes the transaction
			 * and clears HOST_BUSY flag once the byte count
			 * has been reached.
			 */
			outb(status, smbus_base + SMBHSTSTAT);
		}
	} while ((status & SMBHSTSTS_HOST_BUSY) && loops);

	dprintk("%s: status = %02x, len = %d / %d, loops = %d\n",
		__func__, status, bytes_sent, bytes, loops);

	if (bytes_sent < bytes)
		return SMBUS_ERROR;

	return bytes_sent;
}

/* Only since ICH5 */
int do_i2c_block_read(unsigned int smbus_base, u8 device,
		unsigned int offset, const unsigned int bytes, u8 *buf)
{
	u8 status;
	int bytes_read = 0;
	unsigned int loops = SMBUS_TIMEOUT;
	if (smbus_wait_until_ready(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;

	/* Set upp transaction */
	/* Disable interrupts */
	outb(inb(smbus_base + SMBHSTCTL) & SMBHSTCNT_INTREN,
		smbus_base + SMBHSTCTL);
	/* Set the device I'm talking to */
	outb((device & 0x7f) << 1, smbus_base + SMBXMITADD);

	/* device offset */
	outb(offset, smbus_base + SMBHSTDAT1);

	/* Set up for a i2c block data read */
	outb((inb(smbus_base + SMBHSTCTL) & 0xc3) | I801_I2C_BLOCK_DATA,
		(smbus_base + SMBHSTCTL));

	/* Clear any lingering errors, so the transaction will run */
	outb(inb(smbus_base + SMBHSTSTAT), smbus_base + SMBHSTSTAT);
	/* Start the command */
	outb((inb(smbus_base + SMBHSTCTL) | SMBHSTCNT_START),
	     smbus_base + SMBHSTCTL);

	/* poll for it to start */
	if (smbus_wait_until_active(smbus_base) < 0)
		return SMBUS_WAIT_UNTIL_ACTIVE_TIMEOUT;

	/* Poll for transaction completion */
	do {
		loops--;
		status = inb(smbus_base + SMBHSTSTAT);
		if (status & (SMBHSTSTS_FAILED | /* FAILED */
				SMBHSTSTS_BUS_ERR | /* BUS ERR */
				SMBHSTSTS_DEV_ERR)) /* DEV ERR */
			return SMBUS_ERROR;

		if (status & SMBHSTSTS_BYTE_DONE) {

			if (bytes_read < bytes) {
				*buf++ = inb(smbus_base + SMBBLKDAT);
				bytes_read++;
			}

			if (bytes_read + 1 >= bytes) {
				/* indicate that next byte is the last one */
				outb(inb(smbus_base + SMBHSTCTL)
					| SMBHSTCNT_LAST_BYTE,
					smbus_base + SMBHSTCTL);
			}

			outb(status, smbus_base + SMBHSTSTAT);
		}
	} while ((status & SMBHSTSTS_HOST_BUSY) && loops);

	dprintk("%s: status = %02x, len = %d / %d, loops = %d\n",
		__func__, status, bytes_read, bytes, loops);

	if (bytes_read < bytes)
		return SMBUS_ERROR;

	return bytes_read;
}
