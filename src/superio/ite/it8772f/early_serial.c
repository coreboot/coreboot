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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <device/pnp_def.h>
#include "it8772f.h"

/* NOTICE: This file is deprecated, use ite/common instead */

/* The base address is 0x2e or 0x4e, depending on config bytes. */
/* FIXME: SUPERIO include.c */
#define SIO_BASE                     0x2e
#define SIO_INDEX                    SIO_BASE
#define SIO_DATA                     (SIO_BASE + 1)

/* Global configuration registers. */
#define IT8772F_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define IT8772F_CONFIG_REG_LDN       0x07 /* Logical Device Number. */

u8 it8772f_sio_read(u8 index)
{
	outb(index, SIO_BASE);
	return inb(SIO_DATA);
}

void it8772f_sio_write(u8 index, u8 value)
{
	outb(index, SIO_BASE);
	outb(value, SIO_DATA);
}

static void it8772f_enter_conf(void)
{
	u16 port = SIO_BASE;

	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	outb((port == 0x4e) ? 0xaa : 0x55, port);
}

static void it8772f_exit_conf(void)
{
	it8772f_sio_write(IT8772F_CONFIG_REG_CC, 0x02);
}

/* Set AC resume to be up to the Southbridge */
void it8772f_ac_resume_southbridge(void)
{
	it8772f_enter_conf();
	it8772f_sio_write(IT8772F_CONFIG_REG_LDN, IT8772F_EC);
	it8772f_sio_write(0xf4, 0x60);
	it8772f_exit_conf();
}

/* Configure a set of GPIOs */
void it8772f_gpio_setup(int set, u8 select, u8 polarity, u8 pullup,
			u8 output, u8 enable)
{
	set--; /* Set 1 is offset 0 */
	it8772f_enter_conf();
	it8772f_sio_write(IT8772F_CONFIG_REG_LDN, IT8772F_GPIO);
	if (set < 5) {
		it8772f_sio_write(GPIO_REG_SELECT(set), select);
		it8772f_sio_write(GPIO_REG_ENABLE(set), enable);
		it8772f_sio_write(GPIO_REG_POLARITY(set), polarity);
	}
	it8772f_sio_write(GPIO_REG_OUTPUT(set), output);
	it8772f_sio_write(GPIO_REG_PULLUP(set), pullup);
	it8772f_exit_conf();
}
