/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dramc_pi_api.h>
#include <soc/dramc_register.h>
#include <soc/gpio.h>

static const u8 mrr_o1_pinmux_mapping[PINMUX_MAX][CHANNEL_MAX][DQ_DATA_WIDTH] = {
	[PINMUX_DSC] = {
		[CHANNEL_A] = {0, 1, 7, 6, 4, 5, 2, 3, 9, 8, 11, 10, 14, 15, 13, 12},
		[CHANNEL_B] = {1, 0, 5, 6, 3, 2, 7, 4, 8, 9, 11, 10, 12, 14, 13, 15},
	},
	[PINMUX_LPBK] = {
	},
	[PINMUX_EMCP] = {
		[CHANNEL_A] = {1, 0, 3, 2, 4, 7, 6, 5, 8, 9, 10, 14, 11, 15, 13, 12},
		[CHANNEL_B] = {0, 1, 4, 7, 3, 5, 6, 2, 9, 8, 10, 12, 11, 14, 13, 15}
	},
};

static void set_rank_info_to_conf(const struct ddr_cali *cali)
{
	u8 value = ((cali->emi_config->cona_val >> 17) & 0x1) ? 0 : 1;

	SET32_BITFIELDS(&ch[0].ao.sa_reserve,
		SA_RESERVE_MODE_RK0, cali->cbt_mode[RANK_0],
		SA_RESERVE_MODE_RK1, cali->cbt_mode[RANK_1],
		SA_RESERVE_SINGLE_RANK, value);
}

static void get_dram_pinmux_sel(struct ddr_cali *cali)
{
	u32 value = (read32(&mtk_gpio->dram_pinmux_trapping) >> 19) & 0x1;

	if (value)
		cali->pinmux_type = PINMUX_DSC;
	else
		cali->pinmux_type = PINMUX_EMCP;
}

static void set_mrr_pinmux_mapping(const struct ddr_cali *cali)
{
	const u8 *map;
	u32 bc_bak = dramc_get_broadcast();

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		map = mrr_o1_pinmux_mapping[get_pinmux_type(cali)][chn];
		SET32_BITFIELDS(&ch[chn].ao.mrr_bit_mux1,
			MRR_BIT_MUX1_MRR_BIT0_SEL, map[0],
			MRR_BIT_MUX1_MRR_BIT1_SEL, map[1],
			MRR_BIT_MUX1_MRR_BIT2_SEL, map[2],
			MRR_BIT_MUX1_MRR_BIT3_SEL, map[3]);
		SET32_BITFIELDS(&ch[chn].ao.mrr_bit_mux2,
			MRR_BIT_MUX2_MRR_BIT4_SEL, map[4],
			MRR_BIT_MUX2_MRR_BIT5_SEL, map[5],
			MRR_BIT_MUX2_MRR_BIT6_SEL, map[6],
			MRR_BIT_MUX2_MRR_BIT7_SEL, map[7]);
		SET32_BITFIELDS(&ch[chn].ao.mrr_bit_mux3,
			MRR_BIT_MUX3_MRR_BIT8_SEL, map[8],
			MRR_BIT_MUX3_MRR_BIT9_SEL, map[9],
			MRR_BIT_MUX3_MRR_BIT10_SEL, map[10],
			MRR_BIT_MUX3_MRR_BIT11_SEL, map[11]);
		SET32_BITFIELDS(&ch[chn].ao.mrr_bit_mux4,
			MRR_BIT_MUX4_MRR_BIT12_SEL, map[12],
			MRR_BIT_MUX4_MRR_BIT13_SEL, map[13],
			MRR_BIT_MUX4_MRR_BIT14_SEL, map[14],
			MRR_BIT_MUX4_MRR_BIT15_SEL, map[15]);
	}
	dramc_set_broadcast(bc_bak);
}

