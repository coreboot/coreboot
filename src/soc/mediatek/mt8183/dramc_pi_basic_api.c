/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <delay.h>
#include <soc/emi.h>
#include <soc/spm.h>
#include <soc/dramc_register.h>
#include <soc/dramc_pi_api.h>

u8 get_freq_fsq(u8 freq)
{
	if (freq == LP4X_DDR1600 || freq == LP4X_DDR2400)
		return FSP_0;
	else
		return FSP_1;
}

static void dramc_sw_imp_cal_vref_sel(u8 term_option, u8 impcal_stage)
{
	u8 vref_sel = 0;

	if (term_option == ODT_ON)
		vref_sel = IMP_LP4X_TERM_VREF_SEL;
	else {
		switch (impcal_stage) {
		case IMPCAL_STAGE_DRVP:
			vref_sel = IMP_DRVP_LP4X_UNTERM_VREF_SEL;
			break;
		case IMPCAL_STAGE_DRVN:
			vref_sel = IMP_DRVN_LP4X_UNTERM_VREF_SEL;
			break;
		default:
			vref_sel = IMP_TRACK_LP4X_UNTERM_VREF_SEL;
			break;
		}
	}

	clrsetbits32(&ch[0].phy.shu[0].ca_cmd[11], 0x3f << 8, vref_sel << 8);
}

void dramc_sw_impedance_cal(const struct sdram_params *params, u8 term,
			    struct dram_impedance *impedance)
{
	u32 broadcast_bak, impcal_bak, imp_cal_result;
	u32 DRVP_result = 0xff, ODTN_result = 0xff, DRVN_result = 0x9;

	broadcast_bak = dramc_get_broadcast();
	dramc_set_broadcast(DRAMC_BROADCAST_OFF);

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		clrbits32(&ch[chn].phy.misc_spm_ctrl1, 0xf << 0);
		write32(&ch[chn].phy.misc_spm_ctrl2, 0x0);
		write32(&ch[chn].phy.misc_spm_ctrl0, 0x0);
		clrbits32(&ch[chn].ao.impcal, 0x1 << 31);
	}

	impcal_bak = read32(&ch[0].ao.impcal);
	dramc_sw_imp_cal_vref_sel(term, IMPCAL_STAGE_DRVP);
	clrbits32(&ch[0].phy.misc_imp_ctrl1, 0x1 << 6);
	clrsetbits32(&ch[0].ao.impcal, 0x1 << 21, 0x3 << 24);
	clrsetbits32(&ch[0].phy.misc_imp_ctrl0, 0x7 << 4, 0x3 << 4);
	udelay(1);

	dramc_dbg("impedance: K DRVP\n");
	setbits32(&ch[0].ao.impcal, 0x1 << 23);
	setbits32(&ch[0].ao.impcal, 0x1 << 22);
	clrbits32(&ch[0].ao.impcal, 0x1 << 21);
	clrbits32(&ch[0].ao.shu[0].impcal1, 0x1f << 4 | 0x1f << 11);
	clrsetbits32(&ch[0].phy.shu[0].ca_cmd[11], 0xff << 0, 0x3);

	for (u8 impx_drv = 0; impx_drv < 32; impx_drv++) {
		impx_drv = (impx_drv == 16) ? 29 : impx_drv;

		clrsetbits32(&ch[0].ao.shu[0].impcal1,
				0x1f << 4, impx_drv << 4);
		udelay(1);
		imp_cal_result = (read32(&ch[0].phy_nao.misc_phy_rgs_cmd) >>
				  24) & 0x1;
		dramc_dbg("1. OCD DRVP=%d CALOUT=%d\n",
			  impx_drv, imp_cal_result);

		if (imp_cal_result == 1 && DRVP_result == 0xff) {
			DRVP_result = impx_drv;
			dramc_dbg("1. OCD DRVP calibration OK! DRVP=%d\n",
				  DRVP_result);
			break;
		}
	}

	dramc_dbg("impedance: K ODTN\n");
	dramc_sw_imp_cal_vref_sel(term, IMPCAL_STAGE_DRVN);
	clrbits32(&ch[0].ao.impcal, 0x1 << 22);
	if (term == ODT_ON)
		setbits32(&ch[0].ao.impcal, 0x1 << 21);
	clrsetbits32(&ch[0].ao.shu[0].impcal1, 0x1f << 4 | 0x1f << 11,
		DRVP_result << 4);
	clrsetbits32(&ch[0].phy.shu[0].ca_cmd[11], 0xff << 0, 0x3);

	for (u8 impx_drv = 0; impx_drv < 32; impx_drv++) {
		impx_drv = (impx_drv == 16) ? 29 : impx_drv;

		clrsetbits32(&ch[0].ao.shu[0].impcal1,
				0x1f << 11, impx_drv << 11);
		udelay(1);
		imp_cal_result = (read32(&ch[0].phy_nao.misc_phy_rgs_cmd) >>
				  24) & 0x1;
		dramc_dbg("3. OCD ODTN=%d CALOUT=%d\n",
			  impx_drv, imp_cal_result);

		if (imp_cal_result == 0 && ODTN_result == 0xff) {
			ODTN_result = impx_drv;
			dramc_dbg("3. OCD ODTN calibration OK! ODTN=%d\n",
				  ODTN_result);
			break;
		}
	}

	write32(&ch[0].ao.impcal, impcal_bak);

	dramc_dbg("impedance: term=%d, DRVP=%d, DRVN=%d, ODTN=%d\n",
		  term, DRVP_result, DRVN_result, ODTN_result);
	u32 *imp = impedance->data[term];
	if (term == ODT_OFF) {
		imp[0] = DRVP_result;
		imp[1] = ODTN_result;
		imp[2] = 0;
		imp[3] = 15;
	} else {
		imp[0] = (DRVP_result <= 3) ? (DRVP_result * 3) : DRVP_result;
		imp[1] = (DRVN_result <= 3) ? (DRVN_result * 3) : DRVN_result;
		imp[2] = 0;
		imp[3] = (ODTN_result <= 3) ? (ODTN_result * 3) : ODTN_result;
	}
	dramc_sw_imp_cal_vref_sel(term, IMPCAL_STAGE_TRACKING);

	dramc_set_broadcast(broadcast_bak);
}

