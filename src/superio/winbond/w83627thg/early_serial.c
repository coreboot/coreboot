/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
 * Copyright (C) 2004 Tyan
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
#include "w83627thg.h"

static void pnp_enter_ext_func_mode(device_t dev)
{
	u16 port = dev >> 8;
	outb(0x87, port);
	outb(0x87, port);
}

static void pnp_exit_ext_func_mode(device_t dev)
{
	u16 port = dev >> 8;
	outb(0xaa, port);
}

static void inline w83627thg_enable_serial(device_t dev, u16 iobase)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_ext_func_mode(dev);
}

#ifndef __ROMCC__
void w83627thg_set_clksel_48(device_t dev) {
	u8 reg8;
	pnp_enter_ext_func_mode(dev);
	reg8 = pnp_read_config(dev, 0x24);
	reg8 |= (1 << 6); /* Set CLKSEL (clock input on pin 1) to 48MHz. */
	pnp_write_config(dev, 0x24, reg8);
	pnp_exit_ext_func_mode(dev);
}
#endif
