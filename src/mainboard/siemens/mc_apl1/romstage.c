/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
 * Copyright (C) 2017 Siemens AG
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

#include <string.h>
#include <soc/romstage.h>
#include <fsp/api.h>
#include <FspmUpd.h>
#include "brd_gpio.h"

static const uint8_t Ch0_Bit_swizzling[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t Ch1_Bit_swizzling[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t Ch2_Bit_swizzling[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static const uint8_t Ch3_Bit_swizzling[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct pad_config *pads;
	size_t num;

	/* setup early gpio before memory */
	pads = brd_early_gpio_table(&num);
	gpio_configure_pads(pads, num);

	/* DRAM Config settings */
	memupd->FspmConfig.Package = 0x1;
	memupd->FspmConfig.Profile = 0x19;
	memupd->FspmConfig.MemoryDown = 0x5;
	memupd->FspmConfig.DDR3LPageSize = 0x2;
	memupd->FspmConfig.DDR3LASR = 0x0;
	memupd->FspmConfig.ScramblerSupport = 0x0;
	memupd->FspmConfig.ChannelHashMask = 0x0;
	memupd->FspmConfig.SliceHashMask = 0x0;
	memupd->FspmConfig.InterleavedMode = 0x0;
	memupd->FspmConfig.ChannelsSlicesEnable = 0x0;
	memupd->FspmConfig.MinRefRate2xEnable = 0x1;
	memupd->FspmConfig.DualRankSupportEnable = 0x1;
	memupd->FspmConfig.RmtMode = 0x0;
	memupd->FspmConfig.MemorySizeLimit = 0x1000;
	memupd->FspmConfig.LowMemoryMaxValue = 0x0;
	memupd->FspmConfig.DisableFastBoot = 0x0;
	memupd->FspmConfig.HighMemoryMaxValue = 0x0;
	memupd->FspmConfig.DIMM0SPDAddress = 0x0;
	memupd->FspmConfig.DIMM1SPDAddress = 0x0;
	memupd->FspmConfig.Ch0_RankEnable = 0x1;
	memupd->FspmConfig.Ch0_DeviceWidth = 0x1;
	memupd->FspmConfig.Ch0_DramDensity = 0x0;
	memupd->FspmConfig.Ch0_Option = 0x3;
	memupd->FspmConfig.Ch0_OdtConfig = 0x1;
	memupd->FspmConfig.Ch0_TristateClk1 = 0x0;
	memupd->FspmConfig.Ch0_Mode2N = 0x0;
	memupd->FspmConfig.Ch0_OdtLevels = 0x0;
	memupd->FspmConfig.Ch1_RankEnable = 0x1;
	memupd->FspmConfig.Ch1_DeviceWidth = 0x1;
	memupd->FspmConfig.Ch1_DramDensity = 0x0;
	memupd->FspmConfig.Ch1_Option = 0x3;
	memupd->FspmConfig.Ch1_OdtConfig = 0x1;
	memupd->FspmConfig.Ch1_TristateClk1 = 0x0;
	memupd->FspmConfig.Ch1_Mode2N = 0x0;
	memupd->FspmConfig.Ch1_OdtLevels = 0x0;
	memupd->FspmConfig.Ch2_RankEnable = 0x0;
	memupd->FspmConfig.Ch2_DeviceWidth = 0x1;
	memupd->FspmConfig.Ch2_DramDensity = 0x0;
	memupd->FspmConfig.Ch2_Option = 0x3;
	memupd->FspmConfig.Ch2_OdtConfig = 0x0;
	memupd->FspmConfig.Ch2_TristateClk1 = 0x0;
	memupd->FspmConfig.Ch2_Mode2N = 0x0;
	memupd->FspmConfig.Ch2_OdtLevels = 0x0;
	memupd->FspmConfig.Ch3_RankEnable = 0x0;
	memupd->FspmConfig.Ch3_DeviceWidth = 0x1;
	memupd->FspmConfig.Ch3_DramDensity = 0x0;
	memupd->FspmConfig.Ch3_Option = 0x3;
	memupd->FspmConfig.Ch3_OdtConfig = 0x0;
	memupd->FspmConfig.Ch3_TristateClk1 = 0x0;
	memupd->FspmConfig.Ch3_Mode2N = 0x0;
	memupd->FspmConfig.Ch3_OdtLevels = 0x0;
	memupd->FspmConfig.RmtCheckRun = 0x3;
	memupd->FspmConfig.MrcDataSaving = 0x0;
	memupd->FspmConfig.MrcFastBoot   = 0x1;

	memcpy(memupd->FspmConfig.Ch0_Bit_swizzling, &Ch0_Bit_swizzling,
		sizeof(Ch0_Bit_swizzling));
	memcpy(memupd->FspmConfig.Ch1_Bit_swizzling, &Ch1_Bit_swizzling,
		sizeof(Ch1_Bit_swizzling));
	memcpy(memupd->FspmConfig.Ch2_Bit_swizzling, &Ch2_Bit_swizzling,
		sizeof(Ch2_Bit_swizzling));
	memcpy(memupd->FspmConfig.Ch3_Bit_swizzling, &Ch3_Bit_swizzling,
		sizeof(Ch3_Bit_swizzling));

	memupd->FspmConfig.RmtMarginCheckScaleHighThreshold = 0xC8;
	memupd->FspmConfig.MsgLevelMask = 0x0;
}