void dramc_sw_impedance_save_reg(u8 freq_group,
				 const struct dram_impedance *impedance)
{
	u8 ca_term = ODT_OFF, dq_term = ODT_ON;
	u32 sw_impedance[2][4] = {0};

	if (get_freq_fsq(freq_group) == FSP_0)
		dq_term = ODT_OFF;

	for (u8 term = 0; term < 2; term++)
		for (u8 i = 0; i < 4; i++)
			sw_impedance[term][i] = impedance->data[term][i];

	sw_impedance[ODT_OFF][2] = sw_impedance[ODT_ON][2];
	sw_impedance[ODT_OFF][3] = sw_impedance[ODT_ON][3];

	/* DQ */
	clrsetbits32(&ch[0].ao.shu[0].drving[0], (0x1f << 5) | (0x1f << 0),
		(sw_impedance[dq_term][0] << 5) |
		(sw_impedance[dq_term][1] << 0));
	clrsetbits32(&ch[0].ao.shu[0].drving[1],
		(0x1f << 25) | (0x1f << 20) | (1 << 31),
		(sw_impedance[dq_term][0] << 25) |
		(sw_impedance[dq_term][1] << 20) | (!dq_term << 31));
	clrsetbits32(&ch[0].ao.shu[0].drving[2], (0x1f << 5) | (0x1f << 0),
		(sw_impedance[dq_term][2] << 5) |
		(sw_impedance[dq_term][3] << 0));
	clrsetbits32(&ch[0].ao.shu[0].drving[3], (0x1f << 25) | (0x1f << 20),
		(sw_impedance[dq_term][2] << 25) |
		(sw_impedance[dq_term][3] << 20));

	/* DQS */
	for (u8 i = 0; i <= 2; i += 2) {
		clrsetbits32(&ch[0].ao.shu[0].drving[i],
			(0x1f << 25) | (0x1f << 20),
			(sw_impedance[dq_term][i] << 25) |
			(sw_impedance[dq_term][i + 1] << 20));
		clrsetbits32(&ch[0].ao.shu[0].drving[i],
			(0x1f << 15) | (0x1f << 10),
			(sw_impedance[dq_term][i] << 15) |
			(sw_impedance[dq_term][i + 1] << 10));
	}

	/* CMD & CLK */
	for (u8 i = 1; i <= 3; i += 2) {
		clrsetbits32(&ch[0].ao.shu[0].drving[i],
			(0x1f << 15) | (0x1f << 10),
			(sw_impedance[ca_term][i - 1] << 15) |
			(sw_impedance[ca_term][i] << 10));
		clrsetbits32(&ch[0].ao.shu[0].drving[i],
			(0x1f << 5) | (0x1f << 0),
			(sw_impedance[ca_term][i - 1] << 5) |
			(sw_impedance[ca_term][i] << 0));
	}

	clrsetbits32(&ch[0].phy.shu[0].ca_cmd[11], 0x1f << 17,
		sw_impedance[ca_term][0] << 17);
	clrsetbits32(&ch[0].phy.shu[0].ca_cmd[11], 0x1f << 22,
		sw_impedance[ca_term][1] << 22);

	SET32_BITFIELDS(&ch[0].phy.shu[0].ca_cmd[3],
			SHU1_CA_CMD3_RG_TX_ARCMD_PU_PRE, 1);
	SET32_BITFIELDS(&ch[0].phy.shu[0].ca_cmd[0],
			SHU1_CA_CMD0_RG_TX_ARCLK_DRVN_PRE, 0);

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	clrsetbits32(&ch[0].phy.shu[0].ca_dll[1], 0x1f << 16, 0x9 << 16);
	clrsetbits32(&ch[1].phy.shu[0].ca_dll[1], 0x1f << 16, 0x9 << 16);
	dramc_set_broadcast(DRAMC_BROADCAST_ON);
}

