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
 */

#include <arch/io.h>
#include <device/pnp.h>
#include <stdint.h>
#include "f81216h.h"

#define FINTEK_EXIT_KEY 0xAA

static void pnp_enter_conf_state(pnp_devfn_t dev, u8 f81216h_entry_key)
{
	u16 port = dev >> 8;
	outb(f81216h_entry_key, port);
	outb(f81216h_entry_key, port);
}

static void pnp_exit_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(FINTEK_EXIT_KEY, port);
}

/* Bring up early serial debugging output before the RAM is initialized. */
void f81216h_enable_serial(pnp_devfn_t dev, u16 iobase, mode_key k)
{
	u8 key;
	switch(k) {
	case MODE_6767:
		key = 0x67;
		break;
	case MODE_7777:
		key = 0x77;
		break;
	case MODE_8787:
		key = 0x87;
		break;
	case MODE_A0A0:
		key = 0xA0;
		break;
	default:
		key = 0x77; /* try the hw default */
		break;
	}
	pnp_enter_conf_state(dev, key);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
	pnp_exit_conf_state(dev);
}
