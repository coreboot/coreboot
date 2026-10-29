/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * eDP PHY/PLL bring-up for calypso's 3nm "EDP2/Glymur" QSERDES IP.
 *
 * Register values and sequences taken directly from the Glymur UEFI eDP
 * PHY driver: hal_dp_phy_2_0_0.c - HAL_DP_2_0_0_PHY_AUX_Init() and
 * HAL_DP_2_0_0_PHY_Config() (eDP-specific path, NOT hal_dp_qmp_phy_2_0_0.c).
 *
 * The UEFI function splits into two phases:
 *   Phase 1 "before link clock": PLL config + C_READY/PLL_LOCKED poll
 *   Phase 2 "after link clock":  lane enable + PHY_READY poll + DCC cal
 * We map these to edp_phy_pll_config() and edp_phy_pll_enable() respectively.
 */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/display/edp_reg.h>
#include <soc/display/edp_phy.h>
#include <timer.h>

/*
 * Per-rate parameters from gRateParams[] and gEDPSSCParams[] in
 * hal_dp_phy_2_0_0.c.  vco_div from gEDPPhyHwSettings_Frequency_Tables[Nominal].
 * ssc_en_center from gEDPSSCParams[].uSSCEnCenter (1 for RBR..HBR3, 0 for UHBR).
 */
struct edp_pll_params {
	uint32_t link_rate_khz;
	uint8_t tx_ldo_config;        /* uTx0LdoCfg = uTx1LdoCfg */
	uint8_t ldo_cfg;              /* uLdoCfg */
	uint8_t hsclk_sel;            /* uHsclkSel */
	uint8_t dec_start_mode0;      /* uDecStart */
	uint8_t div_frac_start1_mode0;
	uint8_t div_frac_start2_mode0;
	uint8_t div_frac_start3_mode0;
	uint8_t lock_cmp1_mode0;      /* uLockCmp1 */
	uint8_t lock_cmp2_mode0;      /* uLockCmp2 */
	uint8_t vco_tune1_mode0;
	uint8_t vco_tune2_mode0;
	uint8_t lock_cmp_en;          /* uLockCmpEn */
	uint8_t cmp_code1_mode0;      /* uVcocalCmpCode1 */
	uint8_t cmp_code2_mode0;      /* uVcocalCmpCode2 */
	uint8_t auxless_cfg1;         /* uAuxlessCfg1 */
	uint8_t auxless_setup_cyc;    /* uAuxlessSetup */
	uint8_t auxless_silence_cyc;  /* uAuxlessSilence */
	uint8_t lfps_cyc;             /* uLfpsCyc */
	uint8_t lfps_period;          /* uLfpsPeriod */
	uint8_t coreclk_div_mode0;    /* uCoreClkDivMode0 */
	uint8_t interface_select;     /* uInterfaceSel */
	uint8_t tran_drvr_emp_en;     /* uTranDrvrEmpEn */
	uint8_t emp_post1_lvl;        /* uEmpPost1Lvl */
	uint8_t lane_mode2;           /* uLaneMode2 */
	uint8_t aux_cfg2;             /* uAuxCfg2 */
	uint8_t cmn_config_1;         /* uCmnConfig1 */
	uint8_t ssc_step_size1_mode0; /* gEDPSSCParams.uStepSize1 */
	uint8_t ssc_step_size2_mode0; /* gEDPSSCParams.uStepSize2 */
	uint8_t ssc_en_center;        /* gEDPSSCParams.uSSCEnCenter */
	uint8_t vco_div;              /* gEDPPhyHwSettings[Nominal].uPhyVcoDiv */
};

