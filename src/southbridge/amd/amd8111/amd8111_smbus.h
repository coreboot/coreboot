/*
 * This file is part of the coreboot project.
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

#define SMBGSTATUS 0xe0
#define SMBGCTL    0xe2
#define SMBHSTADDR 0xe4
#define SMBHSTDAT  0xe6
#define SMBHSTCMD  0xe8
#define SMBHSTFIFO 0xe9

#define SMBUS_TIMEOUT (100*1000*10)
#define SMBUS_STATUS_MASK 0xfbff

static inline void smbus_delay(void)
{
	outb(0x80, 0x80);
}

static int smbus_wait_until_ready(unsigned int smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned short val;
		smbus_delay();
		val = inw(smbus_io_base + SMBGSTATUS);
		if ((val & 0x800) == 0) {
			break;
		}
		if (loops == (SMBUS_TIMEOUT / 2)) {
			outw(inw(smbus_io_base + SMBGSTATUS),
				smbus_io_base + SMBGSTATUS);
		}
	} while (--loops);
	return loops?0:SMBUS_WAIT_UNTIL_READY_TIMEOUT;
}

static int smbus_wait_until_done(unsigned int smbus_io_base)
{
	unsigned long loops;
	loops = SMBUS_TIMEOUT;
	do {
		unsigned short val;
		smbus_delay();

		val = inw(smbus_io_base + SMBGSTATUS);
		if (((val & 0x8) == 0) | ((val & 0x0037) != 0)) {
			break;
		}
	} while (--loops);
	return loops?0:SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
}

static int do_smbus_recv_byte(unsigned int smbus_io_base, unsigned int device)
{
	unsigned int global_status_register;
	unsigned int byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}

	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBGCTL);
	/* set the device I'm talking to */
	outw(((device & 0x7f) << 1) | 1, smbus_io_base + SMBHSTADDR);
	/* set the command/address... */
	outb(0, smbus_io_base + SMBHSTCMD);
	/* set up for a send byte */
	outw((inw(smbus_io_base + SMBGCTL) & ~7) | (0x1), smbus_io_base + SMBGCTL);

	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBGSTATUS), smbus_io_base + SMBGSTATUS);

	/* set the data word...*/
	outw(0, smbus_io_base + SMBHSTDAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBGCTL) | (1 << 3)), smbus_io_base + SMBGCTL);


	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}

	global_status_register = inw(smbus_io_base + SMBGSTATUS);

	/* read results of transaction */
	byte = inw(smbus_io_base + SMBHSTDAT) & 0xff;

	if ((global_status_register & SMBUS_STATUS_MASK) != (1 << 4)) {
		return SMBUS_ERROR;
	}
	return byte;
}

static int do_smbus_send_byte(unsigned int smbus_io_base, unsigned int device,
			      unsigned int value)
{
	unsigned int global_status_register;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}

	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBGCTL);
	/* set the device I'm talking to */
	outw(((device & 0x7f) << 1) | 0, smbus_io_base + SMBHSTADDR);
	/* set the command/address... */
	outb(0, smbus_io_base + SMBHSTCMD);
	/* set up for a send byte */
	outw((inw(smbus_io_base + SMBGCTL) & ~7) | (0x1), smbus_io_base + SMBGCTL);

	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBGSTATUS), smbus_io_base + SMBGSTATUS);

	/* set the data word...*/
	outw(value, smbus_io_base + SMBHSTDAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBGCTL) | (1 << 3)), smbus_io_base + SMBGCTL);


	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}
	global_status_register = inw(smbus_io_base + SMBGSTATUS);

	if ((global_status_register & SMBUS_STATUS_MASK) != (1 << 4)) {
		return SMBUS_ERROR;
	}
	return 0;
}


static int do_smbus_read_byte(unsigned int smbus_io_base, unsigned int device,
			      unsigned int address)
{
	unsigned int global_status_register;
	unsigned int byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}

	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBGCTL);
	/* set the device I'm talking to */
	outw(((device & 0x7f) << 1) | 1, smbus_io_base + SMBHSTADDR);
	/* set the command/address... */
	outb(address & 0xFF, smbus_io_base + SMBHSTCMD);
	/* set up for a byte data read */
	outw((inw(smbus_io_base + SMBGCTL) & ~7) | (0x2), smbus_io_base + SMBGCTL);

	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBGSTATUS), smbus_io_base + SMBGSTATUS);

	/* clear the data word...*/
	outw(0, smbus_io_base + SMBHSTDAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBGCTL) | (1 << 3)), smbus_io_base + SMBGCTL);


	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}

	global_status_register = inw(smbus_io_base + SMBGSTATUS);

	/* read results of transaction */
	byte = inw(smbus_io_base + SMBHSTDAT) & 0xff;

	if ((global_status_register & SMBUS_STATUS_MASK) != (1 << 4)) {
		return SMBUS_ERROR;
	}
	return byte;
}

