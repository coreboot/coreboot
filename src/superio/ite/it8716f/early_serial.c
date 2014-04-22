/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2007 AMD Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
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
#include <device/pnp.h>
#include "it8716f.h"

/* Global configuration registers. */
#define IT8716F_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define IT8716F_CONFIG_REG_LDN       0x07 /* Logical Device Number. */
#define IT8716F_CONFIG_REG_CONFIGSEL 0x22 /* Configuration Select. */
#define IT8716F_CONFIG_REG_CLOCKSEL  0x23 /* Clock Selection. */
#define IT8716F_CONFIG_REG_SWSUSP    0x24 /* Software Suspend, Flash I/F. */

static void it8716f_enter_conf(device_t dev)
{
	u16 port = dev >> 8;

	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	outb((port == 0x4e) ? 0xaa : 0x55, port);
}

static void it8716f_exit_conf(device_t dev)
{
	pnp_write_config(dev, IT8716F_CONFIG_REG_CC, 0x02);
}

static void it8716f_reg_write(device_t dev, u8 index, u8 value)
{
	it8716f_enter_conf(dev);
	pnp_write_config(dev, index, value);
	it8716f_exit_conf(dev);
}


/*
 * in romstage.c
 * #define CLKIN_DEV PNP_DEV(0x2e, IT8716F_GPIO)
 * and pass: CLKIN_DEV
 * IT8716F_UART_CLK_PREDIVIDE_24
 * IT8716F_UART_CLK_PREDIVIDE_48 (default)
 */
void it8716f_conf_clkin(device_t dev, u8 predivide)
{
	it8716f_reg_write(dev, IT8716F_CONFIG_REG_CLOCKSEL, (0x1 & predivide));
}


/* Enable the serial port(s). */
void it8716f_enable_serial(device_t dev, u16 iobase)
{
	it8716f_enter_conf(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	it8716f_exit_conf(dev);
}
