/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/helpers.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/clock.h>
#include <timer.h>
#include <types.h>

/* Clock Branch Operations */
static bool clock_is_off(u32 *cbcr_addr)
{
	return (read32(cbcr_addr) & CLK_CTL_OFF_BMSK);
}

enum cb_err clock_enable_vote(void *cbcr_addr, void *vote_addr,
				uint32_t vote_bit)
{
	int count = 100;

	setbits32(vote_addr, BIT(vote_bit));

	/* Ensure clock is enabled */
	while (count-- > 0) {
		if (!clock_is_off(cbcr_addr))
			return CB_SUCCESS;
		udelay(1);
	}
	printk(BIOS_ERR, "Failed to enable clock, register val: 0x%x\n",
			read32(cbcr_addr));
	return CB_ERR;
}

enum cb_err clock_enable(void *cbcr_addr)
{
	int count = 100;

	/* Set clock enable bit */
	setbits32(cbcr_addr, BIT(CLK_CTL_EN_SHFT));

	/* Ensure clock is enabled */
	while (count-- > 0) {
		if (!clock_is_off(cbcr_addr))
			return CB_SUCCESS;
		udelay(1);
	}
	printk(BIOS_ERR, "Failed to enable clock, register val: 0x%x\n",
			read32(cbcr_addr));
	return CB_ERR;
}

/* Clock Block Reset Operations */
void clock_reset_bcr(void *bcr_addr, bool assert)
{
	if (assert)
		setbits32(bcr_addr, BIT(CLK_CTL_BCR_BLK_SHFT));
	else
		clrbits32(bcr_addr, BIT(CLK_CTL_BCR_BLK_SHFT));
}

/* Clock GDSC Operations */
enum cb_err enable_and_poll_gdsc_status(void *gdscr_addr)
{
	if (read32(gdscr_addr) & CLK_CTL_OFF_BMSK)
		return CB_SUCCESS;

	clrbits32(gdscr_addr, BIT(GDSC_ENABLE_BIT));

	/* Ensure gdsc is enabled */
	if (!wait_us(100, (read32(gdscr_addr) & CLK_CTL_OFF_BMSK)))
		return CB_ERR;

	return CB_SUCCESS;
}

/* Clock Root clock Generator with MND Operations */
static void clock_configure_mnd(struct clock_rcg *clk, uint32_t m, uint32_t n,
				uint32_t d_2)
{
	struct clock_rcg_mnd *mnd = (struct clock_rcg_mnd *)clk;

	setbits32(&clk->rcg_cfg,
			RCG_MODE_DUAL_EDGE << CLK_CTL_CFG_MODE_SHFT);

	write32(&mnd->m, m & CLK_CTL_RCG_MND_BMSK);
	write32(&mnd->n, ~(n-m) & CLK_CTL_RCG_MND_BMSK);
	write32(&mnd->d_2, ~(d_2) & CLK_CTL_RCG_MND_BMSK);
}

/* Clock Root clock Generator Operations */
enum cb_err clock_configure(struct clock_rcg *clk,
			    struct clock_freq_config *clk_cfg, uint32_t hz,
			    uint32_t num_perfs)
{
	uint32_t reg_val, idx;

	for (idx = 0; idx < num_perfs; idx++)
		if (hz == clk_cfg[idx].hz)
			break;

	/* Verify we matched an entry.  If not, throw error. */
	if (idx >= num_perfs)
		die("Failed to find a matching clock frequency (%d hz) for %p!\n",
		    hz, clk);

	reg_val = (clk_cfg[idx].src << CLK_CTL_CFG_SRC_SEL_SHFT) |
			(clk_cfg[idx].div << CLK_CTL_CFG_SRC_DIV_SHFT);

	/* Set clock config */
	write32(&clk->rcg_cfg, reg_val);

	if (clk_cfg[idx].m != 0)
		clock_configure_mnd(clk, clk_cfg[idx].m, clk_cfg[idx].n,
				clk_cfg[idx].d_2);

	/* Commit config to RCG */
	setbits32(&clk->rcg_cmd, BIT(CLK_CTL_CMD_UPDATE_SHFT));

	return CB_SUCCESS;
}

