/*
 * This file is part of the coreboot project.
 *
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
	0x09, 0x0e, 0x0c, 0x0d, 0x0a, 0x0b, 0x08, 0x0f,
	0x05, 0x06, 0x01, 0x00, 0x02, 0x07, 0x04, 0x03,
	0x1a, 0x1f, 0x1c, 0x1b, 0x1d, 0x19, 0x18, 0x1e,
	0x14, 0x16, 0x17, 0x11, 0x12, 0x13, 0x10, 0x15
};
static const uint8_t Ch1_Bit_swizzling[] = {
	0x06, 0x07, 0x05, 0x04, 0x03, 0x01, 0x00, 0x02,
	0x0c, 0x0a, 0x0b, 0x0d, 0x0e, 0x08, 0x09, 0x0f,
	0x14, 0x10, 0x16, 0x15, 0x12, 0x11, 0x13, 0x17,
	0x1e, 0x1c, 0x1d, 0x19, 0x18, 0x1a, 0x1b, 0x1f
};
static const uint8_t Ch2_Bit_swizzling[] = {
	0x0f, 0x09, 0x08, 0x0b, 0x0c, 0x0d, 0x0e, 0x0a,
	0x05, 0x02, 0x00, 0x03, 0x06, 0x07, 0x01, 0x04,
	0x19, 0x1c, 0x1e, 0x1f, 0x1a, 0x1b, 0x18, 0x1d,
	0x14, 0x17, 0x16, 0x15, 0x12, 0x13, 0x10, 0x11
};
static const uint8_t Ch3_Bit_swizzling[] = {
	0x03, 0x04, 0x06, 0x05, 0x00, 0x01, 0x02, 0x07,
	0x0b, 0x0a, 0x08, 0x09, 0x0e, 0x0c, 0x0f, 0x0d,
	0x11, 0x17, 0x13, 0x10, 0x15, 0x16, 0x14, 0x12,
	0x1c, 0x1d, 0x1a, 0x19, 0x1e, 0x1b, 0x18, 0x1f
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	/* setup early gpio before memory */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));

	/* DRAM Config settings */
	memupd->FspmConfig.Package = 0x1;
	memupd->FspmConfig.Profile = 0xB;
	memupd->FspmConfig.MemoryDown = 0x1;
	memupd->FspmConfig.DDR3LPageSize = 0x0;
	memupd->FspmConfig.DDR3LASR = 0x0;
	memupd->FspmConfig.ScramblerSupport = 0x1;
	memupd->FspmConfig.ChannelHashMask = 0x36;
	memupd->FspmConfig.SliceHashMask = 0x9;
	memupd->FspmConfig.InterleavedMode = 0x2;
	memupd->FspmConfig.ChannelsSlicesEnable = 0x0;
	memupd->FspmConfig.MinRefRate2xEnable = 0x0;
	memupd->FspmConfig.DualRankSupportEnable = 0x1;
	memupd->FspmConfig.RmtMode = 0x0;
	memupd->FspmConfig.MemorySizeLimit = 0x1800;
	memupd->FspmConfig.LowMemoryMaxValue = 0x0;
	memupd->FspmConfig.DisableFastBoot = 0x0;
	memupd->FspmConfig.HighMemoryMaxValue = 0x0;
	memupd->FspmConfig.DIMM0SPDAddress = 0x0;
	memupd->FspmConfig.DIMM1SPDAddress = 0x0;
	memupd->FspmConfig.Ch0_RankEnable = 0x3;
	memupd->FspmConfig.Ch0_DeviceWidth = 0x1;
	memupd->FspmConfig.Ch0_DramDensity = 0x2;
	memupd->FspmConfig.Ch0_Option = 0x3;
	memupd->FspmConfig.Ch0_OdtConfig = 0x0;
	memupd->FspmConfig.Ch0_TristateClk1 = 0x0;
	memupd->FspmConfig.Ch0_Mode2N = 0x0;
	memupd->FspmConfig.Ch0_OdtLevels = 0x0;
	memupd->FspmConfig.Ch1_RankEnable = 0x3;
	memupd->FspmConfig.Ch1_DeviceWidth = 0x1;
	memupd->FspmConfig.Ch1_DramDensity = 0x2;
	memupd->FspmConfig.Ch1_Option = 0x3;
	memupd->FspmConfig.Ch1_OdtConfig = 0x0;
	memupd->FspmConfig.Ch1_TristateClk1 = 0x0;
	memupd->FspmConfig.Ch1_Mode2N = 0x0;
	memupd->FspmConfig.Ch1_OdtLevels = 0x0;
	memupd->FspmConfig.Ch2_RankEnable = 0x3;
	memupd->FspmConfig.Ch2_DeviceWidth = 0x1;
	memupd->FspmConfig.Ch2_DramDensity = 0x2;
	memupd->FspmConfig.Ch2_Option = 0x3;
	memupd->FspmConfig.Ch2_OdtConfig = 0x0;
	memupd->FspmConfig.Ch2_TristateClk1 = 0x0;
	memupd->FspmConfig.Ch2_Mode2N = 0x0;
	memupd->FspmConfig.Ch2_OdtLevels = 0x0;
	memupd->FspmConfig.Ch3_RankEnable = 0x3;
	memupd->FspmConfig.Ch3_DeviceWidth = 0x1;
	memupd->FspmConfig.Ch3_DramDensity = 0x2;
	memupd->FspmConfig.Ch3_Option = 0x3;
	memupd->FspmConfig.Ch3_OdtConfig = 0x0;
	memupd->FspmConfig.Ch3_TristateClk1 = 0x0;
	memupd->FspmConfig.Ch3_Mode2N = 0x0;
	memupd->FspmConfig.Ch3_OdtLevels = 0x0;
	memupd->FspmConfig.RmtCheckRun = 0x0;
	memupd->FspmConfig.MrcDataSaving = 0x0;
	memupd->FspmConfig.MrcFastBoot   = 0x0;

	memcpy(memupd->FspmConfig.Ch0_Bit_swizzling, &Ch0_Bit_swizzling,
		sizeof(Ch0_Bit_swizzling));
	memcpy(memupd->FspmConfig.Ch1_Bit_swizzling, &Ch1_Bit_swizzling,
		sizeof(Ch1_Bit_swizzling));
	memcpy(memupd->FspmConfig.Ch2_Bit_swizzling, &Ch2_Bit_swizzling,
		sizeof(Ch2_Bit_swizzling));
	memcpy(memupd->FspmConfig.Ch3_Bit_swizzling, &Ch3_Bit_swizzling,
		sizeof(Ch3_Bit_swizzling));

	memupd->FspmConfig.RmtMarginCheckScaleHighThreshold = 0x0;
	memupd->FspmConfig.MsgLevelMask = 0x0;
}
