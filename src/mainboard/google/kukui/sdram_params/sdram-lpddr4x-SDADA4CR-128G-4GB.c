/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dramc_param.h>

struct sdram_params params = {
	.source = DRAMC_PARAM_SOURCE_SDRAM_CONFIG,
	.frequency = 1600,
	.rank_num = 2,
	.ddr_geometry = DDR_TYPE_2CH_2RK_4GB_2_2,
	.wr_level = {
		[CHANNEL_A] = { {0x1F, 0x1C}, {0x1C, 0x1B} },
		[CHANNEL_B] = { {0x27, 0x28}, {0x23, 0x28} }
	},
	.cbt_cs_dly = {
		[CHANNEL_A] = {0x3, 0x3},
		[CHANNEL_B] = {0x4, 0x6}
	},
	.cbt_final_vref = {
		[CHANNEL_A] = {0x5C, 0x5A},
		[CHANNEL_B] = {0x5C, 0x5A}
	},
	.emi_cona_val = 0xF053F154,
	.emi_conh_val = 0x44440003,
	.emi_conf_val = 0x00421000,
	.chn_emi_cona_val = {0x0444F051, 0x0444F051},
	.cbt_mode_extern = CBT_NORMAL_MODE,
	.delay_cell_unit = 868,
};
