/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8183_EMI_H
#define SOC_MEDIATEK_MT8183_EMI_H

#include <types.h>
#include <soc/dramc_common_mt8183.h>

enum DRAMC_PARAM_SOURCE {
	DRAMC_PARAM_SOURCE_SDRAM_INVALID = 0,
	DRAMC_PARAM_SOURCE_SDRAM_CONFIG,
	DRAMC_PARAM_SOURCE_FLASH,
};

struct sdram_params {
	u16 source;		/* DRAMC_PARAM_SOURCE */
	u16 frequency;
	u32 ddr_geometry;	/* DRAMC_PARAM_GEOMETRY_TYPE */
	u8 wr_level[CHANNEL_MAX][RANK_MAX][DQS_NUMBER];

	/* DUTY */
	s8 duty_clk_delay[CHANNEL_MAX];
	s8 duty_dqs_delay[CHANNEL_MAX][DQS_NUMBER];

	/* CBT */
	u8 cbt_final_vref[CHANNEL_MAX][RANK_MAX];
	u8 cbt_clk_dly[CHANNEL_MAX][RANK_MAX];
	u8 cbt_cmd_dly[CHANNEL_MAX][RANK_MAX];
	u8 cbt_cs_dly[CHANNEL_MAX][RANK_MAX];
	u8 cbt_ca_perbit_delay[CHANNEL_MAX][RANK_MAX][DQS_BIT_NUMBER];

	/* Gating */
	u8 gating2T[CHANNEL_MAX][RANK_MAX][DQS_NUMBER];
	u8 gating05T[CHANNEL_MAX][RANK_MAX][DQS_NUMBER];
	u8 gating_fine_tune[CHANNEL_MAX][RANK_MAX][DQS_NUMBER];
	u8 gating_pass_count[CHANNEL_MAX][RANK_MAX][DQS_NUMBER];

	/* TX perbit */
	u8 tx_vref[CHANNEL_MAX][RANK_MAX];
	u16 tx_center_min[CHANNEL_MAX][RANK_MAX][DQS_NUMBER];
	u16 tx_center_max[CHANNEL_MAX][RANK_MAX][DQS_NUMBER];
	u16 tx_win_center[CHANNEL_MAX][RANK_MAX][DQ_DATA_WIDTH];
	u16 tx_first_pass[CHANNEL_MAX][RANK_MAX][DQ_DATA_WIDTH];
	u16 tx_last_pass[CHANNEL_MAX][RANK_MAX][DQ_DATA_WIDTH];

	/* datlat */
	u8 rx_datlat[CHANNEL_MAX][RANK_MAX];

	/* RX perbit */
	u8 rx_vref[CHANNEL_MAX];
	s16 rx_firspass[CHANNEL_MAX][RANK_MAX][DQ_DATA_WIDTH];
	u8 rx_lastpass[CHANNEL_MAX][RANK_MAX][DQ_DATA_WIDTH];

	u32 emi_cona_val;
	u32 emi_conh_val;
	u32 emi_conf_val;
	u32 chn_emi_cona_val[CHANNEL_MAX];
	u32 cbt_mode_extern;
	u16 delay_cell_unit;
};

struct dramc_param;
struct dramc_param_ops;

enum {
	LP4X_DDR1600,
	LP4X_DDR2400,
	LP4X_DDR3200,
	LP4X_DDR3600,
	LP4X_DDRFREQ_MAX,
};

struct dram_impedance {
	u32 data[ODT_MAX][4];
};

struct mr_value {
	u8 MR01Value[FSP_MAX];
	u8 MR13Value;
};

struct dram_shared_data {
	struct dram_impedance impedance;
	struct mr_value mr;
};

extern const u8 phy_mapping[CHANNEL_MAX][16];

int complex_mem_test(u8 *start, unsigned int len);
size_t sdram_size(void);
const struct sdram_params *get_sdram_config(void);
void enable_emi_dcm(void);
int mt_set_emi(const struct dramc_param *dparam);
void mt_mem_init(struct dramc_param_ops *dparam_ops);

#endif /* SOC_MEDIATEK_MT8183_EMI_H */