static const struct edp_pll_params edp_pll_params_table[] = {
	{ /* RBR, 1.62 Gbps */
		.link_rate_khz         = 162000,
		.tx_ldo_config         = 0xD1,
		.ldo_cfg               = 0x03,
		.hsclk_sel             = 0x04,
		.dec_start_mode0       = 0x54,
		.div_frac_start1_mode0 = 0x00,
		.div_frac_start2_mode0 = 0x00,
		.div_frac_start3_mode0 = 0x06,
		.lock_cmp1_mode0       = 0x1c,
		.lock_cmp2_mode0       = 0x02,
		.vco_tune1_mode0       = 0xfe,
		.vco_tune2_mode0       = 0x00,
		.lock_cmp_en           = 0x04,
		.cmp_code1_mode0       = 0x8d,
		.cmp_code2_mode0       = 0x27,
		.auxless_cfg1          = 0x01,
		.auxless_setup_cyc     = 0x03,
		.auxless_silence_cyc   = 0x05,
		.lfps_cyc              = 0x08,
		.lfps_period           = 0x00,
		.coreclk_div_mode0     = 0x14,
		.interface_select      = 0x05,
		.tran_drvr_emp_en      = 0x01,
		.emp_post1_lvl         = 0x06,
		.lane_mode2            = 0x00,
		.aux_cfg2              = 0xa4,
		.cmn_config_1          = 0x76,
		.ssc_step_size1_mode0  = 0x83,
		.ssc_step_size2_mode0  = 0x02,
		.ssc_en_center         = 0x01,
		.vco_div               = 0x00,
	},
	{ /* HBR, 2.7 Gbps */
		.link_rate_khz         = 270000,
		.tx_ldo_config         = 0xD1,
		.ldo_cfg               = 0x03,
		.hsclk_sel             = 0x03,
		.dec_start_mode0       = 0x46,
		.div_frac_start1_mode0 = 0x00,
		.div_frac_start2_mode0 = 0x00,
		.div_frac_start3_mode0 = 0x05,
		.lock_cmp1_mode0       = 0x08,
		.lock_cmp2_mode0       = 0x07,
		.vco_tune1_mode0       = 0xae,
		.vco_tune2_mode0       = 0x02,
		.lock_cmp_en           = 0x08,
		.cmp_code1_mode0       = 0xf6,
		.cmp_code2_mode0       = 0x20,
		.auxless_cfg1          = 0x01,
		.auxless_setup_cyc     = 0x04,
		.auxless_silence_cyc   = 0x08,
		.lfps_cyc              = 0x08,
		.lfps_period           = 0x11,
		.coreclk_div_mode0     = 0x14,
		.interface_select      = 0x05,
		.tran_drvr_emp_en      = 0x01,
		.emp_post1_lvl         = 0x06,
		.lane_mode2            = 0x00,
		.aux_cfg2              = 0xa4,
		.cmn_config_1          = 0x96,
		.ssc_step_size1_mode0  = 0x18,
		.ssc_step_size2_mode0  = 0x02,
		.ssc_en_center         = 0x01,
		.vco_div               = 0x00,
	},
	{ /* HBR2, 5.4 Gbps */
		.link_rate_khz         = 540000,
		.tx_ldo_config         = 0xD1,
		.ldo_cfg               = 0x03,
		.hsclk_sel             = 0x01,
		.dec_start_mode0       = 0x46,
		.div_frac_start1_mode0 = 0x00,
		.div_frac_start2_mode0 = 0x00,
		.div_frac_start3_mode0 = 0x05,
		.lock_cmp1_mode0       = 0x10,
		.lock_cmp2_mode0       = 0x0e,
		.vco_tune1_mode0       = 0xae,
		.vco_tune2_mode0       = 0x02,
		.lock_cmp_en           = 0x08,
		.cmp_code1_mode0       = 0xf6,
		.cmp_code2_mode0       = 0x20,
		.auxless_cfg1          = 0x01,
		.auxless_setup_cyc     = 0x09,
		.auxless_silence_cyc   = 0x11,
		.lfps_cyc              = 0x08,
		.lfps_period           = 0x33,
		.coreclk_div_mode0     = 0x14,
		.interface_select      = 0x05,
		.tran_drvr_emp_en      = 0x01,
		.emp_post1_lvl         = 0x06,
		.lane_mode2            = 0x00,
		.aux_cfg2              = 0xa4,
		.cmn_config_1          = 0x56,
		.ssc_step_size1_mode0  = 0x18,
		.ssc_step_size2_mode0  = 0x02,
		.ssc_en_center         = 0x01,
		.vco_div               = 0x02,
	},
	{ /* HBR3, 8.1 Gbps */
		.link_rate_khz         = 810000,
		.tx_ldo_config         = 0xD1,
		.ldo_cfg               = 0x03,
		.hsclk_sel             = 0x02,
		.dec_start_mode0       = 0x4f,
		.div_frac_start1_mode0 = 0x00,
		.div_frac_start2_mode0 = 0xa0,
		.div_frac_start3_mode0 = 0x01,
		.lock_cmp1_mode0       = 0x18,
		.lock_cmp2_mode0       = 0x15,
		.vco_tune1_mode0       = 0xa0,
		.vco_tune2_mode0       = 0x01,
		.lock_cmp_en           = 0x08,
		.cmp_code1_mode0       = 0x14,
		.cmp_code2_mode0       = 0x25,
		.auxless_cfg1          = 0x01,
		.auxless_setup_cyc     = 0x0f,
		.auxless_silence_cyc   = 0x1a,
		.lfps_cyc              = 0x08,
		.lfps_period           = 0x55,
		.coreclk_div_mode0     = 0x0a,
		.interface_select      = 0x07,
		.tran_drvr_emp_en      = 0x01,
		.emp_post1_lvl         = 0x06,
		.lane_mode2            = 0x00,
		.aux_cfg2              = 0xa0,
		.cmn_config_1          = 0x96,
		.ssc_step_size1_mode0  = 0x5b,
		.ssc_step_size2_mode0  = 0x02,
		.ssc_en_center         = 0x01,
		.vco_div               = 0x01,
	},
};