static void set_dqo1_pinmux_mapping(const struct ddr_cali *cali)
{
	const u8 *map;
	u32 bc_bak = dramc_get_broadcast();

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		map = mrr_o1_pinmux_mapping[get_pinmux_type(cali)][chn];
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_dq_se_pinmux_ctrl0,
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ0, map[0],
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ1, map[1],
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ2, map[2],
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ3, map[3],
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ4, map[4],
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ5, map[5],
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ6, map[6],
			MISC_DQ_SE_PINMUX_CTRL0_DQ_PINMUX_SEL_DQ7, map[7]);
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_dq_se_pinmux_ctrl1,
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ8, map[8],
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ9, map[9],
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ10, map[10],
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ11, map[11],
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ12, map[12],
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ13, map[13],
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ14, map[14],
			MISC_DQ_SE_PINMUX_CTRL1_DQ_PINMUX_SEL_DQ15, map[15]);
	}

	dramc_set_broadcast(bc_bak);
}

void global_option_init(struct ddr_cali *cali)
{
	set_rank_info_to_conf(cali);
	get_dram_pinmux_sel(cali);
	set_mrr_pinmux_mapping(cali);
	set_dqo1_pinmux_mapping(cali);
}

static u32 get_write_latency_by_mr(u8 mr_wl)
{
	u32 write_latency;

	switch (mr_wl) {
	case 0:
		write_latency = 4;
		break;
	case 1:
		write_latency = 6;
		break;
	case 2:
		write_latency = 8;
		break;
	case 3:
		write_latency = 10;
		break;
	case 4:
		write_latency = 12;
		break;
	case 5:
		write_latency = 14;
		break;
	case 6:
		write_latency = 16;
		break;
	case 7:
		write_latency = 18;
		break;
	default:
		dramc_err("error: unexpected mr_wl: %x\n", mr_wl);
		return 0;
	}

	dramc_info("mr_wl: %x map to WriteLatency: %d\n", mr_wl, write_latency);
	return write_latency;
}

static void tx_path_algorithm(const struct ddr_cali *cali)
{
	u8 write_latency, wl_mr;
	const u8 ckr = 1;
	u8 dqs_total_ui;
	u8 dqs_oe_total_ui;
	u8 dqs_mck, dqs_ui;
	u8 dqs_oe_mck, dqs_oe_ui;
	u8 shift;
	const u8 tx_dq_oe_shift = 3;

	wl_mr = (cali->mr_value->mr02[get_fsp(cali)] & 0x3f) >> 3;
	shift = get_mck2ui_div_shift(cali);
	write_latency = get_write_latency_by_mr(wl_mr);
	dqs_total_ui = write_latency * ckr * 2 + 1;

	dqs_oe_total_ui = dqs_total_ui - tx_dq_oe_shift;
	dqs_ui = dqs_total_ui - ((dqs_total_ui >> shift) << shift);
	dqs_mck = dqs_total_ui >> shift;
	dqs_oe_ui = dqs_oe_total_ui - ((dqs_oe_total_ui >> shift) << shift);
	dqs_oe_mck = dqs_oe_total_ui >> shift;
	dramc_dbg("[TX_path_calculate] write_latency=%u, DQS_TotalUI=%u\n",
		write_latency, dqs_total_ui);
	dramc_dbg("[TX_path_calculate] DQS = (%u,%u) DQS_OE = (%u,%u)\n",
		dqs_mck, dqs_ui, dqs_oe_mck, dqs_oe_ui);

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		SET32_BITFIELDS(&ch[chn].ao.shu_selph_dqs0,
			SHU_SELPH_DQS0_TXDLY_DQS0, dqs_mck,
			SHU_SELPH_DQS0_TXDLY_DQS1, dqs_mck,
			SHU_SELPH_DQS0_TXDLY_OEN_DQS0, dqs_oe_mck,
			SHU_SELPH_DQS0_TXDLY_OEN_DQS1, dqs_oe_mck);
		SET32_BITFIELDS(&ch[chn].ao.shu_selph_dqs1,
			SHU_SELPH_DQS1_DLY_DQS0, dqs_ui,
			SHU_SELPH_DQS1_DLY_DQS1, dqs_ui,
			SHU_SELPH_DQS1_DLY_OEN_DQS0, dqs_oe_ui,
			SHU_SELPH_DQS1_DLY_OEN_DQS1, dqs_oe_ui);
	}
}

