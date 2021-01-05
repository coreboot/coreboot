/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dramc_param.h>

struct sdram_params params = {
	.source = DRAMC_PARAM_SOURCE_SDRAM_CONFIG,
	.ddr_geometry = DDR_TYPE_2CH_2RK_4GB_2_2,
	.frequency = 1600,
	.rank_num = 2,
	.wr_level = {
		[CHANNEL_A] = { {0x22, 0x1b}, {0x22, 0x19} },
		[CHANNEL_B] = { {0x24, 0x20}, {0x25, 0x20} }
	},
	.cbt_cs_dly = {
		[CHANNEL_A] = {0x0, 0x0},
		[CHANNEL_B] = {0x0, 0x0}
	},
	.cbt_final_vref = {
		[CHANNEL_A] = {0x52, 0x52},
		[CHANNEL_B] = {0x52, 0x52}
	},
	.emi_cona_val = 0xF053F154,
	.emi_conh_val = 0x44440003,
	.emi_conf_val = 0x00421000,
	.chn_emi_cona_val = {0x0444F051, 0x0444F051},
	.cbt_mode_extern = CBT_NORMAL_MODE,
	.delay_cell_unit = 868,
};
