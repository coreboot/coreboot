/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DRAMC_PI_API_H
#define _DRAMC_PI_API_H

#include <soc/dramc_common.h>
#include <soc/dramc_soc.h>
#include <soc/emi.h>

enum {
	MAX_CLKO_DELAY = 15
};

enum {
	/* jitter meter for PLL phase calibration */
	JMETER_COUNT = 1024,
	JMETER_COUNT_N = JMETER_COUNT/10,
	/* 10us for more margin, Fin = 52 */
	JMETER_WAIT_DONE_US = (JMETER_COUNT/52 + 10)
};

enum {
	DLE_TEST_NUM = 4
};

enum {
	/* window type: tx/rx */
	RX_WIN = 0,
	TX_WIN = 1,
	/* stage type: setup/hold time */
	STAGE_SETUP = 0,
	STAGE_HOLD = 1,
	/* combinational flags of stage and window type */
	STAGE_SETUP_RX_WIN = STAGE_SETUP | RX_WIN << 1,
	STAGE_SETUP_TX_WIN = STAGE_SETUP | TX_WIN << 1,
	STAGE_HOLD_RX_WIN = STAGE_HOLD | RX_WIN << 1,
	STAGE_HOLD_TX_WIN = STAGE_HOLD | TX_WIN << 1
};

enum {
	RX_DQ = 0,
	RX_DQS,
	TX_DQ,
	TX_DQS,
	TX_DQM
};

enum {
	AUDIO = 1,
	XTALK,
	ISI
};

enum {
	MEMPLL_INIT = 0,
	MEMPLL_REF_LAG,
	MEMPLL_REF_LEAD
};

enum {
	FIRST_DQ_DELAY = 0,       /* first DQ delay taps */
	FIRST_DQS_DELAY = 0,      /* first DQS delay taps */
	MAX_DQDLY_TAPS = 16,	  /* max DQ delay taps */
	MAX_TX_DQSDLY_TAPS = 16,  /* max TX DQS delay taps */
	MAX_RX_DQSDLY_TAPS = 64   /* max RX DQS delay taps */
};

enum {
	DRAMK_READ = 0,
	DRAMK_WRITE = 1
};

enum {
	ENABLE = 1,
	DISABLE = 0
};

enum {
	DATA_WIDTH_16BIT = 16,
	DATA_WIDTH_32BIT = 32
};

enum dram_tw_op {
	TE_OP_WRITE_READ_CHECK = 0,
	TE_OP_READ_CHECK
};

enum {
	DQS_GW_TE_OFFSET = 0x10,
	DQS_GW_GOLD_COUNTER_32BIT = 0x20202020,
	DQS_GW_PATTERN1 = 0xaa000000,
	DQS_GW_PATTERN2 = 0x55000000
};

enum {
	/* pattern0 and base address for test engine when we do calibration */
	DEFAULT_TEST2_1_CAL = 0x55000000,
	/* for testing, to separate TA4-3 address for running simultaneously */
	/* pattern1 and offset address for test engine when we  do calibraion */
	DEFAULT_TEST2_2_CAL = 0xaa000400,
	/* pattern0 and base addr. for test engine when doing dqs GW */
	DEFAULT_TEST2_1_DQSIEN = 0x55000000,
	/* pattern1 and offset addr. for test engine when doing dqs GW */
	DEFAULT_TEST2_2_DQSIEN = 0xaa000010,
	/* gold pattern */
	DEFAULT_GOLD_DQSIEN = 0x20202020
};

enum {
	TEST_ISI_PATTERN = 0,
	TEST_AUDIO_PATTERN,
	TEST_TA1_SIMPLE,
	TEST_TESTPAT4,
	TEST_TESTPAT4_3,
	TEST_XTALK_PATTERN,
	TEST_MIX_PATTERN
};

struct dqs_perbit_dly {
	s8 first_dqdly_pass;
	s8 last_dqdly_pass;
	s8 first_dqsdly_pass;
	s8 last_dqsdly_pass;
	s8 best_first_dqdly_pass;
	s8 best_last_dqdly_pass;
	s8 best_first_dqsdly_pass;
	s8 best_last_dqsdly_pass;
	u8 best_dqdly;
	u8 best_dqsdly;
};

void transfer_to_spm_control(void);
void transfer_to_reg_control(void);
void dramc_phy_reset(u32 channel);
void clk_duty_cal(u32 channel);
void div2_phase_sync(void);
void dramc_runtime_config(u32 channel, const struct mt8173_sdram_params *sdram_params);
void dramc_rankinctl_config(u32 channel, const struct mt8173_sdram_params *sdram_params);

/* dramc init prototypes */
void mem_pll_init(const struct mt8173_sdram_params *sdram_params);
void dramc_init(u32 channel, const struct mt8173_sdram_params *sdram_params);
void dramc_pre_init(u32 channel, const struct mt8173_sdram_params *sdram_params);

/* mandatory calibration function prototypes */
void tx_window_perbit_cal(u32 channel);
void rx_window_perbit_cal(u32 channel);
void perbit_window_cal(u32 channel, u8 type);
void sw_impedance_cal(u32 channel, const struct mt8173_sdram_params *sdram_params);
void ca_training(u32 channel, const struct mt8173_sdram_params *sdram_params);
void rx_dqs_gating_cal(u32 channel, u8 rank, const struct mt8173_sdram_params *sdram_params);
void dual_rank_rx_datlat_cal(u32 channel, const struct mt8173_sdram_params *sdram_params);
void dual_rank_rx_dqs_gating_cal(u32 channel, const struct mt8173_sdram_params *sdram_params);
void write_leveling(u32 channel, const struct mt8173_sdram_params *sdram_params);

u8 dramk_calcu_best_dly(u8 bit, struct dqs_perbit_dly *p, u8 *p_max_byte);
u8 is_dual_rank(u32 channel, const struct mt8173_sdram_params *sdram_params);
u8 rx_datlat_cal(u32 channel, u8 rank, const struct mt8173_sdram_params *sdram_params);
u32 dram_k_perbit(u32 channel);
u32 dramc_engine2(u32 channel, enum dram_tw_op wr, u32 test2_1, u32 test2_2,
		  u8 testaudpat, u8 log2loopcount);

void dramk_check_dqs_win(struct dqs_perbit_dly *p, u8 dly_step, u8 last_step, u32 fail_bit);
void dramk_check_dq_win(struct dqs_perbit_dly *p, u8 dly_step, u8 last_step, u32 fail_bit);

void tx_delay_for_wrleveling(u32 channel, struct dqs_perbit_dly *dqdqs_perbit_dly,
			     u8 *ave_dqdly_byte, u8 *max_dqsdly_byte);

#endif /* _PI_API_H */