void cke_fix_onoff(const struct ddr_cali *cali, u8 chn, u8 rank, int option)
{
	u8 cke_on = 0, cke_off = 0;
	bool set_rank1 = (rank == RANK_MAX) && (cali->support_ranks == DUAL_RANK_DDR);

	if (option != CKE_DYNAMIC) {
		cke_on = option;
		cke_off = 1 - option;
	}

	if (rank == RANK_0 || rank == RANK_MAX) {
		SET32_BITFIELDS(&ch[chn].ao.ckectrl,
			CKECTRL_CKEFIXOFF, cke_off,
			CKECTRL_CKEFIXON, cke_on);
	}
	if (rank == RANK_1 || set_rank1) {
		SET32_BITFIELDS(&ch[chn].ao.ckectrl,
			CKECTRL_CKE1FIXOFF, cke_off,
			CKECTRL_CKE1FIXON, cke_on);
	}
}

static void set_cke2rank_independent(void)
{
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		SET32_BITFIELDS(&ch[chn].ao.rkcfg, RKCFG_CKE2RANK, 0);
		SET32_BITFIELDS(&ch[chn].ao.ckectrl,
			CKECTRL_CKE2RANK_OPT3, 0,
			CKECTRL_CKE2RANK_OPT, 0,
			CKECTRL_CKE2RANK_OPT2, 1,
			CKECTRL_CKE2RANK_OPT5, 0,
			CKECTRL_CKE2RANK_OPT6, 0,
			CKECTRL_CKE2RANK_OPT7, 1,
			CKECTRL_CKE2RANK_OPT8, 0,
			CKECTRL_CKETIMER_SEL, 0,
			CKECTRL_FASTWAKE_SEL, 1,
			CKECTRL_CKEWAKE_SEL, 0);
		SET32_BITFIELDS(&ch[chn].ao.shu_dcm_ctrl0,
			SHU_DCM_CTRL0_FASTWAKE, 1,
			SHU_DCM_CTRL0_FASTWAKE2, 1);
		SET32_BITFIELDS(&ch[chn].ao.actiming_ctrl, ACTIMING_CTRL_CLKWITRFC, 0);
	}
}

static void dramc_hw_gating_onoff(bool is_on)
{
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_dvfsctl2,
			MISC_DVFSCTL2_R_DVFS_OPTION, is_on,
			MISC_DVFSCTL2_R_DVFS_PARK_N, is_on);
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_stbcal2,
			MISC_STBCAL2_STB_GERRSTOP, is_on);
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_shu_stbcal,
			MISC_SHU_STBCAL_STBCALEN, is_on);
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_shu_stbcal,
			MISC_SHU_STBCAL_STB_SELPHCALEN, is_on);
	}
}

