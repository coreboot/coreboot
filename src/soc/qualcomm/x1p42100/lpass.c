/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/clock.h>
#include <soc/lpass.h>
#include <soc/rpmh_bcm.h>
#include <timer.h>

static enum cb_err lpass_aon_cc_pll_enable(void)
{
	struct alpha_pll_reg_val_config pll_cfg = {0};

	pll_cfg.reg_mode = &lpass_aon_cc_pll->pll_mode;
	pll_cfg.reg_l = &lpass_aon_cc_pll->pll_l_val;
	pll_cfg.l_val = HAL_CLK_LPASS_AON_CC_PLL_L_VALUE |
		HAL_CLK_LPASS_AON_CC_PLL_PROC_CAL_L_VALUE |
			HAL_CLK_LPASS_AON_CC_PLL_RING_CAL_L_VALUE;

	pll_cfg.reg_alpha = &lpass_aon_cc_pll->pll_alpha_val;
	pll_cfg.alpha_val = HAL_CLK_LPASS_AON_CC_PLL_ALPHA_VALUE;

	pll_cfg.reg_cal_l = &lpass_aon_cc_pll->pll_l_val;

	pll_cfg.reg_config_ctl = &lpass_aon_cc_pll->pll_config_ctl;
	pll_cfg.config_ctl_val = HAL_CLK_LPASS_AON_CC_PLL_CONFIG_CTL;
	pll_cfg.reg_config_ctl_hi = &lpass_aon_cc_pll->pll_config_ctl_u;
	pll_cfg.config_ctl_hi_val = HAL_CLK_LPASS_AON_CC_PLL_CONFIG_CTL_U;
	pll_cfg.reg_config_ctl_hi1 = &lpass_aon_cc_pll->pll_config_ctl_u1;
	pll_cfg.config_ctl_hi1_val = HAL_CLK_LPASS_AON_CC_PLL_CONFIG_CTL_U1;

	pll_cfg.reg_user_ctl = &lpass_aon_cc_pll->pll_user_ctl;
	pll_cfg.user_ctl_val = HAL_CLK_LPASS_AON_CC_PLL_USER_CTL;
	pll_cfg.reg_user_ctl_hi = &lpass_aon_cc_pll->pll_user_ctl_u;
	pll_cfg.user_ctl_hi_val = HAL_CLK_LPASS_AON_CC_PLL_USER_CTL_U;

	pll_cfg.reg_opmode = &lpass_aon_cc_pll->pll_opmode;

	if (clock_configure_enable_gpll(&pll_cfg, false, 0) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: AON CC PLL configuration failed\n");
		return CB_ERR;
	}

	write32(&lpass_aon_cc_pll->pll_test_ctl, HAL_CLK_LPASS_AON_CC_PLL_TEST_CTL);
	write32(&lpass_aon_cc_pll->pll_test_ctl_u, HAL_CLK_LPASS_AON_CC_PLL_TEST_CTL_U);
	write32(&lpass_aon_cc_pll->pll_test_ctl_u1, HAL_CLK_LPASS_AON_CC_PLL_TEST_CTL_U1);
	write32(&lpass_aon_cc_pll->pll_test_ctl_u2, HAL_CLK_LPASS_AON_CC_PLL_TEST_CTL_U2);

	setbits32(&lpass_aon_cc_pll->pll_user_ctl, BIT(AON_CC_PLL_ENABLE_VOTE_RUN));
	setbits32(&lpass_aon_cc_pll->pll_mode, BIT(PLL_RESET_SHFT));

	setbits32(&lpass_aon_cc_pll->pll_user_ctl, (BIT(AON_CC_PLL_PLLOUT_EVEN_SHFT_X1P42100) |
				BIT(AON_CC_PLL_PLLOUT_ODD_SHFT_X1P42100)));

	return CB_SUCCESS;
}

static enum cb_err lpass_setup_core_infrastructure(void)
{

