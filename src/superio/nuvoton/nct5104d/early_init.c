/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Sage Electronic Engineering, LLC
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
#include <device/pnp.h>
#include <stdint.h>
#include "nct5104d.h"

#define NUVOTON_ENTRY_KEY 0x87
#define NUVOTON_EXIT_KEY 0xAA

/* Enable configuration: pass entry key '0x87' into index port dev
 * two times. */
static void pnp_enter_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(NUVOTON_ENTRY_KEY, port);
	outb(NUVOTON_ENTRY_KEY, port);
}

/* Disable configuration: pass exit key '0xAA' into index port dev. */
static void pnp_exit_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(NUVOTON_EXIT_KEY, port);
}

/* Route UARTD to pins 41-48 */
void nct5104d_enable_uartd(pnp_devfn_t dev)
{
	u8 tmp;
	u16 port = dev >> 8;
	pnp_enter_conf_state(dev);
	outb(0x1c, port);
	tmp = inb(port + 1);
	tmp |= 0x04;
	outb(tmp, port + 1);
	pnp_exit_conf_state(dev);
}
