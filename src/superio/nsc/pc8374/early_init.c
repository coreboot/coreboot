/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
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
#include "pc8374.h"

static void pc8374_enable(u16 iobase, u8 *init)
{
	u8 val, count;

	outb(0x29, iobase);
	val = inb(iobase + 1);
	val |= 0x91;
	outb(val, iobase + 1);

	for (count = 0; count < 255; count++)
		if (inb(iobase + 1) == 0x91)
			break;

	for (; *init; init++) {
		outb(*init, iobase);
		val = inb(iobase + 1);
		init++;
		val &= *init;
		init++;
		val |= *init;
		outb(val, iobase + 1);
	}
}

static void pc8374_enable_dev(device_t dev, u16 iobase)
{
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	if (iobase)
		pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
}
