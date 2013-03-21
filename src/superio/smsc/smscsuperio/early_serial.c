/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <device/pnp_def.h>

/* All known/supported SMSC Super I/Os have the same logical device IDs
 * for the serial ports (COM1, COM2).
 */
#define SMSCSUPERIO_SP1 4	/* Com1 */
#define SMSCSUPERIO_SP2 5	/* Com2 */

/**
 * Enable the specified serial port.
 *
 * @param dev The device to use.
 * @param iobase The I/O base of the serial port (usually 0x3f8/0x2f8).
 */
static inline void smscsuperio_enable_serial(device_t dev, u16 iobase)
{
	u16 port = dev >> 8;

	outb(0x55, port);		/* Enter the configuration state. */
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	outb(0xaa, port);		/* Exit the configuration state. */
}
