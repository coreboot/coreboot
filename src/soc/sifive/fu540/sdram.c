/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Philipp Hug <philipp@hug.cx>
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

#include <soc/sdram.h>
#include <soc/addressmap.h>

#include "regconfig-phy.h"
#include "regconfig-ctl.h"
#include "ux00ddr.h"

#define DENALI_PHY_DATA ddr_phy_settings
#define DENALI_CTL_DATA ddr_ctl_settings
#include "ddrregs.h"

#define DDR_SIZE  (8UL * 1024UL * 1024UL * 1024UL)
#define DDRCTLPLL_F 55
#define DDRCTLPLL_Q 2

void sdram_init(void)
{
	ux00ddr_writeregmap(FU540_DDRCTRL, ddr_ctl_settings, ddr_phy_settings);
	ux00ddr_disableaxireadinterleave(FU540_DDRCTRL);

	ux00ddr_disableoptimalrmodw(FU540_DDRCTRL);

	ux00ddr_enablewriteleveling(FU540_DDRCTRL);
	ux00ddr_enablereadleveling(FU540_DDRCTRL);
	ux00ddr_enablereadlevelinggate(FU540_DDRCTRL);
	if (ux00ddr_getdramclass(FU540_DDRCTRL) == DRAM_CLASS_DDR4)
		ux00ddr_enablevreftraining(FU540_DDRCTRL);

	//mask off interrupts for leveling completion
	ux00ddr_mask_leveling_completed_interrupt(FU540_DDRCTRL);

	ux00ddr_mask_mc_init_complete_interrupt(FU540_DDRCTRL);
	ux00ddr_mask_outofrange_interrupts(FU540_DDRCTRL);
	ux00ddr_setuprangeprotection(FU540_DDRCTRL, DDR_SIZE);
	ux00ddr_mask_port_command_error_interrupt(FU540_DDRCTRL);

	const uint64_t ddr_size = DDR_SIZE;
	const uint64_t ddr_end = FU540_DRAM + ddr_size;
	ux00ddr_start(FU540_DDRCTRL, FU540_DDRBUSBLOCKER, ddr_end);

	ux00ddr_phy_fixup(FU540_DDRCTRL);
}

size_t sdram_size_mb(void)
{
	static size_t size_mb = 0;

	if (!size_mb) {
		// TODO: implement
		size_mb = 8 * 1024;
	}

	return size_mb;
}