static int do_smbus_write_byte(unsigned int smbus_io_base, unsigned int device,
			       unsigned int address, unsigned char val)
{
	unsigned int global_status_register;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}

	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBGCTL);
	/* set the device I'm talking to */
	outw(((device & 0x7f) << 1) | 0, smbus_io_base + SMBHSTADDR);
	outb(address & 0xFF, smbus_io_base + SMBHSTCMD);
	/* set up for a byte data write */ /* FIXME */
	outw((inw(smbus_io_base + SMBGCTL) & ~7) | (0x2), smbus_io_base + SMBGCTL);
	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBGSTATUS), smbus_io_base + SMBGSTATUS);

	/* write the data word...*/
	outw(val, smbus_io_base + SMBHSTDAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBGCTL) | (1 << 3)), smbus_io_base + SMBGCTL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}
	global_status_register = inw(smbus_io_base + SMBGSTATUS);

	if ((global_status_register & SMBUS_STATUS_MASK) != (1 << 4)) {
		return SMBUS_ERROR;
	}
	return 0;
}

static int do_smbus_block_read(unsigned int smbus_io_base, unsigned int device,
			       unsigned int cmd, u8 bytes, u8 *buf)
{
	unsigned int global_status_register;
	unsigned int i;
	u8 msglen;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}

	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBGCTL);
	/* set the device I'm talking to */
	outw(((device & 0x7f) << 1) | 1, smbus_io_base + SMBHSTADDR);
	/* set the command/address... */
	outb(cmd & 0xFF, smbus_io_base + SMBHSTCMD);
	/* set up for a block data read */
	outw((inw(smbus_io_base + SMBGCTL) & ~7) | (0x5), smbus_io_base + SMBGCTL);

	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBGSTATUS), smbus_io_base + SMBGSTATUS);

	/* clear the length word...*/
	outw(0, smbus_io_base + SMBHSTDAT);

	/* start the command */
	outw((inw(smbus_io_base + SMBGCTL) | (1 << 3)), smbus_io_base + SMBGCTL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}

	global_status_register = inw(smbus_io_base + SMBGSTATUS);

	/* read results of transaction */
	msglen = inw(smbus_io_base + SMBHSTDAT) & 0x3f;

	if ((global_status_register & SMBUS_STATUS_MASK) != (1 << 4)) {
		return SMBUS_ERROR;
	}

	/* read data block */
	for (i = 0; i < msglen && i < bytes; i++) {
		buf[i] = inw(smbus_io_base + SMBHSTFIFO) & 0xff;
	}
	/* empty fifo */
	while (bytes++<msglen) {
		inw(smbus_io_base + SMBHSTFIFO);
	}

	return i;
}

static int do_smbus_block_write(unsigned int smbus_io_base, unsigned int device,
				unsigned int cmd, u8 bytes, const u8 *buf)
{
	unsigned int global_status_register;
	unsigned int i;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_READY_TIMEOUT;
	}

	/* setup transaction */
	/* disable interrupts */
	outw(inw(smbus_io_base + SMBGCTL) & ~((1<<10)|(1<<9)|(1<<8)|(1<<4)), smbus_io_base + SMBGCTL);
	/* set the device I'm talking to */
	outw(((device & 0x7f) << 1) | 0, smbus_io_base + SMBHSTADDR);
	/* set the command/address... */
	outb(cmd & 0xFF, smbus_io_base + SMBHSTCMD);
	/* set up for a block data write */
	outw((inw(smbus_io_base + SMBGCTL) & ~7) | (0x5), smbus_io_base + SMBGCTL);

	/* clear any lingering errors, so the transaction will run */
	/* Do I need to write the bits to a 1 to clear an error? */
	outw(inw(smbus_io_base + SMBGSTATUS), smbus_io_base + SMBGSTATUS);

	/* set the length word...*/
	outw(bytes, smbus_io_base + SMBHSTDAT);

	/* set the data block */
	for (i = 0; i < bytes; i++) {
		outw(buf[i], smbus_io_base + SMBHSTFIFO);
	}

	/* start the command */
	outw((inw(smbus_io_base + SMBGCTL) | (1 << 3)), smbus_io_base + SMBGCTL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return SMBUS_WAIT_UNTIL_DONE_TIMEOUT;
	}
	global_status_register = inw(smbus_io_base + SMBGSTATUS);

	if ((global_status_register & SMBUS_STATUS_MASK) != (1 << 4)) {
		return SMBUS_ERROR;
	}
	return 0;
}
