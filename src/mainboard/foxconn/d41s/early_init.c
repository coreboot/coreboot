/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015  Damien Zammit <damien@zamaudio.com>
 * Copyright (C) 2018  Arthur Heymans <arthur@aheymans.xyz>
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
#include <device/pci_ops.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <northbridge/intel/pineview/pineview.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8721f/it8721f.h>

#define SERIAL_DEV PNP_DEV(0x2e, IT8721F_SP1)

void bootblock_mainboard_early_init(void)
{
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void get_mb_spd_addrmap(u8 *spd_addrmap)
{
	spd_addrmap[0] = 0x50;
	spd_addrmap[1] = 0x51;
}
