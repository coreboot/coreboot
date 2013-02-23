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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/romcc_io.h>
#include "i3100.h"

static void pnp_enter_ext_func_mode(device_t dev)
{
	u16 port = dev >> 8;

	outb(0x80, port);
	outb(0x86, port);
}

static void pnp_exit_ext_func_mode(device_t dev)
{
	u16 port = dev >> 8;

	outb(0x68, port);
	outb(0x08, port);
}

/* Enable device interrupts, set UART_CLK predivide. */
static void i3100_configure_uart_clk(device_t dev, u8 predivide)
{
	pnp_enter_ext_func_mode(dev);
	pnp_write_config(dev, I3100_SIW_CONFIGURATION, (predivide << 2) | 1);
	pnp_exit_ext_func_mode(dev);
}

static void i3100_enable_serial(device_t dev, u16 iobase)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_ext_func_mode(dev);
}