	if (clock_enable(&gcc->lpass_cfg_noc_sway_cbcr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable CFG NOC SWAY clock\n");
		return CB_ERR;
	}

	setbits32(&lpass_core_gdsc->lpass_top_cc_lpass_core_sway_ahb_ls_cbcr, HW_CTL);

	write32(&lpass_aon_cc->lpass_hm_collapse_vote_for_q6, LPASS_CORE_HM_VOTE_POWER_ON);

	if (!wait_us(150000, !(read32(&lpass_core_gdsc->core_hm_gdscr) & GDSC_PWR_ON))) {
		printk(BIOS_ERR, "LPASS: Core HM GDSC PWR_ON timeout after vote\n");
		return CB_ERR;
	}

	if (!wait_us(1000000, read32(&lpass_core_gdsc->lpass_core_gds_hm_ready) & LPASS_CORE_HM_READY)) {
		printk(BIOS_ERR, "LPASS: Core HM ready timeout\n");
		return CB_ERR;
	}

	return CB_SUCCESS;
}

static u32 *lpass_cbcr[LPASS_CLK_COUNT] = {
	[LPASS_CODEC_MEM_CBCR] = &lpass_audio_cc->codec_mem_cbcr,
	[LPASS_CODEC_MEM0_CBCR] = &lpass_audio_cc->codec_mem0_cbcr,
	[LPASS_CODEC_MEM1_CBCR] = &lpass_audio_cc->codec_mem1_cbcr,
	[LPASS_CODEC_MEM2_CBCR] = &lpass_audio_cc->codec_mem2_cbcr,
	[LPASS_CODEC_MEM3_CBCR] = &lpass_audio_cc->codec_mem3_cbcr,
	[LPASS_EXT_MCLK0_CBCR] = &lpass_audio_cc->codec_ext_mclk0_cbcr,
	[LPASS_EXT_MCLK1_CBCR] = &lpass_audio_cc->codec_ext_mclk1_cbcr,
	[LPASS_TX_MCLK_CBCR] = &lpass_aon_cc->tx_mclk_cbcr,
	[LPASS_TX_MCLK_2X_WSA_CBCR] = &lpass_audio_cc->tx_mclk_2x_wsa_cbcr,
	[LPASS_TX_MCLK_WSA_CBCR] = &lpass_audio_cc->tx_mclk_wsa_cbcr,
	[LPASS_WSA_MCLK_2X_CBCR] = &lpass_audio_cc->wsa_mclk_2x_cbcr,
	[LPASS_WSA_MCLK_CBCR] = &lpass_audio_cc->wsa_mclk_cbcr,
	[LPASS_TX_MCLK_2X_WSA2_CBCR] = &lpass_audio_cc->tx_mclk_2x_wsa2_cbcr,
	[LPASS_TX_MCLK_WSA2_CBCR] = &lpass_audio_cc->tx_mclk_wsa2_cbcr,
	[LPASS_WSA2_MCLK_2X_CBCR] = &lpass_audio_cc->wsa2_mclk_2x_cbcr,
	[LPASS_WSA2_MCLK_CBCR] = &lpass_audio_cc->wsa2_mclk_cbcr,
	[LPASS_VA_MEM0_CBCR] = &lpass_aon_cc->va_mem0_cbcr,
	[LPASS_VA_CBCR] = &lpass_aon_cc->va_cbcr,
	[LPASS_VA_2X_CBCR] = &lpass_aon_cc->va_2x_cbcr,
};

static enum cb_err lpass_audio_clocks_enable(void)
{
	for (size_t i = 0; i < LPASS_CLK_COUNT; i++) {
		if (clock_enable(lpass_cbcr[i]) != CB_SUCCESS) {
			printk(BIOS_ERR, "LPASS: Failed to enable audio clock\n");
			return CB_ERR;
		}
	}

	return CB_SUCCESS;
}

enum cb_err lpass_init(void)
{
	int rc;
	rc = rpmh_bcm_vote("LP0", BCM_LP0_VOTE_VALUE);
	if (rc) {
		printk(BIOS_ERR, "LPASS: Failed to send BCM vote for LPASS bus clock manager LP0\n");
		return CB_ERR;
	}

	if (enable_and_poll_gdsc_status(&lpass_aon_cc->lpass_audio_hm_gdscr) != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable Core HM GDSC\n");
		return CB_ERR;
	}

	if (lpass_setup_core_infrastructure() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to setup core infrastructure\n");
		return CB_ERR;
	}

	if (lpass_aon_cc_pll_enable() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable AON CC PLL\n");
		return CB_ERR;
	}

	if (lpass_audio_clocks_enable() != CB_SUCCESS) {
		printk(BIOS_ERR, "LPASS: Failed to enable audio clocks\n");
		return CB_ERR;
	}

	printk(BIOS_INFO, "LPASS: BCM vote for LP0 and LPASS Init completed successfully\n");
	return CB_SUCCESS;
}