static void transfer_pll_to_spm_control(void)
{
	u8 shu_lev = (read32(&ch[0].ao.shustatus) >> 1) & 0x3;

	clrsetbits32(&mtk_spm->poweron_config_set,
		(0xffff << 16) | (0x1 << 0),
		(0xb16 << 16) | (0x1 << 0));

	/* Set SPM pinmux */
	clrbits32(&mtk_spm->pcm_pwr_io_en, (0xff << 0) | (0xff << 16));
	setbits32(&mtk_spm->dramc_dpy_clk_sw_con_sel, 0xffffffff);
	setbits32(&mtk_spm->dramc_dpy_clk_sw_con_sel2, 0xffffffff);

	setbits32(&mtk_spm->spm_power_on_val0, (0x1 << 8) | (0xf << 12));
	setbits32(&mtk_spm->spm_s1_mode_ch, 0x3 << 0);

	shu_lev = (shu_lev == 1) ? 2 : 1;
	clrsetbits32(&mtk_spm->spm_power_on_val0, 0x3 << 28, shu_lev << 28);
	clrsetbits32(&mtk_spm->dramc_dpy_clk_sw_con2,
		0x3 << 2, shu_lev << 2);

	udelay(1);
	for (size_t chn = CHANNEL_A; chn < CHANNEL_MAX; chn++) {
		clrbits32(&ch[chn].phy.pll1, 0x1 << 31);
		clrbits32(&ch[chn].phy.pll2, 0x1 << 31);
	}
}

