/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
 * Enable the serial devices on the VIA CX700
 */

#include <arch/io.h>

static void cx700_writepnpaddr(u8 val)
{
	outb(val, 0x2e);
	outb(val, 0xeb);
}

static void cx700_writepnpdata(u8 val)
{
	outb(val, 0x2f);
	outb(val, 0xeb);
}

static void cx700_writesiobyte(u16 reg, u8 val)
{
	outb(val, reg);
}

static void cx700_writesioword(u16 reg, u16 val)
{
	outw(val, reg);
}

static void enable_cx700_serial(void)
{
	post_code(0x06);

	// WTH?
	outb(0x03, 0x22);

	// Set UART1 I/O Base Address
	pci_write_config8(PCI_DEV(0, 17, 0), 0xb4, 0x7e);

	// UART1 Enable
	pci_write_config8(PCI_DEV(0, 17, 0), 0xb0, 0x10);

	// turn on pnp
	cx700_writepnpaddr(0x87);
	cx700_writepnpaddr(0x87);
	// now go ahead and set up com1.
	// set address
	cx700_writepnpaddr(0x7);
	cx700_writepnpdata(0x2);
	// enable serial out
	cx700_writepnpaddr(0x30);
	cx700_writepnpdata(0x1);
	// serial port 1 base address (FEh)
	cx700_writepnpaddr(0x60);
	cx700_writepnpdata(0xfe);
	// serial port 1 IRQ (04h)
	cx700_writepnpaddr(0x70);
	cx700_writepnpdata(0x4);
	// serial port 1 control
	cx700_writepnpaddr(0xf0);
	cx700_writepnpdata(0x2);
	// turn of pnp
	cx700_writepnpaddr(0xaa);

	// XXX This part should be fully taken care of by
	// src/lib/uart8250.c:uart_init

	// set up reg to set baud rate.
	cx700_writesiobyte(0x3fb, 0x80);
	// Set 115 kb
	cx700_writesioword(0x3f8, 1);
	// Set 9.6 kb
	// cx700_writesioword(0x3f8, 12)
	// now set no parity, one stop, 8 bits
	cx700_writesiobyte(0x3fb, 3);
	// now turn on RTS, DRT
	cx700_writesiobyte(0x3fc, 3);
	// Enable interrupts
	cx700_writesiobyte(0x3f9, 0xf);
	// should be done. Dump a char for fun.
	cx700_writesiobyte(0x3f8, 48);

	post_code(0x07);
}
