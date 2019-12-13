/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 * Copyright (C) 2013 Vladimir Serbinenko <phcoder@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <arch/io.h>
#include <ec/acpi/ec.h>

#include <southbridge/intel/ibexpeak/pch.h>
#include <northbridge/intel/nehalem/nehalem.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* Enabled, Current table lookup index, OC map */
	{ 1, IF1_557, 0 },
	{ 1, IF1_55F, 1 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_557, 3 },
	{ 1, IF1_14B, 3 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_74B, 4 },
	{ 1, IF1_74B, 5 },
	{ 1, IF1_55F, 7 },
	{ 1, IF1_55F, 7 },
	{ 1, IF1_557, 7 },
	{ 1, IF1_55F, 7 },
};

static void set_fsb_frequency(void)
{
	u8 block[5];
	u16 fsbfreq = 62879;
	smbus_block_read(0x69, 0, 5, block);
	block[0] = fsbfreq;
	block[1] = fsbfreq >> 8;

	smbus_block_write(0x69, 0, 5, block);
}

void mainboard_pre_raminit(void)
{
	outb(0x50, 0x15ec);
	outb(inb(0x15ee) & 0x70, 0x15ee);

	set_fsb_frequency();
}

void mainboard_get_spd_map(u8 *spd_addrmap)
{
	spd_addrmap[0] = 0x50;
	spd_addrmap[2] = 0x51;
}
