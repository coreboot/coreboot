/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Digital Design Corporation
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

/* Pre-RAM driver for SMSC LPC47N217 Super I/O chip. */

#include <arch/io.h>
#include <assert.h>
#include "lpc47n217.h"

static void pnp_enter_conf_state(device_t dev)
{
	u16 port = dev >> 8;
	outb(0x55, port);
}

static void pnp_exit_conf_state(device_t dev)
{
	u16 port = dev >> 8;
	outb(0xaa, port);
}

/**
 * Program the base I/O port for the specified logical device.
 *
 * @param dev High 8 bits = Super I/O port, low 8 bits = logical device number.
 * @param iobase Base I/O port for the logical device.
 */
static void lpc47n217_pnp_set_iobase(device_t dev, u16 iobase)
{
	/* LPC47N217 requires base ports to be a multiple of 4. */
	ASSERT(!(iobase & 0x3));

	switch(dev & 0xFF) {
	case LPC47N217_PP:
		pnp_write_config(dev, 0x23, (iobase >> 2) & 0xff);
		break;
	case LPC47N217_SP1:
		pnp_write_config(dev, 0x24, (iobase >> 2) & 0xff);
		break;
	case LPC47N217_SP2:
		pnp_write_config(dev, 0x25, (iobase >> 2) & 0xff);
		break;
	default:
		break;
	}
}

/**
 * Enable or disable the specified logical device.
 *
 * Technically, a full disable requires setting the device's base I/O port
 * below 0x100. We don't do that here, because we don't have access to a data
 * structure that specifies what the 'real' base port is (when asked to enable
 * the device). Also the function is used only to disable the device while its
 * true base port is programmed (see lpc47n217_enable_serial() below).
 *
 * @param dev High 8 bits = Super I/O port, low 8 bits = logical device number.
 * @param enable 0 to disable, anythig else to enable.
 */
static void lpc47n217_pnp_set_enable(device_t dev, int enable)
{
	u8 power_register = 0, power_mask = 0, current_power, new_power;

	switch(dev & 0xFF) {
	case LPC47N217_PP:
		power_register = 0x01;
		power_mask = 0x04;
		break;
	case LPC47N217_SP1:
		power_register = 0x02;
		power_mask = 0x08;
		break;
	case LPC47N217_SP2:
		power_register = 0x02;
		power_mask = 0x80;
		break;
	default:
		return;
	}

	current_power = pnp_read_config(dev, power_register);
	new_power = current_power & ~power_mask; /* Disable by default. */
	if (enable)
		new_power |= power_mask;	 /* Enable. */
	pnp_write_config(dev, power_register, new_power);
}

/**
 * Configure the base I/O port of the specified serial device and enable the
 * serial device.
 *
 * @param dev High 8 bits = Super I/O port, low 8 bits = logical device number.
 * @param iobase Processor I/O port address to assign to this serial device.
 */
static void lpc47n217_enable_serial(device_t dev, u16 iobase)
{
	/*
	 * NOTE: Cannot use pnp_set_XXX() here because they assume chip
	 * support for logical devices, which the LPC47N217 doesn't have.
	 */
	pnp_enter_conf_state(dev);
	lpc47n217_pnp_set_enable(dev, 0);
	lpc47n217_pnp_set_iobase(dev, iobase);
	lpc47n217_pnp_set_enable(dev, 1);
	pnp_exit_conf_state(dev);
}
