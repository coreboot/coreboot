/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootblock_common.h>
#include <superio/fintek/common/fintek.h>
#include <superio/fintek/f71869ad/f71869ad.h>

/* Ensure Super I/O config address (i.e., 0x2e or 0x4e) matches that of devicetree.cb */
#define SERIAL_DEV PNP_DEV(0x2e, F71869AD_SP1)

void bootblock_mainboard_early_init(void)
{
	fintek_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
