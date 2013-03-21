/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
#include "wpcm450.h"

static void wpcm450_pnp_set_logical_device(u8 dev, u16 port)
{
	outb(0x7, port);
	outb(dev, port+1);
}

static void wpcm450_pnp_set_enable(u8 dev, u16 port, u8 enable)
{
	outb(0x30, port);
	outb(enable, port+1);
}

static void wpcm450_pnp_set_iobase(u8 dev, u16 port, u8 index, u16 iobase)
{
	outb(index, port);
	outb((iobase>>8)&0xFF, port+1);
	outb(index+1, port);
	outb(iobase&0xFF, port+1);
}

void wpcm450_enable_dev(u8 dev, u16 port, u16 iobase)
{
	wpcm450_pnp_set_logical_device(dev, port);
	wpcm450_pnp_set_enable(dev, port, 0);
	if (iobase)
		wpcm450_pnp_set_iobase(dev, port, PNP_IDX_IO0, iobase);
	wpcm450_pnp_set_enable(dev, port, 1);
}