/* Clock Root clock Generator with DFS Operations */
void clock_configure_dfsr_table(int qup, struct clock_freq_config *clk_cfg,
		uint32_t num_perfs)
{
	struct qupv3_clock *qup_clk;
	unsigned int idx, s = qup % QUP_WRAP1_S0;
	uint32_t reg_val;

	qup_clk = qup < QUP_WRAP1_S0 ?
				&gcc->qup_wrap0_s[s] : &gcc->qup_wrap1_s[s];

	clrsetbits32(&qup_clk->dfsr_clk.cmd_dfsr,
			BIT(CLK_CTL_CMD_RCG_SW_CTL_SHFT),
			BIT(CLK_CTL_CMD_DFSR_SHFT));

	for (idx = 0; idx < num_perfs; idx++) {
		reg_val = (clk_cfg[idx].src << CLK_CTL_CFG_SRC_SEL_SHFT) |
			(clk_cfg[idx].div << CLK_CTL_CFG_SRC_DIV_SHFT);

		write32(&qup_clk->dfsr_clk.perf_dfsr[idx], reg_val);

		if (clk_cfg[idx].m == 0)
			continue;

		setbits32(&qup_clk->dfsr_clk.perf_dfsr[idx],
				RCG_MODE_DUAL_EDGE << CLK_CTL_CFG_MODE_SHFT);

		reg_val = clk_cfg[idx].m & CLK_CTL_RCG_MND_BMSK;
		write32(&qup_clk->dfsr_clk.perf_m_dfsr[idx], reg_val);

		reg_val = ~(clk_cfg[idx].n - clk_cfg[idx].m)
				& CLK_CTL_RCG_MND_BMSK;
		write32(&qup_clk->dfsr_clk.perf_n_dfsr[idx], reg_val);

		reg_val = ~(clk_cfg[idx].d_2) & CLK_CTL_RCG_MND_BMSK;
		write32(&qup_clk->dfsr_clk.perf_d_dfsr[idx], reg_val);
	}
}

/* General Purpose PLL configuration and enable Operations */
enum cb_err clock_configure_enable_gpll(struct alpha_pll_reg_val_config *cfg,
					bool enable, int br_enable)
{
	if (cfg->l_val)
		write32(cfg->reg_l, cfg->l_val);

	if (cfg->cal_l_val)
		write32(cfg->reg_cal_l, cfg->cal_l_val);

	if (cfg->alpha_val)
		write32(cfg->reg_alpha, cfg->alpha_val);

	if (cfg->user_ctl_val)
		write32(cfg->reg_user_ctl, cfg->user_ctl_val);

	if (cfg->user_ctl_hi_val)
		write32(cfg->reg_user_ctl_hi, cfg->user_ctl_hi_val);

	if (cfg->user_ctl_hi1_val)
		write32(cfg->reg_user_ctl_hi1, cfg->user_ctl_hi1_val);

	if (cfg->config_ctl_val)
		write32(cfg->reg_config_ctl, cfg->config_ctl_val);

	if (cfg->config_ctl_hi_val)
		write32(cfg->reg_config_ctl_hi, cfg->config_ctl_hi_val);

	if (cfg->config_ctl_hi1_val)
		write32(cfg->reg_config_ctl_hi1, cfg->config_ctl_hi1_val);

	if (cfg->fsm_enable)
		setbits32(cfg->reg_mode, BIT(PLL_FSM_EN_SHFT));

	if (enable) {
		setbits32(cfg->reg_opmode, BIT(PLL_STANDBY_MODE));

		/*
		* H/W requires a 1us delay between placing PLL in STANDBY and
		* de-asserting the reset.
		*/
		udelay(1);
		setbits32(cfg->reg_mode, BIT(PLL_RESET_N_SHFT));

		/*
		* H/W requires a 10us delay between de-asserting the reset and
		* enabling the PLL branch bit.
		*/
		udelay(10);
		setbits32(cfg->reg_apcs_pll_br_en, BIT(br_enable));

		/* Wait for Lock Detection */
		if (!wait_us(100, read32(cfg->reg_mode) & PLL_LOCK_DET_BMSK)) {
			printk(BIOS_ERR, "PLL did not lock!\n");
			return CB_ERR;
		}
	}

	return CB_SUCCESS;
}

enum cb_err agera_pll_enable(struct alpha_pll_reg_val_config *cfg)
{
	setbits32(cfg->reg_mode, BIT(PLL_BYPASSNL_SHFT));

	/*
	* H/W requires a 5us delay between disabling the bypass and
	* de-asserting the reset.
	*/
	udelay(5);
	setbits32(cfg->reg_mode, BIT(PLL_RESET_SHFT));

	if (!wait_us(100, read32(cfg->reg_mode) & PLL_LOCK_DET_BMSK)) {
		printk(BIOS_ERR, "CPU PLL did not lock!\n");
		return CB_ERR;
	}

	setbits32(cfg->reg_mode, BIT(PLL_OUTCTRL_SHFT));

	return CB_SUCCESS;
}

enum cb_err zonda_pll_enable(struct alpha_pll_reg_val_config *cfg)
{
	setbits32(cfg->reg_mode, BIT(PLL_BYPASSNL_SHFT));

	/*
	* H/W requires a 1us delay between disabling the bypass and
	* de-asserting the reset.
	*/
	udelay(1);
	setbits32(cfg->reg_mode, BIT(PLL_RESET_SHFT));
	setbits32(cfg->reg_opmode, PLL_RUN_MODE);

	if (!wait_us(100, read32(cfg->reg_mode) & PLL_LOCK_DET_BMSK)) {
		printk(BIOS_ERR, "CPU PLL did not lock!\n");
		return CB_ERR;
	}

	setbits32(cfg->reg_user_ctl, PLL_USERCTL_BMSK);
	setbits32(cfg->reg_mode, BIT(PLL_OUTCTRL_SHFT));

	return CB_SUCCESS;
}

/* Bring subsystem out of RESET */
void clock_reset_subsystem(u32 *misc, u32 shft)
{
	clrbits32(misc, BIT(shft));
}