static void dramc_reset_delay_chain_before_calibration(void)
{
	u32 bc_bak = dramc_get_broadcast();
	dramc_set_broadcast(DRAMC_BROADCAST_OFF);

	for (u8 rk = RANK_0; rk < RANK_MAX; rk++) {
		for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
			struct ddrphy_ao_regs *phy_ao = &ch[chn].phy_ao;

			SET32_BITFIELDS(&phy_ao->ca_rk[rk].shu_r0_ca_txdly0,
				SHU_R0_CA_TXDLY0_TX_ARCA0_DLY, 0,
				SHU_R0_CA_TXDLY0_TX_ARCA1_DLY, 0,
				SHU_R0_CA_TXDLY0_TX_ARCA2_DLY, 0,
				SHU_R0_CA_TXDLY0_TX_ARCA3_DLY, 0);
			SET32_BITFIELDS(&phy_ao->ca_rk[rk].shu_r0_ca_txdly1,
				SHU_R0_CA_TXDLY1_TX_ARCA4_DLY, 0,
				SHU_R0_CA_TXDLY1_TX_ARCA5_DLY, 0,
				SHU_R0_CA_TXDLY1_TX_ARCA6_DLY, 0,
				SHU_R0_CA_TXDLY1_TX_ARCA7_DLY, 0);
			SET32_BITFIELDS(&phy_ao->byte[0].rk[rk].shu_r0_b0_txdly0,
				SHU_R0_B0_TXDLY0_TX_ARDQ0_DLY_B0, 0,
				SHU_R0_B0_TXDLY0_TX_ARDQ1_DLY_B0, 0,
				SHU_R0_B0_TXDLY0_TX_ARDQ2_DLY_B0, 0,
				SHU_R0_B0_TXDLY0_TX_ARDQ3_DLY_B0, 0);
			SET32_BITFIELDS(&phy_ao->byte[0].rk[rk].shu_r0_b0_txdly1,
				SHU_R0_B0_TXDLY1_TX_ARDQ4_DLY_B0, 0,
				SHU_R0_B0_TXDLY1_TX_ARDQ5_DLY_B0, 0,
				SHU_R0_B0_TXDLY1_TX_ARDQ6_DLY_B0, 0,
				SHU_R0_B0_TXDLY1_TX_ARDQ7_DLY_B0, 0);
			SET32_BITFIELDS(&phy_ao->byte[1].rk[rk].shu_r0_b0_txdly0,
				SHU_R0_B1_TXDLY0_TX_ARDQ0_DLY_B1, 0,
				SHU_R0_B1_TXDLY0_TX_ARDQ1_DLY_B1, 0,
				SHU_R0_B1_TXDLY0_TX_ARDQ2_DLY_B1, 0,
				SHU_R0_B1_TXDLY0_TX_ARDQ3_DLY_B1, 0);
			SET32_BITFIELDS(&phy_ao->byte[1].rk[rk].shu_r0_b0_txdly1,
				SHU_R0_B1_TXDLY1_TX_ARDQ4_DLY_B1, 0,
				SHU_R0_B1_TXDLY1_TX_ARDQ5_DLY_B1, 0,
				SHU_R0_B1_TXDLY1_TX_ARDQ6_DLY_B1, 0,
				SHU_R0_B1_TXDLY1_TX_ARDQ7_DLY_B1, 0);
			SET32_BITFIELDS(&phy_ao->byte[0].rk[rk].shu_r0_b0_txdly3,
				SHU_R0_B0_TXDLY3_TX_ARDQM0_DLY_B0, 0x0);
			SET32_BITFIELDS(&phy_ao->byte[1].rk[rk].shu_r0_b0_txdly3,
				SHU_R0_B1_TXDLY3_TX_ARDQM0_DLY_B1, 0x0);
		}
	}

	dramc_set_broadcast(bc_bak);
}

static void dramc_init(const struct ddr_cali *cali)
{
	dramc_reset_delay_chain_before_calibration();
}