static void dramc_rx_input_delay_tracking(u8 chn)
{
	/* Enable RX_FIFO macro DIV4 clock CG */
	write32(&ch[chn].phy.misc_cg_ctrl1, 0xffffffff);

	/* DVS mode to RG mode */
	for (size_t r = 0; r < 2; r++)
		for (size_t b = 0; b < 2; b++)
			clrbits32(&ch[chn].phy.r[r].b[b].rxdvs[2], 3 << 30);

	clrsetbits32(&ch[chn].phy.b0_rxdvs[0], 0x1 << 19, 0x1 << 9);
	clrsetbits32(&ch[chn].phy.b1_rxdvs[0], 0x1 << 19, 0x1 << 9);

	for (size_t r = 0; r < 2; r++)
		for (size_t b = 0; b < 2; b++) {
			clrbits32(&ch[chn].phy.r[r].b[b].rxdvs[2], 1 << 29);
			clrsetbits32(&ch[chn].phy.r[r].b[b].rxdvs[7],
				(0x3f << 0) | (0x3f << 8) |
				(0x7f << 16) | (0x7f << 24),
				(0x0 << 0) | (0x3f << 8) |
				(0x0 << 16) | (0x7f << 24));
			clrsetbits32(&ch[chn].phy.r[r].b[b].rxdvs[1],
				(0xffff << 16) | (0xffff << 0),
				(0x2 << 16) | (0x2 << 0));

			/* DQ/DQS Rx DLY adjustment for tracking mode */
			clrbits32(&ch[chn].phy.r[r].b[b].rxdvs[2],
				(0x3 << 26) | (0x3 << 24) |
				(0x3 << 18) | (0x3 << 16));
		}

	/* Rx DLY tracking setting (Static) */
	clrsetbits32(&ch[chn].phy.b0_rxdvs[0],
		(0x1 << 29) | (0xf << 4) | (0x1 << 0),
		(0x1 << 29) | (0x0 << 4) | (0x1 << 0));
	clrsetbits32(&ch[chn].phy.b1_rxdvs[0],
		(0x1 << 29) | (0xf << 4) | (0x1 << 0),
		(0x1 << 29) | (0x0 << 4) | (0x1 << 0));

	for (u8 b = 0; b < 2; b++)
		clrsetbits32(&ch[chn].phy.b[b].dq[9],
			     (0x7 << 28) | (0x7 << 24),
			     (0x1 << 28) | (0x0 << 24));
	clrbits32(&ch[chn].phy.ca_cmd[10], (0x7 << 28) | (0x7 << 24));
	for (u8 b = 0; b < 2; b++)
		setbits32(&ch[chn].phy.b[b].dq[5], 0x1 << 31);

	setbits32(&ch[chn].phy.b0_rxdvs[0], (0x1 << 28) | (0x1 << 31));
	setbits32(&ch[chn].phy.b1_rxdvs[0], (0x1 << 28) | (0x1 << 31));
	for (u8 rank = RANK_0; rank < RANK_MAX; rank++)
		for (u8 b = 0; b < 2; b++)
			clrsetbits32(&ch[chn].phy.r[rank].b[b].rxdvs[2],
				(0x3 << 30) | (0x1 << 28) | (0x1 << 23),
				(0x2 << 30) | (0x1 << 28) | (0x1 << 23));

}

static void dramc_hw_dqs_gating_tracking(u8 chn)
{
	clrsetbits32(&ch[chn].ao.stbcal,
		(0x1 << 21) | (0x3 << 15) | (0x1f << 8) | (0x1 << 4),
		(0x3 << 26) | (0x1 << 0));
	clrsetbits32(&ch[chn].ao.stbcal1,
		(0xffff << 16) | (0x1 << 8) | (0x1 << 6),
		(0x1 << 16) | (0x1 << 8) | (0x1 << 6));

	clrsetbits32(&ch[chn].phy.misc_ctrl0,
		(0x1 << 24) | (0x1f << 11) | (0xf << 0),
		(0x1 << 24) | (0x0 << 11) | (0x0 << 0));

	clrbits32(&ch[chn].phy.b[0].dq[6], 0x1 << 31);
	clrbits32(&ch[chn].phy.b[1].dq[6], 0x1 << 31);
	clrbits32(&ch[chn].phy.ca_cmd[6], 0x1 << 31);
}

static void dramc_hw_gating_init(u8 chn)
{
	clrbits32(&ch[chn].ao.stbcal,
		  (0x7 << 22) | (0x3 << 14) | (0x1 << 19) | (0x1 << 21));
	setbits32(&ch[chn].ao.stbcal, (0x1 << 20) | (0x3 << 28));
	setbits32(&ch[chn].phy.misc_ctrl1, 0x1 << 24);

	dramc_hw_dqs_gating_tracking(chn);
}

