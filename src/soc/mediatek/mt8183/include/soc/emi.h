/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#ifndef SOC_MEDIATEK_MT8183_EMI_H
#define SOC_MEDIATEK_MT8183_EMI_H

#include <stdint.h>
#include <types.h>
#include <soc/dramc_common_mt8183.h>

struct sdram_params {
	u32 impedance[2][4];
	u8 wr_level[CHANNEL_MAX][RANK_MAX][DQS_NUMBER];
	u8 cbt_cs[CHANNEL_MAX][RANK_MAX];
	u8 cbt_mr12[CHANNEL_MAX][RANK_MAX];
	u32 emi_cona_val;
	u32 emi_conh_val;
	u32 emi_conf_val;
	u32 chn_emi_cona_val[CHANNEL_MAX];
	u32 cbt_mode_extern;
	u16 delay_cell_unit;
};

extern const u8 phy_mapping[CHANNEL_MAX][16];

int complex_mem_test(u8 *start, unsigned int len);
size_t sdram_size(void);
const struct sdram_params *get_sdram_config(void);
void enable_emi_dcm(void);
void mt_set_emi(const struct sdram_params *params);
void mt_mem_init(const struct sdram_params *params);

#endif /* SOC_MEDIATEK_MT8183_EMI_H */
