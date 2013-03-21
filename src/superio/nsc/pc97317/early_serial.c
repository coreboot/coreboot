/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
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
#include "pc97317.h"

#define PM_DEV PNP_DEV(0x2e, PC97317_PM)
#define PM_BASE 0xe8

/* The PC97317 needs clocks to be set up before the serial port will operate. */
static void pc97317_enable_serial(device_t dev, u16 iobase)
{
	/* Set base address of power management unit. */
	pnp_set_logical_device(PM_DEV);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, PM_BASE);
	pnp_set_enable(dev, 1);

	/* Use on-chip clock multiplier. */
	outb(0x03, PM_BASE);
	outb(inb(PM_BASE + 1) | 0x07, PM_BASE + 1);

	/* Wait for the clock to stabilise. */
	while(!(inb(PM_BASE + 1) & 0x80))
		;

	/* Set the base address of the port. */
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
}
