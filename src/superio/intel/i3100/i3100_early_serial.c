/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
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

#include <arch/romcc_io.h>
#include "i3100.h"

static void i3100_sio_write(u8 port, u8 ldn, u8 index, u8 value)
{
	outb(0x07, port);
	outb(ldn, port + 1);
	outb(index, port);
	outb(value, port + 1);
}

static void i3100_enable_serial(u8 port, u8 ldn, u16 iobase)
{
	/* Enter configuration state. */
	outb(0x80, port);
	outb(0x86, port);

	/* Enable serial port. */
	i3100_sio_write(port, ldn, 0x30, 0x01);

	/* Set serial port I/O region. */
	i3100_sio_write(port, ldn, 0x60, (iobase >> 8) & 0xff);
	i3100_sio_write(port, ldn, 0x61, iobase & 0xff);

	/* Enable device interrupts, set UART_CLK predivide to 26. */
	i3100_sio_write(port, 0x00, 0x29, 0x0b);

	/* Exit configuration state. */
	outb(0x68, port);
	outb(0x08, port);
}
