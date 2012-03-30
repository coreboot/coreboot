/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
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

/*
 * The chip could be bootstrap mapped to one of four LPC addresses:
 * 0x2e, 0x4e, 0x162e, and 0x164e.
 */
const u16 sio1007_lpc_ports[] = {0x2e, 0x4e, 0x162e, 0x164e};

static void sio1007_setreg(u16 lpc_port, u8 reg, u8 value, u8 mask)
{
	u8 reg_value;

	outb(reg, lpc_port);
	reg_value = inb(lpc_port + 1);
	reg_value &= ~mask;
	reg_value |= (value & mask);
	outb(reg_value, lpc_port + 1);
}

static int sio1007_enable_uart_at(u16 port)
{
	/* Enable config mode. */
	outb(0x55, port);
	if (inb(port) != 0x55)
		return 0; /* There is no LPC device at this address. */

	/* Registers 12 and 13 hold config address, look for a match. */
	outb(0x12, port);
	if (inb(port + 1) != (port & 0xff))
		return 0;

	outb(0x13, port);
	if (inb(port + 1) != (port >> 8))
		return 0;

	/* This must be the sio1007, enable the UART. */
	/* turn on power */
	sio1007_setreg(port, 0x2, 1 << 3, 1 << 3);
	/* enable high speed */
	sio1007_setreg(port, 0xc, 1 << 6, 1 << 6);
	/* set the base address */
	sio1007_setreg(port, 0x24, CONFIG_TTYS0_BASE >> 2, 0xff);

	/* Disable config mode. */
	outb(0xaa, port);
	return 1;
}
