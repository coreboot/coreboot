/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __DRAMC_CUSTOM_H__
#define __DRAMC_CUSTOM_H__

//[FOR_CHROMEOS] Define the EMI_SETTINGS here
#include "dramc_top.h"


EMI_SETTINGS g_default_emi_setting = {
	.type = 0x0006,
	.EMI_CONA_VAL = 0xf053f154,
	.EMI_CONF_VAL = 0x08421000,
	.EMI_CONH_VAL = 0x44440083,
	.EMI_CONK_VAL = 0x0,
	.CHN0_EMI_CONA_VAL = 0x0400f051,
	.CHN1_EMI_CONA_VAL = 0x0400f051,
	.DRAM_RANK_SIZE = {0x100000000, 0x100000000, 0x0, 0x0},
	.dram_cbt_mode_extern = CBT_R0_R1_BYTE,
	.iLPDDR3_MODE_REG_5 = 0x6,
	.highest_freq = 4266,
};
#endif /* __DRAMC_CUSTOM_H__ */