static const struct edp_pll_params *edp_get_pll_params(uint32_t link_rate)
{
	for (size_t i = 0; i < ARRAY_SIZE(edp_pll_params_table); i++)
		if (edp_pll_params_table[i].link_rate_khz == link_rate)
			return &edp_pll_params_table[i];

	printk(BIOS_ERR, "%s: unsupported link rate %u, defaulting to HBR\n",
	       __func__, link_rate);
	return &edp_pll_params_table[1];
}

/*
 * HAL_DP_2_0_0_PHY_AUX_Init() from hal_dp_phy_2_0_0.c.
 *
 * eDP-specific vs. USB4/DP QMP path:
 *  - pd_ctl: 0x02 -> 0x7d  (full power-on, not 0x67)
 *  - CFG3: toggled 0x01 -> 0x00  (reset pulse)
 *  - CFG8: 0x37  (8-bit AUX preamble, avoids eDP PSR flicker)
 *  - CFG12: 0x04  (AUX_DCTRL DCC control, new in Glymur)
 *  - CFG0, CFG10, CFG11: NOT written (left at HW reset)
 */
static void edp_phy_aux_setup(void)
{
	write32(&edp_phy_pll->qserdes_com_clk_fwd_config_1, 0x3f);

	write32(&edp_phy->pd_ctl, 0x02);
	write32(&edp_phy->pd_ctl, 0x7d);

	write32(&edp_phy_pll->qserdes_com_bias_en_clkbuflr_en, 0x1f);

	write32(&edp_phy->aux_cfg[1],  0x13);
	write32(&edp_phy->aux_cfg[2],  0xa4);
	write32(&edp_phy->aux_cfg[3],  0x01); /* toggle: reset pulse */
	write32(&edp_phy->aux_cfg[3],  0x00);
	write32(&edp_phy->aux_cfg[4],  0x0a);
	write32(&edp_phy->aux_cfg[5],  0x26);
	write32(&edp_phy->aux_cfg[6],  0x0a);
	write32(&edp_phy->aux_cfg[7],  0x03);
	write32(&edp_phy->aux_cfg[8],  0x37); /* 8-bit preamble for eDP */
	write32(&edp_phy->aux_cfg[9],  0x03);
	write32(&edp_phy->aux_cfg[12], 0x04); /* AUX_DCTRL DCC ctrl, new in Glymur */

	write32(&edp_phy->aux_interrupt_mask, 0x1f);
}

void early_phy_enable(void)
{
	edp_phy_aux_setup();
}

