/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
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

#include <soc/romstage.h>
#include <string.h>
#include <fsp/soc_binding.h>

/* Channel 0 PHY to DUnit DQ mapping */
static const uint8_t swizzling_ch0_amenia[] = {
	 9,  14,  12,  13,  10,  11,  8,  15,  7,  3, 5, 2, 6, 0, 1, 4,
	28, 24, 26, 27, 29, 30, 31, 25, 20, 22, 23, 16, 19, 17, 18, 21,
};

/* Channel 1 PHY to DUnit DQ mapping */
static const uint8_t swizzling_ch1_amenia[] = {
	 6,  7,  5,  4,  3,  1,  0,  2,  12,  10, 11, 13, 14, 8, 9, 15,
	16, 22, 23, 20, 18, 17, 19, 21, 30, 28, 29, 25, 24, 26, 27, 31,
};

/* Channel 2 PHY to DUnit DQ mapping */
static const uint8_t swizzling_ch2_amenia[] = {
	15,  9,  8, 11, 14, 13, 12, 10,  4,  3,  7,  5,  6,  1,  0,  2,
	25, 28, 30, 31, 26, 27, 24, 29, 20, 23, 22, 21, 18, 19, 16, 17,
};

/* Channel 3 PHY to DUnit DQ mapping */
static const uint8_t swizzling_ch3_amenia[] = {
	 2,  1,  6,  7,  5,  4,  3,  0,  11,  10, 8, 9, 12, 15, 13, 14,
	17, 23, 19, 16, 21, 22, 20, 18, 31, 29, 26, 25, 28, 27, 24, 30,
};

static void amenia_fill_memory_params(FSP_M_CONFIG *cfg)
{
	cfg->Profile = 11;			/* 0xB:LPDDR4_2400_24_22_22 */
	cfg->DIMM0SPDAddress = 0;
	cfg->DIMM1SPDAddress = 0;
	cfg->Ch0_RankEnable = 0x1;
	cfg->Ch0_DeviceWidth = 0x1;
	cfg->Ch0_DramDensity = 0x2;
	cfg->Ch0_Option = 0x3;			/* Bank Address Hashing enabled */
	cfg->Ch0_TristateClk1 = 0;
	cfg->Ch0_Mode2N = 0;
	cfg->Ch0_OdtLevels = 0;
	cfg->Ch1_RankEnable = 0x1;
	cfg->Ch1_DeviceWidth = 0x1;
	cfg->Ch1_DramDensity = 0x2;
	cfg->Ch1_Option = 0x3;			/* Bank Address Hashing enabled */
	cfg->Ch1_TristateClk1 = 0;
	cfg->Ch1_Mode2N = 0;
	cfg->Ch1_OdtLevels = 0;
	cfg->Ch2_RankEnable = 0x1;
	cfg->Ch2_DeviceWidth = 0x1;
	cfg->Ch2_DramDensity = 0x2;
	cfg->Ch2_Option = 0x3;			/* Bank Address Hashing enabled */
	cfg->Ch2_TristateClk1 = 0;
	cfg->Ch2_Mode2N = 0;
	cfg->Ch2_OdtLevels = 0;
	cfg->Ch3_RankEnable = 0x1;
	cfg->Ch3_DeviceWidth = 0x1;
	cfg->Ch3_DramDensity = 0x2;
	cfg->Ch3_Option = 0x3;			/* Bank Address Hashing enabled */
	cfg->Ch3_TristateClk1 = 0;
	cfg->Ch3_Mode2N = 0;
	cfg->Ch3_OdtLevels = 0;

	memcpy(cfg->Ch0_Bit_swizzling, swizzling_ch0_amenia,
	       sizeof(swizzling_ch0_amenia));
	memcpy(cfg->Ch1_Bit_swizzling, swizzling_ch1_amenia,
	       sizeof(swizzling_ch1_amenia));
	memcpy(cfg->Ch2_Bit_swizzling, swizzling_ch2_amenia,
	       sizeof(swizzling_ch2_amenia));
	memcpy(cfg->Ch3_Bit_swizzling, swizzling_ch3_amenia,
	       sizeof(swizzling_ch3_amenia));
}

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *cfg = &memupd->FspmConfig;

	cfg->Package = 1;			/* 0x1:BGA */
	cfg->MemoryDown = 1;
	cfg->DDR3LPageSize = 0;
	cfg->DDR3LASR = 0;
	cfg->ScramblerSupport = 1;
	cfg->ChannelHashMask = 0x36;
	cfg->SliceHashMask = 0x9;
	cfg->InterleavedMode = 2;
	cfg->ChannelsSlicesEnable = 0;
	cfg->MinRefRate2xEnable = 0;
	cfg->DualRankSupportEnable = 1;
	cfg->DisableFastBoot = 0;
	cfg->RmtMode = 0;
	cfg->RmtCheckRun = 0;
	cfg->RmtMarginCheckScaleHighThreshold = 0;
	cfg->MemorySizeLimit = 0x1800;		/* Limit to 6GB */
	cfg->LowMemoryMaxValue = 0;
	cfg->HighMemoryMaxValue = 0;

	amenia_fill_memory_params(cfg);
}