static void dramc_impedance_tracking_enable(void)
{
	setbits32(&ch[0].phy.misc_ctrl0, 0x1 << 10);
	for (size_t chn = 0; chn < CHANNEL_MAX; chn++) {
		setbits32(&ch[chn].ao.impcal, (0x1 << 31) | (0x1 << 29) |
			(0x1 << 26) | (0x1 << 17) | (0x7 << 11));
		clrbits32(&ch[chn].ao.impcal, 0x1 << 30);
		setbits32(&ch[chn].phy.misc_ctrl0, 0x1 << 18);
		setbits32(&ch[chn].ao.impcal, 0x1 << 19);
	}
	setbits32(&ch[0].ao.impcal, 0x1 << 14);
	for (size_t chn = 0; chn < CHANNEL_MAX; chn++)
		setbits32(&ch[chn].ao.refctrl0, (0x1 << 2) | (0x1 << 3));
}

static void dramc_phy_low_power_enable(u8 chn)
{
	for (u8 b = 0; b < 2; b++) {
		clrbits32(&ch[chn].phy.b[b].dll_fine_tune[2], 0x3fffff << 10);
		write32(&ch[chn].phy.b[b].dll_fine_tune[3], 0x2e800);
	}
	clrsetbits32(&ch[chn].phy.ca_dll_fine_tune[2],
		     0x3fffff << 10, 0x2 << 10);
	write32(&ch[chn].phy.ca_dll_fine_tune[3],
		(chn == CHANNEL_A) ? 0xba000 : 0x3a000);
}

static void dramc_dummy_read_for_tracking_enable(u8 chn, u32 rk_num)
{
	setbits32(&ch[chn].ao.dummy_rd, rk_num << 16);

	for (size_t r = 0; r < 2; r++)
		for (size_t i = 0; i < 4; i++)
			write32(&ch[chn].ao.rk[r].dummy_rd_wdata[i],
				0xaaaa5555);

	clrsetbits32(&ch[chn].ao.test2_4, 0x7 << 28, 0x4 << 28);
	for (size_t r = 0; r < 2; r++) {
		clrsetbits32(&ch[chn].ao.rk[r].dummy_rd_adr,
			(0x1ffff << 0) | (0x7ff << 17) | (0xf << 28),
			(0xffff << 0) | (0x3f0 << 17));
		clrbits32(&ch[chn].ao.rk[r].dummy_rd_bk, 0x7 << 0);
	}

	clrbits32(&ch[chn].ao.dummy_rd, 0x1 << 25 | 0x1 << 20);
}

static void dramc_set_CKE_2_rank_independent(u8 chn)
{
	clrsetbits32(&ch[chn].ao.rkcfg, (0x1 << 15) | (0x1 << 12), 0x1 << 2);
	clrsetbits32(&ch[chn].ao.ckectrl,
		(0x1 << 1) | (0xf << 8) | (0x7 << 13),
		(0x4 << 8) | (0x2 << 13));

	for (u8 shu = 0; shu < DRAM_DFS_SHUFFLE_MAX; shu++)
		setbits32(&ch[chn].ao.shu[shu].conf[2],
			(0x1 << 29) | (0x1 << 31));
	clrbits32(&ch[chn].ao.dramctrl, 0x1 << 9);
}

static void dramc_pa_improve(u8 chn)
{
	clrbits32(&ch[chn].ao.clkar, 0xffff);
	clrbits32(&ch[chn].ao.srefctrl, 0xf << 12);
	clrbits32(&ch[chn].ao.zqcs, 0x1 << 19);
	clrbits32(&ch[chn].ao.pre_tdqsck[0], 0x1 << 17);
	clrbits32(&ch[chn].ao.zqcs, 0x1 << 19);
	clrbits32(&ch[chn].ao.pre_tdqsck[0], 0x1 << 17);

	for (u8 shu = 0; shu < DRAM_DFS_SHUFFLE_MAX; shu++)
		clrbits32(&ch[chn].ao.shu[shu].odtctrl, 0x3 << 2);
}

static void dramc_enable_dramc_dcm(void)
{
	for (size_t chn = 0; chn < CHANNEL_MAX; chn++) {
		clrsetbits32(&ch[chn].ao.dramc_pd_ctrl,
			(0x7 << 0) | (0x1 << 26) | (0x1 << 30) | (0x1 << 31),
			(0x7 << 0) | (0x1 << 30) | (0x1 << 31));
		setbits32(&ch[chn].ao.clkar, 0x1 << 31);
	}
}