int edp_phy_enable(uint32_t link_rate)
{
	edp_phy_aux_setup();
	return edp_phy_power_on(link_rate, 2);
}

static const uint8_t edp_voltage_swing[4][4] = {
	{0x07, 0x0f, 0x16, 0x1f},
	{0x0d, 0x16, 0x1e, 0xff},
	{0x11, 0x1b, 0xff, 0xff},
	{0x16, 0xff, 0xff, 0xff},
};
static const uint8_t edp_pre_emphasis[4][4] = {
	{0x05, 0x11, 0x17, 0x1d},
	{0x05, 0x11, 0x18, 0xff},
	{0x06, 0x11, 0xff, 0xff},
	{0x00, 0xff, 0xff, 0xff},
};

void edp_phy_config(uint8_t v_level, uint8_t p_level, uint32_t link_rate)
{
	(void)link_rate;
	write32(&edp_phy_lane_tx0->tx_drv_lvl,      edp_voltage_swing[v_level][p_level]);
	write32(&edp_phy_lane_tx0->tx_emp_post1_lvl, edp_pre_emphasis[v_level][p_level]);
	write32(&edp_phy_lane_tx1->tx_drv_lvl,      edp_voltage_swing[v_level][p_level]);
	write32(&edp_phy_lane_tx1->tx_emp_post1_lvl, edp_pre_emphasis[v_level][p_level]);
}

/* Lane TX init from HAL_DP_2_0_0_PHY_Config() lane tx0/tx1 block. */
static void edp_phy_lane_tx_config(const struct edp_pll_params *p)
{
	write32(&edp_phy_lane_tx0->tx_clk_buf_enable,             0x0f);
	write32(&edp_phy_lane_tx0->tx_pre_emph,                   0x00);
	write32(&edp_phy_lane_tx0->tx_vmode_ctrl1,                0x00);
	write32(&edp_phy_lane_tx0->tx_serdes_byp_en_out,          0x00);
	write32(&edp_phy_lane_tx0->tx_lpb0_cfg[0],                0x03);
	write32(&edp_phy_lane_tx0->tx_drv_lvl_offset,             0x10);
	write32(&edp_phy_lane_tx0->tx_interface_select,           p->interface_select);
	write32(&edp_phy_lane_tx0->tx_tran_drvr_emp_en,           p->tran_drvr_emp_en);
	write32(&edp_phy_lane_tx0->tx_emp_post1_lvl,              p->emp_post1_lvl);
	write32(&edp_phy_lane_tx0->tx_lane_mode1,                 0x00);
	write32(&edp_phy_lane_tx0->tx_lane_mode2,                 p->lane_mode2);
	write32(&edp_phy_lane_tx0->tx_drv_lvl,                    0x12);
	write32(&edp_phy_lane_tx0->tx_parrate_rec_detect_idle_en, 0x00);
	write32(&edp_phy_lane_tx0->tx_idle_lvl_large_amp,         0x00);
	write32(&edp_phy_lane_tx0->tx_reset_tsync_en,             0x03);
	write32(&edp_phy_lane_tx0->tx_tx_band,                    0x04);
	write32(&edp_phy_lane_tx0->tx_slew_cntl,                  0x00);
	write32(&edp_phy_lane_tx0->tx_ldo_config,                 0xd0);
	write32(&edp_phy_lane_tx0->tx_rescode_lane_tx,            0x60);
	write32(&edp_phy_lane_tx0->tx_rescode_lane_tx1,           0x60);
	write32(&edp_phy_lane_tx0->tx_rescode_lane_offset_tx0,    0x06);
	write32(&edp_phy_lane_tx0->tx_rescode_lane_offset_tx1,    0x06);

	write32(&edp_phy_lane_tx1->tx_clk_buf_enable,             0x0f);
	write32(&edp_phy_lane_tx1->tx_pre_emph,                   0x00);
	write32(&edp_phy_lane_tx1->tx_vmode_ctrl1,                0x00);
	write32(&edp_phy_lane_tx1->tx_serdes_byp_en_out,          0x00);
	write32(&edp_phy_lane_tx1->tx_lpb0_cfg[0],                0x03);
	write32(&edp_phy_lane_tx1->tx_drv_lvl_offset,             0x10);
	write32(&edp_phy_lane_tx1->tx_interface_select,           p->interface_select);
	write32(&edp_phy_lane_tx1->tx_tran_drvr_emp_en,           p->tran_drvr_emp_en);
	write32(&edp_phy_lane_tx1->tx_emp_post1_lvl,              p->emp_post1_lvl);
	write32(&edp_phy_lane_tx1->tx_lane_mode1,                 0x00);
	write32(&edp_phy_lane_tx1->tx_lane_mode2,                 p->lane_mode2);
	write32(&edp_phy_lane_tx1->tx_drv_lvl,                    0x12);
	write32(&edp_phy_lane_tx1->tx_parrate_rec_detect_idle_en, 0x00);
	write32(&edp_phy_lane_tx1->tx_idle_lvl_large_amp,         0x00);
	write32(&edp_phy_lane_tx1->tx_reset_tsync_en,             0x03);
	write32(&edp_phy_lane_tx1->tx_tx_band,                    0x04);
	write32(&edp_phy_lane_tx1->tx_slew_cntl,                  0x00);
	write32(&edp_phy_lane_tx1->tx_ldo_config,                 0xd0);
	write32(&edp_phy_lane_tx1->tx_rescode_lane_tx,            0x60);
	write32(&edp_phy_lane_tx1->tx_rescode_lane_tx1,           0x60);
	write32(&edp_phy_lane_tx1->tx_rescode_lane_offset_tx0,    0x06);
	write32(&edp_phy_lane_tx1->tx_rescode_lane_offset_tx1,    0x06);
}

