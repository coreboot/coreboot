/* SPDX-License-Identifier: GPL-2.0-only */

#include <gpio.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <string.h>

static const struct lpddr4_sku skus[] = {
	[0] = {
		.speed			= LP4_SPEED_2133,
		.ch0_rank_density	= LP4_16Gb_DENSITY,
		.ch1_rank_density	= LP4_16Gb_DENSITY,
		.ch0_dual_rank		= 0,
		.ch1_dual_rank		= 0,
		.part_num		= "D9SKJ",
	},
};

static const struct lpddr4_cfg lp4cfg = {
	.skus = skus,
	.num_skus = ARRAY_SIZE(skus),
};

static const uint8_t ch0_bit_swizzling[] = {
	0x03, 0x01, 0x04, 0x02, 0x00, 0x05, 0x07, 0x06,
	0x08, 0x0a, 0x0b, 0x09, 0x0c, 0x0d, 0x0f, 0x0e,
	0x17, 0x13, 0x11, 0x10, 0x15, 0x14, 0x16, 0x12,
	0x1d, 0x1c, 0x1f, 0x19, 0x1e, 0x18, 0x1b, 0x1a
};

static const uint8_t ch1_bit_swizzling[] = {
	0x00, 0x06, 0x07, 0x05, 0x03, 0x02, 0x01, 0x04,
	0x0c, 0x0f, 0x0d, 0x08, 0x09, 0x0a, 0x0b, 0x0e,
	0x15, 0x11, 0x13, 0x17, 0x12, 0x19, 0x14, 0x16,
	0x18, 0x1c, 0x19, 0x1a, 0x1b, 0x1d, 0x1f, 0x1e
};

static const uint8_t ch2_bit_swizzling[] = {
	0x05, 0x04, 0x02, 0x03, 0x07, 0x01, 0x00, 0x06,
	0x09, 0x0d, 0x0a, 0x0c, 0x0f, 0x0b, 0x0e, 0x08,
	0x17, 0x10, 0x13, 0x11, 0x15, 0x14, 0x12, 0x16,
	0x1c, 0x1a, 0x1e, 0x1b, 0x1d, 0x1f, 0x19, 0x18
};

static const uint8_t ch3_bit_swizzling[] = {
	0x00, 0x04, 0x05, 0x06, 0x01, 0x03, 0x02, 0x07,
	0x08, 0x0f, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x09,
	0x12, 0x16, 0x13, 0x11, 0x14, 0x17, 0x15, 0x19,
	0x1d, 0x1e, 0x18, 0x1b, 0x1c, 0x1a, 0x19, 0x1f
};

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *config = &memupd->FspmConfig;

	config->Package					= 0x01,
	config->Profile					= 0x06,
	config->MemoryDown				= 0x01,
	config->DDR3LPageSize				= 0x01,
	config->DDR3LASR				= 0x00,
	config->ScramblerSupport			= 0x01,
	config->ChannelHashMask				= 0x36,
	config->SliceHashMask				= 0x09,
	config->InterleavedMode				= 0x02,
	config->ChannelsSlicesEnable			= 0x00,
	config->MinRefRate2xEnable			= 0x00,
	config->DualRankSupportEnable			= 0x00,
	config->RmtMode					= 0x00,
	config->MemorySizeLimit				= 0x00,
	config->LowMemoryMaxValue			= 0x00,
	config->DisableFastBoot				= 0x00,
	config->HighMemoryMaxValue			= 0x00,
	config->DIMM0SPDAddress				= 0x00,
	config->DIMM1SPDAddress				= 0x00,

	config->Ch0_RankEnable				= 0x03,
	config->Ch0_DeviceWidth				= 0x01,
	config->Ch0_DramDensity				= 0x04,
	config->Ch0_Option				= 0x03,
	config->Ch0_OdtConfig				= 0x00,
	config->Ch0_TristateClk1			= 0x00,
	config->Ch0_Mode2N				= 0x00,
	config->Ch0_OdtLevels				= 0x00,

	config->Ch1_RankEnable				= 0x03,
	config->Ch1_DeviceWidth				= 0x01,
	config->Ch1_DramDensity				= 0x04,
	config->Ch1_Option				= 0x03,
	config->Ch1_OdtConfig				= 0x00,
	config->Ch1_TristateClk1			= 0x00,
	config->Ch1_Mode2N				= 0x00,
	config->Ch1_OdtLevels				= 0x00,

	config->Ch2_RankEnable				= 0x00,
	config->Ch2_DeviceWidth				= 0x00,
	config->Ch2_DramDensity				= 0x00,
	config->Ch2_Option				= 0x00,
	config->Ch2_OdtConfig				= 0x00,
	config->Ch2_TristateClk1			= 0x00,
	config->Ch2_Mode2N				= 0x00,
	config->Ch2_OdtLevels				= 0x00,

	config->Ch3_RankEnable				= 0x00,
	config->Ch3_DeviceWidth				= 0x00,
	config->Ch3_DramDensity				= 0x00,
	config->Ch3_Option				= 0x00,
	config->Ch3_OdtConfig				= 0x00,
	config->Ch3_TristateClk1			= 0x00,
	config->Ch3_Mode2N				= 0x00,
	config->Ch3_OdtLevels				= 0x00,

	config->RmtCheckRun				= 0x00,
	config->RmtMarginCheckScaleHighThreshold	= 0x00,
	config->MsgLevelMask				= 0x00,

	memcpy(config->Ch0_Bit_swizzling, &ch0_bit_swizzling,
			sizeof(ch0_bit_swizzling));
	memcpy(config->Ch1_Bit_swizzling, &ch1_bit_swizzling,
			sizeof(ch1_bit_swizzling));
	memcpy(config->Ch2_Bit_swizzling, &ch2_bit_swizzling,
			sizeof(ch2_bit_swizzling));
	memcpy(config->Ch3_Bit_swizzling, &ch3_bit_swizzling,
			sizeof(ch3_bit_swizzling));
}

void mainboard_save_dimm_info(void)
{
	save_lpddr4_dimm_info(&lp4cfg, 0);
}
