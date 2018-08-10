/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#ifndef SOC_MEDIATEK_MT8173_EMI_H
#define SOC_MEDIATEK_MT8173_EMI_H

#include <soc/dramc_common.h>
#include <stdint.h>
#include <types.h>

/* DDR type */
enum ram_type {
	TYPE_INVALID,
	TYPE_DDR1,
	TYPE_LPDDR2,
	TYPE_LPDDR3,
	TYPE_PCDDR3
};

enum {
	/*
	 * Vmem voltage adjustment:
	 * 1) HV: high voltage
	 * 2) NV: normal voltage
	 * 3) LV: low voltage
	 */
	Vmem_HV_LPDDR3 = 0x50,	/* 1.300V */
	Vmem_NV_LPDDR3 = 0x44,	/* 1.225V */
	Vmem_LV_LPDDR3 = 0x36	/* 1.138V */
};

enum {
	/*
	 * Vcore voltage adjustment:
	 * 1) HHV: extra high voltage
	 * 2) HV: high voltage
	 * 3) NV: normal voltage
	 * 4) LV: low voltage
	 * 5) LLV: extra low voltage
	 */
	Vcore_HHV_LPPDR3 = 0x60,   /* 1.300V */
	Vcore_HV_LPPDR3  = 0x48,   /* 1.150V */
	Vcore_NV_LPPDR3  = 0x44,   /* 1.125V */
	Vcore_LV_LPPDR3  = 0x34,   /* 1.025V */
	Vcore_LLV_LPPDR3 = 0x25    /* 0.931V */
};

struct mt8173_calib_params {
	u8 impedance_drvp;
	u8 impedance_drvn;
	u8 datlat_ucfirst;
	s8 ca_train[CHANNEL_NUM][CATRAINING_NUM];
	s8 ca_train_center[CHANNEL_NUM];
	s8 wr_level[CHANNEL_NUM][DQS_NUMBER];
	u8 gating_win[CHANNEL_NUM][DUAL_RANKS][GW_PARAM_NUM];
	u32 rx_dqs_dly[CHANNEL_NUM];
	u32 rx_dq_dly[CHANNEL_NUM][DQS_BIT_NUMBER];
};

struct mt8173_timing_params {
	u32 actim;
	u32 actim1;
	u32 actim05t;
	u32 conf1;
	u32 conf2;
	u32 ddr2ctl;
	u32 gddr3ctl1;
	u32 misctl0;
	u32 pd_ctrl;
	u32 rkcfg;
	u32 test2_4;
	u32 test2_3;
};

struct mt8173_emi_params {
	u32 cona;
	u32 conb;
	u32 conc;
	u32 cond;
	u32 cone;
	u32 conf;
	u32 cong;
	u32 conh;
	u32 conm_1;
	u32 conm_2;
	u32 mdct_1;
	u32 mdct_2;
	u32 test0;
	u32 test1;
	u32 testb;
	u32 testc;
	u32 testd;
	u32 arba;
	u32 arbc;
	u32 arbd;
	u32 arbe;
	u32 arbf;
	u32 arbg;
	u32 arbi;
	u32 arbj;
	u32 arbk;
	u32 slct_1;
	u32 slct_2;
	u32 bmen;
};

struct mt8173_mrs_params {
	u32 mrs_1;
	u32 mrs_2;
	u32 mrs_3;
	u32 mrs_10;
	u32 mrs_11;
	u32 mrs_63;
};

enum {
	/* CONA = 0x000 */
	CONA_DUAL_CH_EN  = BIT(0),
	CONA_32BIT_EN  = BIT(1),
	CONA_DUAL_RANK_EN  = BIT(17),
	COL_ADDR_BITS_SHIFT = 4,
	COL_ADDR_BITS_MASK = 3 << COL_ADDR_BITS_SHIFT,
	ROW_ADDR_BITS_SHIFT = 12,
	ROW_ADDR_BITS_MASK = 3 << ROW_ADDR_BITS_SHIFT
};

struct mt8173_sdram_params {
	struct mt8173_calib_params calib_params;
	struct mt8173_timing_params ac_timing;
	struct mt8173_emi_params emi_set;
	struct mt8173_mrs_params mrs_set;
	enum ram_type type;
	unsigned int dram_freq;
};

int complex_mem_test(u8 *start, unsigned int len);
void mt_set_emi(const struct mt8173_sdram_params *sdram_params);
void mt_mem_init(const struct mt8173_sdram_params *sdram_params);
const struct mt8173_sdram_params *get_sdram_config(void);
size_t sdram_size(void);
#endif
