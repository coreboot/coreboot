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
 * A generic romstage (pre-ram) driver for Winbond variant Super I/O chips.
 *
 * The following is derived directly from the vendor Winbond's data-sheets:
 *
 * To toggle between `configuration mode` and `normal operation mode` as to
 * manipulation the various LDN's in Winbond Super I/O's we are required to
 * pass magic numbers `passwords keys`.
 *
 *  WINBOUND_ENTRY_KEY :=  enable  configuration : 0x87
 *  WINBOUND_EXIT_KEY  :=  disable configuration : 0xAA
 *
 * To modify a LDN's configuration register, we use the index port to select
 * the index of the LDN and then write to the data port to alter the
 * parameters. A default index, data port pair is 0x4E, 0x4F respectively, a
 * user modified pair is 0x2E, 0x2F respectively.
 *
 */

#include <arch/io.h>
#include <device/pnp.h>
#include <stdint.h>
#include "winbond.h"

#define WINBOND_ENTRY_KEY 0x87
#define WINBOND_EXIT_KEY 0xAA

/* Enable configuration: pass entry key '0x87' into index port dev. */
static void pnp_enter_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(WINBOND_ENTRY_KEY, port);
	outb(WINBOND_ENTRY_KEY, port);
}

/* Disable configuration: pass exit key '0xAA' into index port dev. */
static void pnp_exit_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(WINBOND_EXIT_KEY, port);
}

/* Bring up early serial debugging output before the RAM is initialized. */
void winbond_enable_serial(pnp_devfn_t dev, u16 iobase)
{
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_conf_state(dev);
}
