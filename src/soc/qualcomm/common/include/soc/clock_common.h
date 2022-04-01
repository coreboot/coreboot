/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_COMMON_CLOCK_H__
#define __SOC_QUALCOMM_COMMON_CLOCK_H__

#define QCOM_CLOCK_DIV(div)	(2 * div - 1)

/* Root Clock Generator */
struct clock_rcg {
	u32 rcg_cmd;
	u32 rcg_cfg;
};

/* Root Clock Generator with MND */
struct clock_rcg_mnd {
	struct clock_rcg clock;
	u32 m;
	u32 n;
	u32 d_2;
};

/* DFS controlled Root Clock Generator */
struct clock_rcg_dfsr {
	u32 cmd_dfsr;
	u8 _res0[0x20 - 0x1c];
	u32 perf_dfsr[8];
	u8 _res1[0x60 - 0x40];
	u32 perf_m_dfsr[8];
	u8 _res2[0xa0 - 0x80];
	u32 perf_n_dfsr[8];
	u8 _res3[0xe0 - 0xc0];
	u32 perf_d_dfsr[8];
	u8 _res4[0x130 - 0x100];
};

/* Clock Frequency Table */
struct clock_freq_config {
	uint32_t hz;
	uint8_t src;
	uint8_t div;
	uint16_t m;
	uint16_t n;
	uint16_t d_2;
};

struct qupv3_clock {
	u32 cbcr;
	struct clock_rcg_mnd clk;
	struct clock_rcg_dfsr dfsr_clk;
};

/* PLL Configuration */
struct alpha_pll_reg_val_config {
	void *reg_mode;
	u32 mode_val;
	void *reg_l;
	u32 l_val;
	void *reg_cal_l;
	u32 cal_l_val;
	void *reg_user_ctl;
	u32 user_ctl_val;
	void *reg_user_ctl_hi;
	u32 user_ctl_hi_val;
	void *reg_user_ctl_hi1;
	u32 user_ctl_hi1_val;
	void *reg_config_ctl;
	u32 config_ctl_val;
	void *reg_config_ctl_hi;
	u32 config_ctl_hi_val;
	void *reg_config_ctl_hi1;
	u32 config_ctl_hi1_val;
	void *reg_alpha;
	u32 alpha_val;
	void *reg_opmode;
	void *reg_apcs_pll_br_en;
	bool fsm_enable;
};

enum clk_ctl_gpll_user_ctl {
	PLL_PLLOUT_MAIN_SHFT = 0,
	PLL_PLLOUT_EVEN_SHFT = 1,
	PLL_PLLOUT_ODD_SHFT = 2,
	PLL_POST_DIV_EVEN_SHFT = 8,
	PLL_POST_DIV_ODD_SHFT = 12,
	PLL_PLLOUT_EVEN_BMSK = 0x2,
};

enum gpll_mode {
	PLL_LOCK_DET_BMSK = 0x80000000,
	PLL_BYPASSNL_BMSK = 0x2,
	PLL_OUTCTRL_BMSK = 0x1,
	PLL_USERCTL_BMSK = 0xF,
	PLL_STANDBY_MODE = 0,
	PLL_RUN_MODE = 1,
	PLL_OPMODE_SHFT = 0,
	PLL_OUTCTRL_SHFT = 0,
	PLL_BYPASSNL_SHFT = 1,
	PLL_RESET_SHFT = 2,
	PLL_RESET_N_SHFT = 2,
	PLL_FSM_EN_SHFT = 20,
};

enum clk_ctl_cfg_rcgr {
	CLK_CTL_CFG_SRC_DIV_SHFT = 0,
	CLK_CTL_CFG_SRC_SEL_SHFT = 8,
	CLK_CTL_CFG_MODE_SHFT = 12,
};

enum clk_ctl_cmd_rcgr {
	CLK_CTL_CMD_UPDATE_SHFT  = 0,
};

enum clk_ctl_cbcr {
	CLK_CTL_EN_SHFT  = 0,
	CLK_CTL_OFF_SHFT = 31,
	CLK_CTL_EN_BMSK = 0x1,
	CLK_CTL_OFF_BMSK = 0x80000000,
};

enum clk_ctl_rcg_mnd {
	RCG_MODE_DUAL_EDGE = 2,
	CLK_CTL_RCG_MND_SHFT = 0,
	CLK_CTL_RCG_MND_BMSK = 0xFFFF,
};

enum clk_ctl_bcr {
	CLK_CTL_BCR_BLK_SHFT = 0,
	CLK_CTL_BCR_BLK_BMSK = 0x1,
};

enum clk_ctl_dfsr {
	CLK_CTL_CMD_DFSR_SHFT = 0,
	CLK_CTL_CMD_RCG_SW_CTL_SHFT = 15,
	CLK_CTL_CMD_DFSR_BMSK = 0x1,
};

#define GDSC_ENABLE_BIT		0

enum cb_err clock_enable_vote(void *cbcr_addr, void *vote_addr,
				uint32_t vote_bit);

enum cb_err clock_enable(void *cbcr_addr);

enum cb_err enable_and_poll_gdsc_status(void *gdscr_addr);

void clock_reset_bcr(void *bcr_addr, bool assert);

/*
 * clock_configure(): Configure the clock at the given clock speed (hz).  If hz
 * does not match any entries in the clk_cfg array, will throw and error and die().
 *
 * @param clk          struct clock_rcg pointer (root clock generator)
 * @param clk_cfg      Array with possible clock configurations
 * @param hz           frequency of clock to set
 * @param num_perfs    size of clock array
 */
enum cb_err clock_configure(struct clock_rcg *clk, struct clock_freq_config *clk_cfg,
			    uint32_t hz, uint32_t num_perfs);

void clock_configure_dfsr_table(int qup, struct clock_freq_config *clk_cfg,
		uint32_t num_perfs);

enum cb_err clock_configure_enable_gpll(struct alpha_pll_reg_val_config *cfg,
					bool enable, int br_enable);
enum cb_err agera_pll_enable(struct alpha_pll_reg_val_config *cfg);

enum cb_err zonda_pll_enable(struct alpha_pll_reg_val_config *cfg);

struct aoss {
	u8 _res0[0x50020];
	u32 aoss_cc_reset_status;
	u8 _res1[0x5002c - 0x50024];
	u32 aoss_cc_apcs_misc;
};
check_member(aoss, aoss_cc_reset_status, 0x50020);
check_member(aoss, aoss_cc_apcs_misc, 0x5002c);

struct shrm {
	u32 shrm_sproc_ctrl;
};

void clock_reset_subsystem(u32 *misc, u32 shft);

#endif
