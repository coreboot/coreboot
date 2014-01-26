/*
 * This file is part of the coreboot project.
 *
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

/*
 * Pre-RAM driver for the Fintek F71869AD Super I/O chip.
 *
 * Derived from p.34 in vendor data-sheet:
 *
 * - default index port : 0x4E
 * - default data  port : 0x4F
 *
 * - enable  configuration : 0x87
 * - disable configuration : 0xAA
 *
 */

#include <arch/io.h>
#include <device/pnp.h>
#include "f71869ad.h"

/*
 * Enable configuration: pass entry key '0x87' into index port dev.
 */
static void pnp_enter_conf_state(device_t dev)
{
	u16 port = dev >> 8;
	outb(0x87, port);
	outb(0x87, port);
}

/*
 * Disable configuration: pass exit key '0xAA' into index port dev.
 */
static void pnp_exit_conf_state(device_t dev)
{
	u16 port = dev >> 8;
	outb(0xaa, port);
}

/*
 * Bring up early serial debugging output before the RAM is initialized.
 */
void f71869ad_enable_serial(device_t dev, u16 iobase)
{
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_conf_state(dev);
}
