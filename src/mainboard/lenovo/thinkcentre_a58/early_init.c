/*
 * This file is part of the coreboot project.
 *
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

#include <bootblock_common.h>
#include <northbridge/intel/x4x/x4x.h>
#include <superio/smsc/smscsuperio/smscsuperio.h>

#define SERIAL_DEV PNP_DEV(0x2e, SMSCSUPERIO_SP1)

void bootblock_mainboard_early_init(void)
{
	smscsuperio_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void mb_get_spd_map(u8 spd_map[4])
{
	spd_map[0] = 0x50;
	spd_map[2] = 0x52;
}
