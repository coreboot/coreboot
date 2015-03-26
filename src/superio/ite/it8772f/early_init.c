/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <device/pnp_def.h>

#include "it8772f.h"

/* NOTICE: This file is deprecated, use ite/common instead */

/* RAMstage equiv */
/* u8 pnp_read_config(pnp_devfn_t dev, u8 reg) */
u8 it8772f_sio_read(pnp_devfn_t dev, u8 reg)
{
	u16 port = dev >> 8;

	outb(reg, port);
	return inb(port + 1);
}

/* RAMstage equiv */
/* void pnp_write_config(pnp_devfn_t dev, u8 reg, u8 value) */
void it8772f_sio_write(pnp_devfn_t dev, u8 reg, u8 value)
{
	u16 port = dev >> 8;

	outb(reg, port);
	outb(value, port + 1);
}

void it8772f_enter_conf(pnp_devfn_t dev)
{
	u16 port = dev >> 8;

	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	outb((port == 0x4e) ? 0xaa : 0x55, port);
}

void it8772f_exit_conf(pnp_devfn_t dev)
{
	it8772f_sio_write(dev, IT8772F_CONFIG_REG_CC, 0x02);
}

/* Set AC resume to be up to the Southbridge */
void it8772f_ac_resume_southbridge(pnp_devfn_t dev)
{
	it8772f_enter_conf(dev);
	it8772f_sio_write(dev, IT8772F_CONFIG_REG_LDN, IT8772F_EC);
	it8772f_sio_write(dev, 0xf4, 0x60);
	it8772f_exit_conf(dev);
}

/* Configure a set of GPIOs */
void it8772f_gpio_setup(pnp_devfn_t dev, int set, u8 select, u8 polarity,
			u8 pullup, u8 output, u8 enable)
{
	set--; /* Set 1 is offset 0 */
	it8772f_enter_conf(dev);
	it8772f_sio_write(dev, IT8772F_CONFIG_REG_LDN, IT8772F_GPIO);
	if (set < 5) {
		it8772f_sio_write(dev, GPIO_REG_SELECT(set), select);
		it8772f_sio_write(dev, GPIO_REG_ENABLE(set), enable);
		it8772f_sio_write(dev, GPIO_REG_POLARITY(set), polarity);
	}
	it8772f_sio_write(dev, GPIO_REG_OUTPUT(set), output);
	it8772f_sio_write(dev, GPIO_REG_PULLUP(set), pullup);
	it8772f_exit_conf(dev);
}
