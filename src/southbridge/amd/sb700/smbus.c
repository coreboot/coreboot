/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#ifndef _SB700_SMBUS_C_
#define _SB700_SMBUS_C_

#include "smbus.h"

void alink_ab_indx(u32 reg_space, u32 reg_addr, u32 mask, u32 val)
{
	u32 tmp;

	outl((reg_space & 0x3) << 30 | reg_addr, AB_INDX);
	tmp = inl(AB_DATA);
	/* rpr 4.2
	 * For certain revisions of the chip, the ABCFG registers,
	 * with an address of 0x100NN (where 'N' is any hexadecimal
	 * number), require an extra programming step.*/
	reg_addr & 0x10000 ? outl(0, AB_INDX) : NULL;

	tmp &= ~mask;
	tmp |= val;

	/* printk(BIOS_DEBUG, "about write %x, index=%x", tmp, (reg_space&0x3)<<30 | reg_addr); */
	outl((reg_space & 0x3) << 30 | reg_addr, AB_INDX);	/* probably we dont have to do it again. */
	outl(tmp, AB_DATA);
	reg_addr & 0x10000 ? outl(0, AB_INDX) : NULL;
}

/* space = 0: AX_INDXC, AX_DATAC
 * space = 1: AX_INDXP, AX_DATAP
 */
void alink_ax_indx(u32 space, u32 axindc, u32 mask, u32 val)
{
	u32 tmp;

	/* read axindc to tmp */
	outl(space << 30 | space << 3 | 0x30, AB_INDX);
	outl(axindc, AB_DATA);
	outl(space << 30 | space << 3 | 0x34, AB_INDX);
	tmp = inl(AB_DATA);

	tmp &= ~mask;
	tmp |= val;

	/* write tmp */
	outl(space << 30 | space << 3 | 0x30, AB_INDX);
	outl(axindc, AB_DATA);
	outl(space << 30 | space << 3 | 0x34, AB_INDX);
	outl(tmp, AB_DATA);
}


static inline void smbus_delay(void)
{
	outb(inb(0x80), 0x80);
}

static int smbus_wait_until_ready(u32 smbus_io_base)
{
	u32 loops;
	loops = SMBUS_TIMEOUT;
	do {
		u8 val;
		val = inb(smbus_io_base + SMBHSTSTAT);
		val &= 0x1f;
		if (val == 0) {	/* ready now */
			return 0;
		}
		outb(val, smbus_io_base + SMBHSTSTAT);
	} while (--loops);
	return -2;		/* time out */
}

static int smbus_wait_until_done(u32 smbus_io_base)
{
	u32 loops;
	loops = SMBUS_TIMEOUT;
	do {
		u8 val;

		val = inb(smbus_io_base + SMBHSTSTAT);
		val &= 0x1f;	/* mask off reserved bits */
		if (val & 0x1c) {
			return -5;	/* error */
		}
		if (val == 0x02) {
			outb(val, smbus_io_base + SMBHSTSTAT);	/* clear status */
			return 0;
		}
	} while (--loops);
	return -3;		/* timeout */
}

int do_smbus_recv_byte(u32 smbus_io_base, u32 device)
{
	u8 byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return -2;	/* not ready */
	}

	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 1, smbus_io_base + SMBHSTADDR);

	byte = inb(smbus_io_base + SMBHSTCTRL);
	byte &= 0xe3;		/* Clear [4:2] */
	byte |= (1 << 2) | (1 << 6);	/* Byte data read/write command, start the command */
	outb(byte, smbus_io_base + SMBHSTCTRL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;	/* timeout or error */
	}

	/* read results of transaction */
	byte = inb(smbus_io_base + SMBHSTCMD);

	return byte;
}

int do_smbus_send_byte(u32 smbus_io_base, u32 device, u8 val)
{
	u8 byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return -2;	/* not ready */
	}

	/* set the command... */
	outb(val, smbus_io_base + SMBHSTCMD);

	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 0, smbus_io_base + SMBHSTADDR);

	byte = inb(smbus_io_base + SMBHSTCTRL);
	byte &= 0xe3;		/* Clear [4:2] */
	byte |= (1 << 2) | (1 << 6);	/* Byte data read/write command, start the command */
	outb(byte, smbus_io_base + SMBHSTCTRL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;	/* timeout or error */
	}

	return 0;
}

int do_smbus_read_byte(u32 smbus_io_base, u32 device, u32 address)
{
	u8 byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return -2;	/* not ready */
	}

	/* set the command/address... */
	outb(address & 0xff, smbus_io_base + SMBHSTCMD);

	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 1, smbus_io_base + SMBHSTADDR);

	byte = inb(smbus_io_base + SMBHSTCTRL);
	byte &= 0xe3;		/* Clear [4:2] */
	byte |= (1 << 3) | (1 << 6);	/* Byte data read/write command, start the command */
	outb(byte, smbus_io_base + SMBHSTCTRL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;	/* timeout or error */
	}

	/* read results of transaction */
	byte = inb(smbus_io_base + SMBHSTDAT0);

	return byte;
}

int do_smbus_write_byte(u32 smbus_io_base, u32 device, u32 address, u8 val)
{
	u8 byte;

	if (smbus_wait_until_ready(smbus_io_base) < 0) {
		return -2;	/* not ready */
	}

	/* set the command/address... */
	outb(address & 0xff, smbus_io_base + SMBHSTCMD);

	/* set the device I'm talking too */
	outb(((device & 0x7f) << 1) | 0, smbus_io_base + SMBHSTADDR);

	/* output value */
	outb(val, smbus_io_base + SMBHSTDAT0);

	byte = inb(smbus_io_base + SMBHSTCTRL);
	byte &= 0xe3;		/* Clear [4:2] */
	byte |= (1 << 3) | (1 << 6);	/* Byte data read/write command, start the command */
	outb(byte, smbus_io_base + SMBHSTCTRL);

	/* poll for transaction completion */
	if (smbus_wait_until_done(smbus_io_base) < 0) {
		return -3;	/* timeout or error */
	}

	return 0;
}

#endif
