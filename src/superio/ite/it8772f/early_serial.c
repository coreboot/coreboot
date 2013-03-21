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

/* The base address is 0x2e or 0x4e, depending on config bytes. */
#define SIO_BASE                     IT8772F_BASE
#define SIO_INDEX                    SIO_BASE
#define SIO_DATA                     (SIO_BASE + 1)

/* Global configuration registers. */
#define IT8772F_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define IT8772F_CONFIG_REG_LDN       0x07 /* Logical Device Number. */
#define IT8772F_CONFIG_REG_CLOCKSEL  0x23 /* Clock Selection. */
#define IT8772F_CONFIG_REG_MFC       0x2a /* Multi-function control */
#define IT8772F_CONFIG_REG_WATCHDOG  0x72 /* Watchdog control. */

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

/* Select 24MHz CLKIN (48MHz is the default). */
void it8772f_24mhz_clkin(void)
{
	it8772f_enter_conf();
	it8772f_sio_write(IT8772F_CONFIG_REG_LDN, 0x00);
	it8772f_sio_write(IT8772F_CONFIG_REG_CLOCKSEL, 0x1);
	it8772f_exit_conf();
}

/*
 * LDN 7, reg 0x2a - needed for S3, or memory power will be cut off.
 *
 * Enable 3VSBSW#. (For System Suspend-to-RAM)
 * 0: 3VSBSW# will be always inactive.
 * 1: 3VSBSW# enabled. It will be (NOT SUSB#) NAND SUSC#.
 */
void it8772f_enable_3vsbsw(void)
{
	it8772f_enter_conf();
	it8772f_sio_write(IT8772F_CONFIG_REG_LDN, IT8772F_GPIO);
	it8772f_sio_write(IT8772F_CONFIG_REG_MFC, 0x80);
	it8772f_exit_conf();
}

void it8772f_kill_watchdog(void)
{
	it8772f_enter_conf();
	it8772f_sio_write(IT8772F_CONFIG_REG_LDN, IT8772F_GPIO);
	it8772f_sio_write(IT8772F_CONFIG_REG_WATCHDOG, 0x00);
	it8772f_exit_conf();
}

/* Enable the serial port(s). */
void it8772f_enable_serial(device_t dev, u16 iobase)
{
	it8772f_enter_conf();
	it8772f_sio_write(IT8772F_CONFIG_REG_LDN, dev & 0xff);
	it8772f_sio_write(PNP_IDX_IO0, (iobase >> 8) & 0xff);
	it8772f_sio_write(PNP_IDX_IO0+1, iobase & 0xff);
	it8772f_sio_write(PNP_IDX_EN, 1);
	it8772f_exit_conf();
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
