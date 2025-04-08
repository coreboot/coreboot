/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_MEDIATEK_MT8186_DRAMC_PARAM_H__
#define __SOC_MEDIATEK_MT8186_DRAMC_PARAM_H__

/*
 * NOTE: This file is shared between coreboot and dram blob. Any change in this
 * file should be synced to the other repository.
 */

#include <stdint.h>
#include <sys/types.h>
#include <soc/dramc_param_common.h>
#include <soc/dramc_soc.h>

#define DRAMC_PARAM_HEADER_VERSION 1

struct sdram_params {
	/* Sometimes, we may need to compare params member
	 * between coreboot and blob for analysis. Here,
	 * add member size using xxxB.
	 */
	/* 4 + 4 = 8B */
	u32 rank_num;
	u16 num_dlycell_perT;
	u16 delay_cell_timex100;

	/* duty 16B */
	s8 duty_clk_delay[CHANNEL_MAX][RANK_MAX];
	s8 duty_dqs_delay[CHANNEL_MAX][DQS_NUMBER_LP4];
	s8 duty_dq_delay[CHANNEL_MAX][DQS_NUMBER_LP4];
	s8 duty_dqm_delay[CHANNEL_MAX][DQS_NUMBER_LP4];

	/* CBT 48B */
	u8 cbt_final_vref[CHANNEL_MAX][RANK_MAX];
	u8 cbt_clk_dly[CHANNEL_MAX][RANK_MAX];
	u8 cbt_cmd_dly[CHANNEL_MAX][RANK_MAX];
	u8 cbt_cs_dly[CHANNEL_MAX][RANK_MAX];
	u8 cbt_ca_prebit_dly[CHANNEL_MAX][RANK_MAX][DQS_BIT_NUMBER];

	/* write leveling 8B */
	u8 wr_level[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];

	/* Gating 32B */
	u8 gating_MCK[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u8 gating_UI[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u8 gating_PI[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u8 gating_pass_count[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];

	/* TX perbit 164B */
	u8 tx_window_vref[CHANNEL_MAX][RANK_MAX];
	u16 tx_center_min[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u16 tx_center_max[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u16 tx_win_center[CHANNEL_MAX][RANK_MAX][DQ_DATA_WIDTH_LP4];

	/* rx datlat 4B */
	u8 rx_datlat[CHANNEL_MAX][RANK_MAX];

	/* RX perbit 88B */
	u8 rx_best_vref[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u8 rx_perbit_dqs[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u8 rx_perbit_dqm[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u8 rx_perbit_dq[CHANNEL_MAX][RANK_MAX][DQ_DATA_WIDTH_LP4];

	/* TX OE 16B */
	u8 tx_oe_dq_mck[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u8 tx_oe_dq_ui[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
};

struct dramc_data {
	struct ddr_base_info ddr_info;
	struct sdram_params freq_params[DRAM_DFS_SHU_MAX];
};

struct dramc_param {
	struct dramc_param_header header;
	void (*do_putc)(unsigned char c);
	struct dramc_data dramc_datas;
};

#endif  /* __SOC_MEDIATEK_MT8186_DRAMC_PARAM_H__ */
