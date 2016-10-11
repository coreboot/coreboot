/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Damien Zammit <damien@zamaudio.com>
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
 */

#include <arch/io.h>
#include <device/pnp.h>
#include <stdint.h>
#include "ite.h"

/* Global configuration registers. */
#define ITE_CONFIG_REG_CC        0x02 /* Configure Control (write-only). */
#define ITE_CONFIG_REG_LDN       0x07 /* Logical Device Number. */
#define ITE_CONFIG_REG_CLOCKSEL  0x23 /* Clock Selection. */
#define ITE_CONFIG_REG_SWSUSP    0x24 /* Software Suspend, Flash I/F. */
#define ITE_CONFIG_REG_MFC       0x2a /* multi function pin */
#define ITE_CONFIG_REG_WATCHDOG  0x72 /* watchdog config */

/* Helper procedure */
static void ite_sio_write(pnp_devfn_t dev, u8 reg, u8 value)
{
	pnp_set_logical_device(dev);
	pnp_write_config(dev, reg, value);
}

/* Enable configuration */
void pnp_enter_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;

	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	outb((port == 0x4e) ? 0xaa : 0x55, port);
}

/* Disable configuration */
void pnp_exit_conf_state(pnp_devfn_t dev)
{
	ite_sio_write(dev, ITE_CONFIG_REG_CC, 0x02);
}

void ite_reg_write(pnp_devfn_t dev, u8 reg, u8 value)
{
	pnp_enter_conf_state(dev);
	ite_sio_write(dev, reg, value);
	pnp_exit_conf_state(dev);
}


/*
 * in romstage.c
 * #define CLKIN_DEV PNP_DEV(0x2e, ITE_GPIO)
 * and pass: CLKIN_DEV
 * ITE_UART_CLK_PREDIVIDE_24
 * ITE_UART_CLK_PREDIVIDE_48 (default)
 */
void ite_conf_clkin(pnp_devfn_t dev, u8 predivide)
{
	ite_reg_write(dev, ITE_CONFIG_REG_CLOCKSEL, (0x1 & predivide));
}

/* Bring up early serial debugging output before the RAM is initialized. */
void ite_enable_serial(pnp_devfn_t dev, u16 iobase)
{
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_conf_state(dev);
}

/*
 *
 * LDN 7, reg 0x2a - needed for S3, or memory power will be cut off
 * this was documented only in IT8712F_V0.9.2!
 *
 * Enable 3VSBSW#. (For System Suspend-to-RAM)
 * 0: 3VSBSW# will be always inactive.
 * 1: 3VSBSW# enabled. It will be (NOT SUSB#) NAND SUSC#.
 *
 * in romstage.c
 * #define GPIO_DEV PNP_DEV(0x2e, ITE_GPIO)
 * and pass: GPIO_DEV
 */

void ite_enable_3vsbsw(pnp_devfn_t dev)
{
	u8 tmp;
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	tmp = pnp_read_config(dev, ITE_CONFIG_REG_MFC);
	tmp |= 0x80;
	pnp_write_config(dev, ITE_CONFIG_REG_MFC, tmp);
	pnp_exit_conf_state(dev);
}

/*
 * in romstage.c
 * #define GPIO_DEV PNP_DEV(0x2e, ITE_GPIO)
 * and pass: GPIO_DEV
*/

void ite_kill_watchdog(pnp_devfn_t dev)
{
	pnp_enter_conf_state(dev);
	ite_sio_write(dev, ITE_CONFIG_REG_WATCHDOG, 0x00);
	pnp_exit_conf_state(dev);
}