static void dramc_before_calibration(const struct ddr_cali *cali)
{
	u8 ma_type = get_column_num();
	dram_freq_grp freq_group = cali->freq_group;
	u8 dis_imp_hw = (freq_group > DDRFREQ_1200) ? 0 : 1;

	dramc_hw_gating_onoff(false);

	cke_fix_onoff(cali, CHANNEL_A, RANK_MAX, CKE_FIXON);
	cke_fix_onoff(cali, CHANNEL_B, RANK_MAX, CKE_FIXON);

	set_cke2rank_independent();

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		SET32_BITFIELDS(&ch[chn].ao.shu_tx_set0, SHU_TX_SET0_DBIWR, 0x0);
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_shu_impedamce_upd_dis1,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_ODTN_UPD_DIS, dis_imp_hw,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_DRVN_UPD_DIS, dis_imp_hw,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_DRVP_UPD_DIS, dis_imp_hw,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_WCK_ODTN_UPD_DIS, dis_imp_hw,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_WCK_DRVN_UPD_DIS, dis_imp_hw,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_WCK_DRVP_UPD_DIS, dis_imp_hw,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_DQ_ODTN_UPD_DIS, dis_imp_hw,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_DQ_DRVN_UPD_DIS, dis_imp_hw,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_DQ_DRVP_UPD_DIS, dis_imp_hw,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_DQS_ODTN_UPD_DIS, dis_imp_hw,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_DQS_DRVN_UPD_DIS, dis_imp_hw,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_DQS_DRVP_UPD_DIS, dis_imp_hw,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_WCK_DRVP_UPD_DIS, 1,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_WCK_DRVN_UPD_DIS, 1,
			MISC_SHU_IMPEDAMCE_UPD_DIS1_WCK_ODTN_UPD_DIS, 1);

		SET32_BITFIELDS(&ch[chn].phy_ao.shu_misc_impcal1,
			SHU_MISC_IMPCAL1_IMPCALCNT, dis_imp_hw ? 0x0 : 0x40);
		SET32_BITFIELDS(&ch[chn].phy_ao.shu_misc_drving1,
			SHU_MISC_DRVING1_DIS_IMPCAL_HW, dis_imp_hw);
		SET32_BITFIELDS(&ch[chn].phy_ao.shu_misc_drving1,
			SHU_MISC_DRVING1_DIS_IMP_ODTN_TRACK, dis_imp_hw);
		SET32_BITFIELDS(&ch[chn].phy_ao.shu_misc_drving2,
			SHU_MISC_DRVING2_DIS_IMPCAL_ODT_EN, dis_imp_hw);
		SET32_BITFIELDS(&ch[chn].phy_ao.shu_ca_cmd12,
			SHU_CA_CMD12_RG_RIMP_UNTERM_EN, dis_imp_hw);
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_clk_ctrl,
			MISC_CLK_CTRL_DVFS_CLK_MEM_SEL, 0,
			MISC_CLK_CTRL_DVFS_MEM_CK_MUX_UPDATE_EN, 0);
		SET32_BITFIELDS(&ch[chn].ao.shu_zq_set0,
			SHU_ZQ_SET0_ZQCSCNT, 0x1ff,
			SHU_ZQ_SET0_TZQLAT, 0x1b);
		SET32_BITFIELDS(&ch[chn].ao.zq_set0,
			ZQ_SET0_ZQCSDUAL, 1,
			ZQ_SET0_ZQCSMASK_OPT, 0,
			ZQ_SET0_ZQMASK_CGAR, 0,
			ZQ_SET0_ZQCS_MASK_SEL_CGAR, 0);
	}
	SET32_BITFIELDS(&ch[0].ao.zq_set0, ZQ_SET0_ZQCSMASK, 1);
	SET32_BITFIELDS(&ch[1].ao.zq_set0, ZQ_SET0_ZQCSMASK, 0);

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		SET32_BITFIELDS(&ch[chn].ao.zq_set0, ZQ_SET0_ZQCS_MASK_SEL, 0);
		SET32_BITFIELDS(&ch[chn].phy_ao.byte[0].shu_b0_dq2,
			SHU_B0_DQ2_RG_ARPI_OFFSET_LAT_EN_B0, 0);
		SET32_BITFIELDS(&ch[chn].phy_ao.byte[1].shu_b0_dq2,
			SHU_B1_DQ2_RG_ARPI_OFFSET_LAT_EN_B1, 0);
		SET32_BITFIELDS(&ch[chn].ao.dcm_sub_ctrl,
			DCM_SUB_CTRL_SUBCLK_CTRL_TX_AUTOK, 0x0);
		SET32_BITFIELDS(&ch[chn].ao.dcm_sub_ctrl,
			DCM_SUB_CTRL_SUBCLK_CTRL_TX_TRACKING, 0);
		SET32_BITFIELDS(&ch[chn].ao.dqsoscr, DQSOSCR_DQSOSCRDIS, 0x1);
		SET32_BITFIELDS(&ch[chn].ao.refctrl0, REFCTRL0_REFDIS, 0x1);
		SET32_BITFIELDS(&ch[chn].ao.shu_matype, SHU_MATYPE_MATYPE, ma_type);
	}
	tx_path_algorithm(cali);
}

void dfs_init_for_calibration(const struct ddr_cali *cali)
{
	dramc_init(cali);
	dramc_before_calibration(cali);
}
