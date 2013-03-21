/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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
 * Enable the serial devices on the VIA
 */
#include <arch/io.h>

/* The base address is 0x15c, 0x2e, depending on config bytes */

#define SIO_BASE 0x3f0
#define SIO_DATA  SIO_BASE+1

static void vx800_writepnpaddr(uint8_t val)
{
	outb(val, 0x2e);
	outb(val, 0xeb);
}

static void vx800_writepnpdata(uint8_t val)
{
	outb(val, 0x2f);
	outb(val, 0xeb);
}

static void vx800_writesiobyte(uint16_t reg, uint8_t val)
{
	outb(val, reg);
}

static void vx800_writesioword(uint16_t reg, uint16_t val)
{
	outw(val, reg);
}

/* regs we use: 85, and the southbridge devfn is defined by the
   mainboard
 */

void enable_vx800_serial(void)
{
	post_code(0x06);
	outb(0x03, 0x22);

	//pci_write_config8(PCI_DEV(0,17,0),0xb4,0x7e);
	//pci_write_config8(PCI_DEV(0,17,0),0xb0,0x10);

	// turn on pnp
	vx800_writepnpaddr(0x87);
	vx800_writepnpaddr(0x87);
	// now go ahead and set up com1.
	// set address
	vx800_writepnpaddr(0x7);
	vx800_writepnpdata(0x2);
	// enable serial out
	vx800_writepnpaddr(0x30);
	vx800_writepnpdata(0x1);
	// serial port 1 base address (FEh)
	vx800_writepnpaddr(0x60);
	vx800_writepnpdata(0xfe);
	// serial port 1 IRQ (04h)
	vx800_writepnpaddr(0x70);
	vx800_writepnpdata(0x4);
	// serial port 1 control
	vx800_writepnpaddr(0xf0);
	vx800_writepnpdata(0x2);
	// turn of pnp
	vx800_writepnpaddr(0xaa);

	// set up reg to set baud rate.
	vx800_writesiobyte(0x3fb, 0x80);
	// Set 115 kb
	vx800_writesioword(0x3f8, 1);
	// Set 9.6 kb
	//      WRITESIOWORD(0x3f8, 12)
	// now set no parity, one stop, 8 bits
	vx800_writesiobyte(0x3fb, 3);
	// now turn on RTS, DRT
	vx800_writesiobyte(0x3fc, 3);
	// Enable interrupts
	vx800_writesiobyte(0x3f9, 0xf);
	// should be done. Dump a char for fun.
	vx800_writesiobyte(0x3f8, 48);
	post_code(0x07);
}
