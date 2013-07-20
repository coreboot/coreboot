/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2013 Damien Zammit <damien@zamaudio.com>
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
#include "early_serial.h"

/* Superio raw commands */
static void it8728f_sio_write(device_t dev, u8 index, u8 value)
{
	pnp_set_logical_device(dev);
	pnp_write_config(dev, index, value);
}

static void it8728f_enter_conf(device_t dev)
{
	u16 port = dev >> 8;

	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	outb((port == 0x4e) ? 0xaa : 0x55, port);
}

static void it8728f_exit_conf(device_t dev)
{
	it8728f_sio_write(dev, IT8728F_CONFIG_REG_CC, 0x02);
}

/* Superio low level commands */
void it8728f_reg_write(device_t dev, u8 index, u8 value)
{
	it8728f_enter_conf(dev);
	it8728f_sio_write(dev, index, value);
	it8728f_exit_conf(dev);
}

void it8728f_24mhz_clkin(device_t dev)
{
	it8728f_reg_write(dev, IT8728F_CONFIG_REG_CLOCKSEL, 0x1);
}

void it8728f_enable_serial(device_t dev, u16 iobase)
{
	/* (1) Enter the configuration state (MB PnP mode). */
	it8728f_enter_conf(dev);

	/* (2) Modify the data of configuration registers. */
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);

	/* (3) Exit the configuration state (MB PnP mode). */
	it8728f_exit_conf(dev);
}
