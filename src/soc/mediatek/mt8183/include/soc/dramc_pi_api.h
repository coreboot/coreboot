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

#ifndef _DRAMC_PI_API_MT8183_H
#define _DRAMC_PI_API_MT8183_H

#include <types.h>
#include <soc/emi.h>
#include <console/console.h>

#define dramc_show(_x_...) printk(BIOS_INFO, _x_)
#if CONFIG(DEBUG_DRAM)
#define dramc_dbg(_x_...) printk(BIOS_DEBUG, _x_)
#else
#define dramc_dbg(_x_...)
#endif

#define DATLAT_TAP_NUMBER 32

#define DRAMC_BROADCAST_ON 0x1f
#define DRAMC_BROADCAST_OFF 0x0
#define TX_DQ_COARSE_TUNE_TO_FINE_TUNE_TAP 64

#define IMP_LP4X_TERM_VREF_SEL		0x1b
#define IMP_DRVP_LP4X_UNTERM_VREF_SEL	0x1a
#define IMP_DRVN_LP4X_UNTERM_VREF_SEL	0x16
#define IMP_TRACK_LP4X_UNTERM_VREF_SEL	0x1a

enum dram_te_op {
	TE_OP_WRITE_READ_CHECK = 0,
	TE_OP_READ_CHECK
};

enum {
	FSP_0 = 0,
	FSP_1,
	FSP_MAX
};

enum {
	TX_DQ_DQS_MOVE_DQ_ONLY = 0,
	TX_DQ_DQS_MOVE_DQM_ONLY,
	TX_DQ_DQS_MOVE_DQ_DQM
};

enum {
	MAX_CA_FINE_TUNE_DELAY = 63,
	MAX_CS_FINE_TUNE_DELAY = 63,
	MAX_CLK_FINE_TUNE_DELAY = 31,
	CATRAINING_NUM = 6,
	PASS_RANGE_NA = 0x7fff
};

enum {
	GATING_PATTERN_NUM = 0x23,
	GATING_GOLDEND_DQSCNT = 0x4646
};

enum {
	IMPCAL_STAGE_DRVP = 0x1,
	IMPCAL_STAGE_DRVN,
	IMPCAL_STAGE_TRACKING
};

enum {
	DQS_GW_COARSE_STEP = 1,
	DQS_GW_FINE_START = 0,
	DQS_GW_FINE_END = 32,
	DQS_GW_FINE_STEP = 4,
	DQS_GW_FREQ_DIV = 4,
	RX_DQS_CTL_LOOP = 8,
	RX_DLY_DQSIENSTB_LOOP = 32
};

enum {
	SAVE_VALUE,
	RESTORE_VALUE
};

struct reg_value {
	u32 *addr;
	u32 value;
};

enum {
	DQ_DIV_SHIFT = 3,
	DQ_DIV_MASK = BIT(DQ_DIV_SHIFT) - 1,
	OEN_SHIFT = 16,

	DQS_DELAY_2T = 3,
	DQS_DELAY_0P5T = 4,
	DQS_DELAY = ((DQS_DELAY_2T << DQ_DIV_SHIFT) + DQS_DELAY_0P5T) << 5,

	DQS_OEN_DELAY_2T = 3,
	DQS_OEN_DELAY_0P5T = 1,

	SELPH_DQS0 = (DQS_DELAY_2T << 0) | (DQS_DELAY_2T << 4) |
		     (DQS_DELAY_2T << 8) | (DQS_DELAY_2T << 12) |
		     (DQS_OEN_DELAY_2T << 16) | (DQS_OEN_DELAY_2T << 20) |
		     (DQS_OEN_DELAY_2T << 24) | (DQS_OEN_DELAY_2T << 28),

	SELPH_DQS1 = (DQS_DELAY_0P5T << 0) | (DQS_DELAY_0P5T << 4) |
		     (DQS_DELAY_0P5T << 8) | (DQS_DELAY_0P5T << 12) |
		     (DQS_OEN_DELAY_0P5T << 16) | (DQS_OEN_DELAY_0P5T << 20) |
		     (DQS_OEN_DELAY_0P5T << 24) | (DQS_OEN_DELAY_0P5T << 28)
};

void dramc_get_rank_size(u64 *dram_rank_size);
void dramc_runtime_config(void);
void dramc_set_broadcast(u32 onoff);
u32 dramc_get_broadcast(void);
void dramc_init(void);
void dramc_sw_impedance(const struct sdram_params *params);
void dramc_apply_config_before_calibration(void);
void dramc_apply_config_after_calibration(void);
void dramc_calibrate_all_channels(const struct sdram_params *params);
void dramc_hw_gating_onoff(u8 chn, bool onoff);
void dramc_enable_phy_dcm(bool bEn);

#endif /* _DRAMC_PI_API_MT8183_H */
