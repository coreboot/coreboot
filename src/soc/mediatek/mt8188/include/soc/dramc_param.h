/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8188 Functional Specification
 * Chapter number: 3.7
 */

#ifndef __SOC_MEDIATEK_MT8188_DRAMC_PARAM_H__
#define __SOC_MEDIATEK_MT8188_DRAMC_PARAM_H__

/*
 * NOTE: This file is shared between coreboot and dram blob. Any change in this
 * file should be synced to the other repository.
 */

#include <soc/dramc_param_common.h>
#include <soc/dramc_soc.h>
#include <stdint.h>
#include <sys/types.h>

#define DRAMC_PARAM_HEADER_VERSION 1

struct sdram_params {
	/* rank, cbt */
	u32 rank_num;
	u32 dram_cbt_mode;

	u16 delay_cell_timex100;
	u8 u18ph_dly;

	/* duty */
	s8 duty_clk_delay[CHANNEL_MAX][RANK_MAX];
	s8 duty_dqs_delay[CHANNEL_MAX][DQS_NUMBER_LP4];
	s8 duty_wck_delay[CHANNEL_MAX][DQS_NUMBER_LP4];
	s8 duty_mck16x_delay[CHANNEL_MAX][DQS_NUMBER_LP4 + 1];
	s8 duty_dq_delay[CHANNEL_MAX][DQS_NUMBER_LP4];
	s8 duty_dqm_delay[CHANNEL_MAX][DQS_NUMBER_LP4];

	/* cbt */
	u8 cbt_final_vref[CHANNEL_MAX][RANK_MAX];
	u8 cbt_final_range[CHANNEL_MAX][RANK_MAX];
	s16 cbt_cmd_dly[CHANNEL_MAX];
	u16 cbt_cs_dly[CHANNEL_MAX];
	u8 cbt_ca_prebit_dly[CHANNEL_MAX][DQS_BIT_NUMBER];

	/* write leveling */
	u8 wr_level_pi[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u8 wr_level_dly[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];

	/* gating */
	u8 gating_UI[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u8 gating_PI[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u8 gating_pass_count[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];

	/* rx input buffer */
	s8 dq_offc[CHANNEL_MAX][DQ_DATA_WIDTH_LP4];
	s8 dqm_offc[CHANNEL_MAX][DQS_NUMBER_LP4];

	/* tx perbit */
	u8 tx_window_vref[CHANNEL_MAX][RANK_MAX];
	u16 tx_window_vref_range[CHANNEL_MAX][RANK_MAX];
	u16 tx_dq[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u16 tx_dqm[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u16 tx_dqm_only[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u8 tx_perbit_dlyline[CHANNEL_MAX][RANK_MAX][DQ_DATA_WIDTH_LP4];

	/* rx datlat */
	u8 rx_datlat[CHANNEL_MAX][RANK_MAX];

	/* rx perbit */
	u8 rx_best_vref[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u16 rx_perbit_dqs[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u16 rx_perbit_dqm[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u16 rx_perbit_dq[CHANNEL_MAX][RANK_MAX][DQ_DATA_WIDTH_LP4];
	s16 rx_perbit_begin;

	/* dcm */
	u8 best_u[CHANNEL_MAX][RANK_MAX];
	u8 best_l[CHANNEL_MAX][RANK_MAX];

	/* tx oe */
	u8 tx_oe_dq_mck[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u8 tx_oe_dq_ui[CHANNEL_MAX][RANK_MAX][DQS_NUMBER_LP4];
	u8 tx_oe_offset[CHANNEL_MAX][RANK_MAX];

	/* imp k */
	u8 sw_impedance[IMP_DRV_MAX];
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

const struct sdram_info *get_sdram_config(void);
struct dramc_param *get_dramc_param_from_blob(void *blob);
void dump_param_header(const void *blob);
int validate_dramc_param(const void *blob);
int is_valid_dramc_param(const void *blob);
int initialize_dramc_param(void *blob);

#endif  /* __SOC_MEDIATEK_MT8188_DRAMC_PARAM_H__ */
