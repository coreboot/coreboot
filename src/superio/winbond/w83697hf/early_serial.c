/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Sean Nelson <snelson@nmt.edu>
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
 */

#include <arch/io.h>
#include <device/pnp_def.h>
#include "w83697hf.h"

static void pnp_enter_ext_func_mode(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(0x87, port);
	outb(0x87, port);
}

static void pnp_exit_ext_func_mode(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(0xaa, port);
}

void w83697hf_set_clksel_48(pnp_devfn_t dev)
{
	u8 reg8;

	pnp_enter_ext_func_mode(dev);
	reg8 = pnp_read_config(dev, 0x24);
	reg8 |= (1 << 6); /* Set the clock input to 48MHz. */
	pnp_write_config(dev, 0x24, reg8);
	pnp_exit_ext_func_mode(dev);
}

/* Depreciated, use winbond_enable_serial() */
void w83697hf_enable_serial(pnp_devfn_t dev, u16 iobase)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_ext_func_mode(dev);
}
