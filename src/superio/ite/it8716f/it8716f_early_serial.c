/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <arch/romcc_io.h>
#include "it8716f.h"

/* The base address is 0x2e or 0x4e, depending on config bytes. */
#define SIO_BASE                     0x2e
#define SIO_INDEX                    SIO_BASE
#define SIO_DATA                     SIO_BASE+1

/* Global configuration registers. */
#define IT8716F_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define IT8716F_CONFIG_REG_LDN       0x07 /* Logical Device Number. */
#define IT8716F_CONFIG_REG_CONFIGSEL 0x22 /* Configuration Select. */
#define IT8716F_CONFIG_REG_CLOCKSEL  0x23 /* Clock Selection. */
#define IT8716F_CONFIG_REG_SWSUSP    0x24 /* Software Suspend, Flash I/F. */

#define IT8716F_CONFIGURATION_PORT   0x2e /* Write-only. */

/* Perform MB PnP setup to put the SIO chip at 0x2e. */
/* Base address 0x2e: 0x87 0x01 0x55 0x55. */
/* Base address 0x4e: 0x87 0x01 0x55 0xaa. */
static inline void pnp_enter_ext_func_mode(device_t dev)
{
	unsigned port = dev >> 8;
	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	if (port == 0x4e) {
		outb(0xaa, port);
	} else {
		outb(0x55, port);
	}
}

static void pnp_exit_ext_func_mode(device_t dev)
{
	pnp_write_config(dev, 0x02, 0x02);
}

static void it8716f_enable_serial(device_t dev, unsigned iobase)
{
	pnp_enter_ext_func_mode(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_ext_func_mode(dev);
}
