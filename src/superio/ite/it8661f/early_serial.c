/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
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
#include "it8661f.h"

/* Perform MB PnP setup to put the SIO chip at 0x3f0. */
/* Base address 0x3f0: 0x86 0x80 0x55 0x55. */
/* Base address 0x3bd: 0x86 0x80 0x55 0xaa. */
/* Base address 0x370: 0x86 0x80 0xaa 0x55. */
static void pnp_enter_ext_func_mode(device_t dev)
{
	int i;
	u16 port = dev >> 8;

	/* TODO: Don't hardcode Super I/O config port to 0x3f0. */
	outb(0x86, IT8661F_ISA_PNP_PORT);
	outb(0x80, IT8661F_ISA_PNP_PORT);
	outb(0x55, IT8661F_ISA_PNP_PORT);
	outb(0x55, IT8661F_ISA_PNP_PORT);

	/* Sequentially write the 32 special values. */
	for (i = 0; i < 32; i++)
		outb(init_values[i], port);
}

static void pnp_exit_ext_func_mode(device_t dev)
{
	pnp_write_config(dev, IT8661F_REG_CC, (1 << 1));
}

/*
 * The logical devices will only be involved in the ISA PnP sequence if their
 * respective enable bits in IT8661F_REG_LDE are set.
 *
 * TODO: Find out if we actually need this (we use MB PnP mode).
 *
 * Bits: FDC (0), Com1 (1), Com2 (2), PP (3), IR (4). Bits 5-7 are reserved.
 */
static void it8661f_enable_logical_devices(device_t dev)
{
	pnp_enter_ext_func_mode(dev);
	pnp_write_config(dev, IT8661F_REG_LDE, 0x1f);
	pnp_exit_ext_func_mode(dev);
}

static void it8661f_set_clkin(device_t dev, u8 clkin)
{
	pnp_enter_ext_func_mode(dev);
	pnp_write_config(dev, IT8661F_REG_SWSUSP, (clkin << 1));
	pnp_exit_ext_func_mode(dev);
}

static void it8661f_enable_serial(device_t dev, u16 iobase)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_ext_func_mode(dev);
}
