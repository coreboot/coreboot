/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dramc_param.h>

struct sdram_params params = {
	.source = DRAMC_PARAM_SOURCE_SDRAM_CONFIG,
	.ddr_geometry = DDR_TYPE_2CH_2RK_8GB_4_4,
	.frequency = 1600,
	.wr_level = {
		[CHANNEL_A] = { {0x1F, 0x19}, {0x20, 0x1A} },
		[CHANNEL_B] = { {0x22, 0x1E}, {0x22, 0x1E} }
	},
	.cbt_cs_dly = {
		[CHANNEL_A] = {0x5, 0x4},
		[CHANNEL_B] = {0x8, 0x8}
	},
	.cbt_final_vref = {
		[CHANNEL_A] = {0x56, 0x56},
		[CHANNEL_B] = {0x56, 0x56}
	},
	.emi_cona_val = 0xF053F154,
	.emi_conh_val = 0x44440003,
	.emi_conf_val = 0x00421000,
	.chn_emi_cona_val = {0x0444F051, 0x0444F051},
	.cbt_mode_extern = CBT_NORMAL_MODE,
	.delay_cell_unit = 868,
};