void dramc_runtime_config(u32 rk_num)
{
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
		clrbits32(&ch[chn].ao.refctrl0, 0x1 << 29);

	transfer_pll_to_spm_control();
	setbits32(&mtk_spm->spm_power_on_val0, 0x1 << 25);

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		dramc_hw_dqsosc(chn, rk_num);

		/* RX_TRACKING: ON */
		dramc_rx_input_delay_tracking(chn);

		/* HW_GATING: ON */
		dramc_hw_gating_init(chn);
		dramc_hw_gating_onoff(chn, true);

		/* HW_GATING DBG: OFF */
		clrbits32(&ch[chn].ao.stbcal2,
			  (0x3 << 4) | (0x3 << 8) | (0x1 << 28));

		/* DUMMY_READ_FOR_TRACKING: ON */
		dramc_dummy_read_for_tracking_enable(chn, rk_num);

		/* ZQCS_ENABLE_LP4: ON */
		clrbits32(&ch[chn].ao.spcmdctrl, 0x1 << 30);

		/* LOWPOWER_GOLDEN_SETTINGS(DCM): ON */
		dramc_phy_low_power_enable(chn);
		dramc_enable_phy_dcm(chn, true);

		/* DUMMY_READ_FOR_DQS_GATING_RETRY: OFF */
		for (size_t shu = 0; shu < DRAM_DFS_SHUFFLE_MAX; shu++)
			clrbits32(&ch[chn].ao.shu[shu].dqsg_retry,
				  (0x1 << 1) | (0x3 << 13));
	}

	/* SPM_CONTROL_AFTERK: ON */
	write32(&ch[0].phy.misc_spm_ctrl0, 0xfbffefff);
	write32(&ch[1].phy.misc_spm_ctrl0, 0xfbffefff);
	write32(&ch[0].phy.misc_spm_ctrl2, 0xffffffef);
	write32(&ch[1].phy.misc_spm_ctrl2, 0x7fffffef);

	/* IMPEDANCE_TRACKING: ON */
	dramc_impedance_tracking_enable();

	for (size_t chn = 0; chn < CHANNEL_MAX; chn++) {
		/* TEMP_SENSOR: ON */
		clrbits32(&ch[chn].ao.spcmdctrl, 0x3 << 28);
		setbits32(&ch[chn].ao.hw_mrr_fun, (0x1 << 0) | (0x1 << 11));

		/* PER_BANK_REFRESH: ON */
		clrbits32(&ch[chn].ao.refctrl0, 0x1 << 18);

		/* HW_SAVE_FOR_SR: ON */
		clrbits32(&ch[chn].ao.rstmask, (0x1 << 25) | (0x1 << 28));
		setbits32(&ch[chn].ao.refctrl1, 0x1 << 0);
		clrsetbits32(&ch[chn].ao.srefctrl, 0x1 << 20, 0x1 << 22);

		/* SET_CKE_2_RANK_INDEPENDENT_RUN_TIME: ON */
		dramc_set_CKE_2_rank_independent(chn);

		/* CLK_FREE_FUN_FOR_DRAMC_PSEL: ON */
		clrbits32(&ch[chn].ao.refctrl1, (0x1 << 6) | (0x3 << 2));
		clrbits32(&ch[chn].ao.clkar, 0x1 << 19);

		/* PA_IMPROVEMENT_FOR_DRAMC_ACTIVE_POWER: ON */
		dramc_pa_improve(chn);

		/* DRAM DRS DISABLE */
		clrsetbits32(&ch[chn].ao.drsctrl,
			(0x1 << 0) | (0x1 << 2) | (0x1 << 4) | (0x1 << 5) | (0x1 << 6) |
			(0xf << 8) | (0x7f << 12) | (0x1 << 19) | (0x1 << 21),
			(0x1 << 0) | (0x0 << 2) | (0x0 << 4) | (0x1 << 5) | (0x0 << 6) |
			(0x8 << 8) | (0x3 << 12) | (0x1 << 19) | (0x0 << 21));
		setbits32(&ch[chn].ao.dummy_rd, 0x3 << 26);
	}
	dramc_dqs_precalculation_preset();

	enable_emi_dcm();
	dramc_enable_dramc_dcm();
}
