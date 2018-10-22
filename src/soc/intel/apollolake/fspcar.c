/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corp.
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
#include <FsptUpd.h>

const FSPT_UPD temp_ram_init_params = {
	.FspUpdHeader = {
		.Signature = 0x545F4450554C5041ULL,	/* 'APLUPD_T' */
		.Revision = 1,
		.Reserved = {0},
	},
	.FsptCommonUpd = {
		.Revision = 0,
		.Reserved = {0},
		.MicrocodeRegionBase = 0,
		.MicrocodeRegionLength = 0,
		.CodeRegionBase =
			(uint32_t)(0x100000000ULL - CONFIG_ROM_SIZE),
		.CodeRegionLength = (uint32_t)CONFIG_ROM_SIZE,
		.Reserved1 = {0},
	},
};
