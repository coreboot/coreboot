/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
#include <cf9_reset.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <arch/romstage.h>
#include <northbridge/intel/i945/i945.h>
#include <northbridge/intel/i945/raminit.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <southbridge/intel/common/pmclib.h>

__weak void mainboard_lpc_decode(void)
{
}

__weak void mainboard_pre_raminit_config(int s3_resume)
{
}

__weak void mainboard_get_spd_map(u8 spd_map[4])
{
	spd_map[0] = 0x50;
	spd_map[1] = 0x51;
	spd_map[2] = 0x52;
	spd_map[3] = 0x53;
}

void mainboard_romstage_entry(void)
{
	int s3resume = 0;
	u8 spd_map[4] = {};

	enable_lapic();

	mainboard_lpc_decode();

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		system_reset();
	}

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	i82801gx_early_init();
	i945_early_initialization();

	s3resume = southbridge_detect_s3_resume();

	mainboard_pre_raminit_config(s3resume);

	if (CONFIG(DEBUG_RAM_SETUP))
		dump_spd_registers();

	mainboard_get_spd_map(spd_map);

	sdram_initialize(s3resume ? 2 : 0, spd_map);

	/* This should probably go away. Until now it is required
	 * and mainboard specific
	 */
	mainboard_late_rcba_config();

	/* Chipset Errata! */
	fixup_i945_errata();

	/* Initialize the internal PCIe links before we go into stage2 */
	i945_late_initialization(s3resume);
}