/*
 * Phase 1: "before link clock enabled" from HAL_DP_2_0_0_PHY_Config().
 * Ends with C_READY + PLL_LOCKED poll.
 */
static int edp_phy_pll_config(const struct edp_pll_params *p)
{
	write32(&edp_phy->mode, 0xfc);

	if (!wait_us(10000, read32(&edp_phy_pll->qserdes_com_cmn_status) & BIT(7)))
		printk(BIOS_ERR, "%s: refgen not ready: 0x%x\n", __func__,
		       read32(&edp_phy_pll->qserdes_com_cmn_status));

	write32(&edp_phy_lane_tx0->tx_dcc0_ctrl, 0x06);
	write32(&edp_phy_lane_tx0->tx_dcc1_ctrl, 0x06);
	write32(&edp_phy_lane_tx1->tx_dcc0_ctrl, 0x06);
	write32(&edp_phy_lane_tx1->tx_dcc1_ctrl, 0x06);

	write32(&edp_phy_pll->qserdes_com_bias_en_clkbuflr_en, 0x1f);

	write32(&edp_phy_lane_tx0->tx_ldo_config, p->tx_ldo_config);
	write32(&edp_phy_lane_tx1->tx_ldo_config, p->tx_ldo_config);
	write32(&edp_phy->ldo_cfg, p->ldo_cfg);

	write32(&edp_phy_lane_tx0->tx_lane_mode1, 0x00);
	write32(&edp_phy_lane_tx1->tx_lane_mode1, 0x00);

	write32(&edp_phy->cfg1, 0x0f);
	write32(&edp_phy->cfg1, 0x2f);
	write32(&edp_phy->pd_ctl, 0x7d);

	/* AUX CFG re-applied for mainlink (CFG1=0x00, CFG8=0x37 eDP) */
	write32(&edp_phy->aux_cfg[1], 0x00);
	write32(&edp_phy->aux_cfg[3], 0x00);
	write32(&edp_phy->aux_cfg[4], 0x0a);
	write32(&edp_phy->aux_cfg[5], 0x26);
	write32(&edp_phy->aux_cfg[6], 0x0a);
	write32(&edp_phy->aux_cfg[7], 0x03);
	write32(&edp_phy->aux_cfg[8], 0x37);
	write32(&edp_phy->aux_cfg[9], 0x03);
	write32(&edp_phy->aux_interrupt_mask, 0x1f);

	write32(&edp_phy_pll->qserdes_com_cmn_config_1,   0x12);
	write32(&edp_phy_pll->qserdes_com_ssc_en_center,  0x01);
	write32(&edp_phy_pll->qserdes_com_ssc_adj_per1,   0x00);

	write32(&edp_phy_pll->qserdes_com_svs_mode_clk_sel,  0x01);
	write32(&edp_phy_pll->qserdes_com_sysclk_en_sel,     0x0b);
	write32(&edp_phy_pll->qserdes_com_sys_clk_ctrl,      0x02);
	write32(&edp_phy_pll->qserdes_com_clk_enable1,       0x0c);
	write32(&edp_phy_pll->qserdes_com_sysclk_buf_enable, 0x06);
	write32(&edp_phy_pll->qserdes_com_clk_sel,           0x30);
	write32(&edp_phy_pll->qserdes_com_hsclk_sel,         p->hsclk_sel);

	write32(&edp_phy_pll->qserdes_com_pll_ivco, 0x07);
	write32(&edp_phy_pll->qserdes_com_lock_cmp_en, p->lock_cmp_en);

	write32(&edp_phy_pll->qserdes_com_pll_cctrl_mode0, 0x36);
	write32(&edp_phy_pll->qserdes_com_pll_rctrl_mode0, 0x16);
	write32(&edp_phy_pll->qserdes_com_cp_ctrl_mode0,   0x06);

	write32(&edp_phy_pll->qserdes_com_hsclk_sel,            p->hsclk_sel);
	write32(&edp_phy_pll->qserdes_com_ssc_step_size1_mode0, p->ssc_step_size1_mode0);
	write32(&edp_phy_pll->qserdes_com_ssc_step_size2_mode0, p->ssc_step_size2_mode0);
	write32(&edp_phy_pll->qserdes_com_cp_ctrl_mode0,        0x06);
	write32(&edp_phy_pll->qserdes_com_pll_rctrl_mode0,      0x16);
	write32(&edp_phy_pll->qserdes_com_pll_cctrl_mode0,      0x36);
	write32(&edp_phy_pll->qserdes_com_coreclk_div_mode0,    p->coreclk_div_mode0);
	write32(&edp_phy_pll->qserdes_com_lock_cmp1_mode0,      p->lock_cmp1_mode0);
	write32(&edp_phy_pll->qserdes_com_lock_cmp2_mode0,      p->lock_cmp2_mode0);
	write32(&edp_phy_pll->qserdes_com_dec_start_mode0,      p->dec_start_mode0);
	write32(&edp_phy_pll->qserdes_com_div_frac_start1_mode0, p->div_frac_start1_mode0);
	write32(&edp_phy_pll->qserdes_com_div_frac_start2_mode0, p->div_frac_start2_mode0);
	write32(&edp_phy_pll->qserdes_com_div_frac_start3_mode0, p->div_frac_start3_mode0);

	write32(&edp_phy_pll->qserdes_com_integloop_gain0_mode0, 0x3f);
	write32(&edp_phy_pll->qserdes_com_integloop_gain1_mode0, 0x00);
	write32(&edp_phy_pll->qserdes_com_vco_tune1_mode0,       p->vco_tune1_mode0);
	write32(&edp_phy_pll->qserdes_com_vco_tune2_mode0,       p->vco_tune2_mode0);
	write32(&edp_phy_pll->qserdes_com_bg_timer,              0x0a);
	write32(&edp_phy_pll->qserdes_com_ssc_en_center,         p->ssc_en_center);
	write32(&edp_phy_pll->qserdes_com_ssc_per1,              0x6b);
	write32(&edp_phy_pll->qserdes_com_ssc_per2,              0x02);
	write32(&edp_phy_pll->qserdes_com_clk_enable1,           0x0c);
	write32(&edp_phy_pll->qserdes_com_sysclk_en_sel,         0x3b);
	write32(&edp_phy_pll->qserdes_com_lock_cmp_en,           p->lock_cmp_en);
	write32(&edp_phy_pll->qserdes_com_vco_tune_map,          0x00);
	write32(&edp_phy_pll->qserdes_com_clk_sel,               0x30);

	if (p->link_rate_khz == 162000)
		write32(&edp_phy_pll->qserdes_com_core_clk_en, 0x60);
	else
		write32(&edp_phy_pll->qserdes_com_core_clk_en, 0x00);

	write32(&edp_phy_pll->qserdes_com_cmn_config_1,              p->cmn_config_1);
	write32(&edp_phy_pll->qserdes_com_clk_fwd_config_1,          0x3f);
	write32(&edp_phy_pll->qserdes_com_vco_tune_ctrl,             0x00);
	write32(&edp_phy_pll->qserdes_com_bin_vcocal_cmp_code1_mode0, p->cmp_code1_mode0);
	write32(&edp_phy_pll->qserdes_com_bin_vcocal_cmp_code2_mode0, p->cmp_code2_mode0);

	write32(&edp_phy->tx0_tx1_lane_ctl, 0x05);
	write32(&edp_phy->tx2_tx3_lane_ctl, 0x05);

	edp_phy_lane_tx_config(p);

	write32(&edp_phy->auxless_cfg1,        p->auxless_cfg1);
	write32(&edp_phy->auxless_setup_cyc,   p->auxless_setup_cyc);
	write32(&edp_phy->auxless_silence_cyc, p->auxless_silence_cyc);
	write32(&edp_phy->lfps_cyc,            p->lfps_cyc);
	write32(&edp_phy->lfps_period,         p->lfps_period);

	/* VCO_DIV before AUX_CFG2 - matches UEFI dp_pcs_config_3 order */
	write32(&edp_phy->vco_div,    p->vco_div);
	write32(&edp_phy->aux_cfg[2], p->aux_cfg2);

	/* CFG dance: uCfg0..3 = 0x01,0x05,0x01,0x09 for all rates */
	write32(&edp_phy->cfg, 0x01);
	write32(&edp_phy->cfg, 0x05);
	write32(&edp_phy->cfg, 0x01);
	write32(&edp_phy->cfg, 0x09);

	write32(&edp_phy_pll->qserdes_com_resetsm_cntrl, 0x20);

	if (!wait_us(10000, read32(&edp_phy_pll->qserdes_com_c_ready_status) & BIT(0))) {
		printk(BIOS_ERR, "%s: C_READY timeout, status=0x%x\n", __func__,
		       read32(&edp_phy_pll->qserdes_com_c_ready_status));
		return -1;
	}

	if (!wait_us(10000, read32(&edp_phy_pll->qserdes_com_cmn_status) &
	             (BIT(1) | BIT(0)))) {
		printk(BIOS_ERR, "%s: PLL not locked, status=0x%x\n", __func__,
		       read32(&edp_phy_pll->qserdes_com_cmn_status));
		return -1;
	}

	return 0;
}

