/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Felix Singer <felix.singer@9elements.com>
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
#include <console/console.h>
#include "gpio.h"

static const uint8_t ch0_bit_swizzling[] = {
	0x0D, 0x0A, 0x08, 0x0B, 0x0C, 0x0F, 0x0E, 0x09,
	0x06, 0x00, 0x03, 0x04, 0x07, 0x01, 0x05, 0x02,
	0x1C, 0x1A, 0x19, 0x1B, 0x1D, 0x1F, 0x1E, 0x18,
	0x10, 0x17, 0x15, 0x16, 0x14, 0x12, 0x13, 0x11
};

static const uint8_t ch1_bit_swizzling[] = {
	0x00, 0x07, 0x04, 0x05, 0x06, 0x02, 0x03, 0x01,
	0x08, 0x0F, 0x0D, 0x0B, 0x0A, 0x09, 0x0E, 0x0C,
	0x17, 0x11, 0x13, 0x12, 0x14, 0x15, 0x16, 0x10,
	0x1C, 0x1A, 0x1D, 0x1F, 0x18, 0x19, 0x1E, 0x1B
};

static const uint8_t ch2_bit_swizzling[] = {
	0x0D, 0x08, 0x0B, 0x0E, 0x0C, 0x0F, 0x09, 0x0A,
	0x04, 0x07, 0x01, 0x06, 0x02, 0x03, 0x00, 0x05,
	0x18, 0x19, 0x1C, 0x1A, 0x1D, 0x1E, 0x1F, 0x1B,
	0x11, 0x13, 0x15, 0x10, 0x16, 0x12, 0x17, 0x14
};

static const uint8_t ch3_bit_swizzling[] = {
	0x00, 0x05, 0x04, 0x07, 0x03, 0x02, 0x06, 0x01,
	0x0A, 0x0B, 0x08, 0x09, 0x0C, 0x0E, 0x0D, 0x0F,
	0x12, 0x16, 0x14, 0x13, 0x17, 0x11, 0x15, 0x10,
	0x19, 0x1F, 0x1D, 0x1B, 0x1E, 0x18, 0x1C, 0x1A
};


void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	printk(BIOS_DEBUG, "MAINBOARD: %s/%s called\n", __FILE__, __func__);

	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));

	memupd->FspmConfig.Package = 0x1;			// 0x0
	memupd->FspmConfig.Profile = 0xB;			// 0x19
	memupd->FspmConfig.MemoryDown = 0x1;			// 0x0
	memupd->FspmConfig.DDR3LPageSize = 0x0;			// 0x1
	memupd->FspmConfig.DIMM0SPDAddress = 0x0;		// 0xa0
	memupd->FspmConfig.DIMM1SPDAddress = 0x0;		// 0xa4
	memupd->FspmConfig.RmtCheckRun = 0x3;			// 0x0
	memupd->FspmConfig.RmtMarginCheckScaleHighThreshold = 0xC8;	// 0x0
	memupd->FspmConfig.EnhancePort8xhDecoding = 0x0;	// 0x1
	memupd->FspmConfig.NpkEn = 0x0;				// 0x3
	memupd->FspmConfig.PrimaryVideoAdaptor = 0x2;		// 0x0

	memupd->FspmConfig.Ch0_RankEnable = 0x1;		// 0x0
	memupd->FspmConfig.Ch0_DeviceWidth = 0x1;		// 0x0
	memupd->FspmConfig.Ch0_DramDensity = 0x2;		// 0x0
	memupd->FspmConfig.Ch0_Option = 0x3;			// 0x0
	memupd->FspmConfig.Ch1_RankEnable = 0x1;		// 0x0
	memupd->FspmConfig.Ch1_DeviceWidth = 0x1;		// 0x0
	memupd->FspmConfig.Ch1_DramDensity = 0x2;		// 0x0
	memupd->FspmConfig.Ch1_Option = 0x3;			// 0x0
	memupd->FspmConfig.Ch2_RankEnable = 0x1;		// 0x0
	memupd->FspmConfig.Ch2_DeviceWidth = 0x1;		// 0x0
	memupd->FspmConfig.Ch2_DramDensity = 0x2;		// 0x0
	memupd->FspmConfig.Ch2_Option = 0x3;			// 0x0
	memupd->FspmConfig.Ch3_RankEnable = 0x1;		// 0x0
	memupd->FspmConfig.Ch3_DeviceWidth = 0x1;		// 0x0
	memupd->FspmConfig.Ch3_DramDensity = 0x2;		// 0x0
	memupd->FspmConfig.Ch3_Option = 0x3;			// 0x0
	memupd->FspmConfig.StartTimerTickerOfPfetAssert = 0x4E20;	// 0x0

	memcpy(memupd->FspmConfig.Ch0_Bit_swizzling, &ch0_bit_swizzling,
			sizeof(ch0_bit_swizzling));
	memcpy(memupd->FspmConfig.Ch1_Bit_swizzling, &ch1_bit_swizzling,
			sizeof(ch1_bit_swizzling));
	memcpy(memupd->FspmConfig.Ch2_Bit_swizzling, &ch2_bit_swizzling,
			sizeof(ch2_bit_swizzling));
	memcpy(memupd->FspmConfig.Ch3_Bit_swizzling, &ch3_bit_swizzling,
			sizeof(ch3_bit_swizzling));
}