/*
 * Phase 2: "after link clock enabled" from HAL_DP_2_0_0_PHY_Config().
 * Lane bias/driver enable, PHY_READY poll, DCC calibration.
 * uCfg4..6 from gRateParams = 0x19, 0x18, 0x19 for all HBR rates.
 */
static int edp_phy_pll_enable(uint32_t lane_count)
{
	/* highz_drvr_en first, then transceiver_bias_en, then highz_drvr_en again */
	if (lane_count == 4) {
		write32(&edp_phy_lane_tx0->tx_highz_drvr_en,       0x1f);
		write32(&edp_phy_lane_tx0->tx_transceiver_bias_en, 0x03);
		write32(&edp_phy_lane_tx0->tx_highz_drvr_en,       0x04);
		write32(&edp_phy_lane_tx0->tx_tx_pol_inv,          0x00);
		write32(&edp_phy_lane_tx1->tx_highz_drvr_en,       0x1f);
		write32(&edp_phy_lane_tx1->tx_transceiver_bias_en, 0x03);
		write32(&edp_phy_lane_tx1->tx_highz_drvr_en,       0x04);
		write32(&edp_phy_lane_tx1->tx_tx_pol_inv,          0x00);
	} else {
		write32(&edp_phy_lane_tx0->tx_highz_drvr_en,       0x1b);
		write32(&edp_phy_lane_tx0->tx_transceiver_bias_en, 0x03);
		write32(&edp_phy_lane_tx0->tx_highz_drvr_en,       0x04);
		write32(&edp_phy_lane_tx1->tx_highz_drvr_en,       0x1b);
		write32(&edp_phy_lane_tx1->tx_transceiver_bias_en, 0x03);
		write32(&edp_phy_lane_tx1->tx_highz_drvr_en,       0x04);
		write32(&edp_phy_lane_tx1->tx_idle_lvl_large_amp,  0x50);
		write32(&edp_phy_lane_tx1->tx_tx_pol_inv,          0x00);
	}

	/* uCfg6 = 0x19 */
	write32(&edp_phy->cfg, 0x19);

	/* HAL_DP_PHY_LaneCountConfig(): aLaneCountSettings[] */
	uint32_t bias_en0, bias_en1, drvr_en0, drvr_en1, phy_cfg_1;

	switch (lane_count) {
	case 1:
		bias_en0 = 0x01; bias_en1 = 0x00;
		drvr_en0 = 0x06; drvr_en1 = 0x07;
		phy_cfg_1 = 0x01;
		break;
	case 2:
		bias_en0 = 0x03; bias_en1 = 0x00;
		drvr_en0 = 0x04; drvr_en1 = 0x07;
		phy_cfg_1 = 0x03;
		break;
	default: /* 4 lanes */
		bias_en0 = 0x03; bias_en1 = 0x03;
		drvr_en0 = 0x04; drvr_en1 = 0x04;
		phy_cfg_1 = 0x0f;
		break;
	}

	write32(&edp_phy_lane_tx0->tx_transceiver_bias_en, bias_en0);
	write32(&edp_phy_lane_tx1->tx_transceiver_bias_en, bias_en1);
	write32(&edp_phy_lane_tx0->tx_highz_drvr_en,       drvr_en0);
	write32(&edp_phy_lane_tx1->tx_highz_drvr_en,       drvr_en1);
	write32(&edp_phy->cfg1,                            phy_cfg_1);

	if (lane_count == 2)
		write32(&edp_phy_lane_tx1->tx_idle_lvl_large_amp, 0x50);

	if (!wait_us(10000, read32(&edp_phy->status) & BIT(1))) {
		printk(BIOS_ERR, "%s: PHY not ready, status=0x%x\n", __func__,
		       read32(&edp_phy->status));
		return -1;
	}

	/* uCfg5=0x18, uCfg6=0x19 after PHY_READY */
	write32(&edp_phy->cfg, 0x18);
	write32(&edp_phy->cfg, 0x19);

	/* DCC calibration */
	write32(&edp_phy_lane_tx0->tx_dcc0_ctrl, 0x07);
	write32(&edp_phy_lane_tx0->tx_dcc1_ctrl, 0x07);
	write32(&edp_phy_lane_tx1->tx_dcc0_ctrl, 0x07);
	write32(&edp_phy_lane_tx1->tx_dcc1_ctrl, 0x07);

	udelay(200);

	if ((read32(&edp_phy_lane_tx0->tx_dcc_done) & 0x03) != 0x03)
		printk(BIOS_ERR, "%s: TX0 DCC cal timeout\n", __func__);
	if ((read32(&edp_phy_lane_tx1->tx_dcc_done) & 0x03) != 0x03)
		printk(BIOS_ERR, "%s: TX1 DCC cal timeout\n", __func__);

	write32(&edp_phy_lane_tx0->tx_dcc0_ctrl, 0x06);
	write32(&edp_phy_lane_tx0->tx_dcc1_ctrl, 0x06);
	write32(&edp_phy_lane_tx1->tx_dcc0_ctrl, 0x06);
	write32(&edp_phy_lane_tx1->tx_dcc1_ctrl, 0x06);

	return 0;
}

int edp_phy_power_on(uint32_t link_rate, uint32_t lane_count)
{
	const struct edp_pll_params *p = edp_get_pll_params(link_rate);

	if (edp_phy_pll_config(p) != 0)
		return -1;

	return edp_phy_pll_enable(lane_count);
}
