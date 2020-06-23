/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dramc_pi_api.h>
#include <soc/dramc_register.h>
#include <string.h>

static const u8 imp_vref_sel[ODT_MAX][IMP_DRV_MAX] = {
	/* DRVP  DRVN  ODTP  ODTN */
	[ODT_OFF] = {0x37, 0x33, 0x00, 0x37},
	[ODT_ON] = {0x3a, 0x33, 0x00, 0x3a},
};

static void dramc_imp_cal_vref_sel(dram_odt_state odt, imp_drv_type drv_type)
{
	u8 vref_tmp = imp_vref_sel[odt][drv_type];

	switch (drv_type) {
	case DRVP:
		SET32_BITFIELDS(&ch[0].phy_ao.shu_ca_cmd12,
			SHU_CA_CMD12_RG_RIMP_VREF_SEL_DRVP, vref_tmp);
		break;
	case DRVN:
		SET32_BITFIELDS(&ch[0].phy_ao.shu_ca_cmd12,
			SHU_CA_CMD12_RG_RIMP_VREF_SEL_DRVN, vref_tmp);
		break;
	case ODTN:
		SET32_BITFIELDS(&ch[0].phy_ao.shu_ca_cmd12,
			SHU_CA_CMD12_RG_RIMP_VREF_SEL_ODTN, vref_tmp);
		break;
	default:
		die("Can't support drv_type %d\n", drv_type);
		break;
	}
}

static u32 dramc_sw_imp_cal_result(imp_drv_type drv_type)
{
	u32 drive = 0, cal_res = 0;
	u32 change = (drv_type == DRVP) ? 1 : 0;

	static const char *const drv_type_str[IMP_DRV_MAX] = {
		[DRVP] = "DRVP",
		[DRVN] = "DRVN",
		[ODTP] = "ODTP",
		[ODTN] = "ODTN",
	};
	if (drv_type >= IMP_DRV_MAX)
		die("Can't support drv_type %d", drv_type);

	const char *drv_str = drv_type_str[drv_type];

	for (drive = 0; drive < 32; drive++) {
		if (drv_type == DRVP)
			SET32_BITFIELDS(&ch[0].phy_ao.shu_misc_impcal1,
				SHU_MISC_IMPCAL1_IMPDRVP, drive);
		else if (drv_type == DRVN || drv_type == ODTN)
			SET32_BITFIELDS(&ch[0].phy_ao.shu_misc_impcal1,
				SHU_MISC_IMPCAL1_IMPDRVN, drive);

		udelay(1);
		cal_res = READ32_BITFIELD(&ch[0].phy_nao.misc_phy_rgs_cmd,
				MISC_PHY_RGS_CMD_RGS_RIMPCALOUT);
		dramc_dbg("OCD %s=%d ,CALOUT=%d\n", drv_str, drive, cal_res);

		if (cal_res == change) {
			dramc_info("%s calibration passed! result=%d\n", drv_str, drive);
			break;
		}
	}

	if (drive == 32) {
		drive = 31;
		dramc_err("OCD %s calibration failed! %s=%d\n", drv_str, drv_str, drive);
	}

	return drive;
}

void dramc_sw_impedance_cal(dram_odt_state odt, struct dram_impedance *imp)
{
	const u8 chn = 0;
	u8 i_chn, enp, enn;
	u32 bc_bak, impcal_bak, cal_res;
	u32 drvp_result = 0xff, odtn_result = 0xff, drvn_result = 0xff;

	bc_bak = dramc_get_broadcast();
	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	for (i_chn = 0; i_chn < CHANNEL_MAX; i_chn++) {
		SET32_BITFIELDS(&ch[i_chn].phy_ao.misc_lp_ctrl,
			MISC_LP_CTRL_RG_ARDMSUS_10, 0x0,
			MISC_LP_CTRL_RG_ARDMSUS_10_LP_SEL, 0x0,
			MISC_LP_CTRL_RG_RIMP_DMSUS_10, 0x0,
			MISC_LP_CTRL_RG_RIMP_DMSUS_10_LP_SEL, 0x0);
		SET32_BITFIELDS(&ch[i_chn].phy_ao.misc_impcal, MISC_IMPCAL_IMPCAL_HW, 0);
	}

	impcal_bak = read32(&ch[chn].phy_ao.misc_impcal);
	SET32_BITFIELDS(&ch[chn].phy_ao.misc_imp_ctrl1, MISC_IMP_CTRL1_RG_RIMP_PRE_EN, 0);
	SET32_BITFIELDS(&ch[chn].phy_ao.misc_impcal,
		MISC_IMPCAL_IMPCAL_CALI_ENN, 0,
		MISC_IMPCAL_IMPCAL_IMPPDP, 1,
		MISC_IMPCAL_IMPCAL_IMPPDN, 1);
	SET32_BITFIELDS(&ch[chn].phy_ao.misc_imp_ctrl1,
		MISC_IMP_CTRL1_RG_IMP_EN, 1,
		MISC_IMP_CTRL1_RG_RIMP_DDR3_SEL, 0,
		MISC_IMP_CTRL1_RG_RIMP_VREF_EN, 1,
		MISC_IMP_CTRL1_RG_RIMP_DDR4_SEL, 1);
	udelay(1);
	SET32_BITFIELDS(&ch[chn].phy_ao.misc_impcal, MISC_IMPCAL_IMPCAL_CALI_EN, 1);
	SET32_BITFIELDS(&ch[chn].phy_ao.shu_misc_impcal1,
		SHU_MISC_IMPCAL1_IMPDRVN, 0,
		SHU_MISC_IMPCAL1_IMPDRVP, 0);

	for (imp_drv_type drv_type = DRVP; drv_type < IMP_DRV_MAX; drv_type++) {
		if (drv_type == ODTP)
			continue;
		dramc_imp_cal_vref_sel(odt, drv_type);

		switch (drv_type) {
		case DRVP:
			enp = 0x1;
			enn = 0x0;
			drvp_result = 0;
			break;
		case DRVN:
		case ODTN:
			enp = 0x0;
			enn = (drv_type == DRVN) ? 0x0 : 0x1;
			break;
		default:
			die("Can't support drv_type %d\n", drv_type);
			break;
		}

		SET32_BITFIELDS(&ch[chn].phy_ao.misc_impcal,
			MISC_IMPCAL_IMPCAL_CALI_ENP, enp);
		SET32_BITFIELDS(&ch[chn].phy_ao.misc_impcal,
			MISC_IMPCAL_IMPCAL_CALI_ENN, enn);
		SET32_BITFIELDS(&ch[chn].phy_ao.shu_misc_impcal1,
			SHU_MISC_IMPCAL1_IMPDRVP, drvp_result);
		SET32_BITFIELDS(&ch[chn].phy_ao.shu_ca_cmd12,
			SHU_CA_CMD12_RG_RIMP_DRV05, 0);

		cal_res = dramc_sw_imp_cal_result(drv_type);
		switch (drv_type) {
		case DRVP:
			drvp_result = cal_res;
			break;
		case DRVN:
			drvn_result = cal_res;
			break;
		case ODTN:
			odtn_result = cal_res;
			break;
		default:
			die("Can't support drv_type %d\n", drv_type);
			break;
		}
	}

	imp->result[odt][DRVP] = drvp_result;
	imp->result[odt][DRVN] = drvn_result;
	imp->result[odt][ODTP] = 0;
	imp->result[odt][ODTN] = odtn_result;

	dramc_info("freq_region=%d, Reg: DRVP=%d, DRVN=%d, ODTN=%d\n",
		odt, drvp_result, drvn_result, odtn_result);

	write32(&ch[chn].phy_ao.misc_impcal, impcal_bak);
	dramc_set_broadcast(bc_bak);
}

void dramc_sw_impedance_save_register(const struct ddr_cali *cali)
{
	u8 ca_term, dq_term;
	u32 bc_bak = dramc_get_broadcast();
	const u32 (*result)[IMP_DRV_MAX] = cali->impedance.result;
	struct ddrphy_ao_regs *phy_ao = &ch[0].phy_ao;

	ca_term = get_odt_state(cali);
	dq_term = (get_freq_group(cali) < DDRFREQ_2133) ? ODT_OFF : ODT_ON;

	dramc_set_broadcast(DRAMC_BROADCAST_ON);
	SET32_BITFIELDS(&phy_ao->shu_misc_drving1,
		SHU_MISC_DRVING1_DQDRVP2, result[dq_term][DRVP],
		SHU_MISC_DRVING1_DQDRVN2, result[dq_term][DRVN]);
	SET32_BITFIELDS(&phy_ao->shu_misc_drving2,
		SHU_MISC_DRVING2_DQDRVP1, result[dq_term][DRVP],
		SHU_MISC_DRVING2_DQDRVN1, result[dq_term][DRVN]);
	SET32_BITFIELDS(&phy_ao->shu_misc_drving3,
		SHU_MISC_DRVING3_DQODTP2, result[dq_term][ODTP],
		SHU_MISC_DRVING3_DQODTN2, result[dq_term][ODTN]);
	SET32_BITFIELDS(&phy_ao->shu_misc_drving4,
		SHU_MISC_DRVING4_DQODTP1, result[dq_term][ODTP],
		SHU_MISC_DRVING4_DQODTN1, result[dq_term][ODTN]);

	SET32_BITFIELDS(&phy_ao->shu_misc_drving1,
		SHU_MISC_DRVING1_DQSDRVP2, result[dq_term][DRVP],
		SHU_MISC_DRVING1_DQSDRVN2, result[dq_term][DRVN]);
	SET32_BITFIELDS(&phy_ao->shu_misc_drving1,
		SHU_MISC_DRVING1_DQSDRVP1, result[dq_term][DRVP],
		SHU_MISC_DRVING1_DQSDRVN1, result[dq_term][DRVN]);
	SET32_BITFIELDS(&phy_ao->shu_misc_drving3,
		SHU_MISC_DRVING3_DQSODTP2, result[dq_term][ODTP],
		SHU_MISC_DRVING3_DQSODTN2, result[dq_term][ODTN]);
	SET32_BITFIELDS(&phy_ao->shu_misc_drving3,
		SHU_MISC_DRVING3_DQSODTP, result[dq_term][ODTP],
		SHU_MISC_DRVING3_DQSODTN, result[dq_term][ODTN]);

	SET32_BITFIELDS(&phy_ao->shu_misc_drving2,
		SHU_MISC_DRVING2_CMDDRVP2, result[ca_term][DRVP],
		SHU_MISC_DRVING2_CMDDRVN2, result[ca_term][DRVN]);
	SET32_BITFIELDS(&phy_ao->shu_misc_drving2,
		SHU_MISC_DRVING2_CMDDRVP1, result[ca_term][DRVP],
		SHU_MISC_DRVING2_CMDDRVN1, result[ca_term][DRVN]);
	SET32_BITFIELDS(&phy_ao->shu_misc_drving4,
		SHU_MISC_DRVING4_CMDODTP2, result[ca_term][ODTP],
		SHU_MISC_DRVING4_CMDODTN2, result[ca_term][ODTN]);
	SET32_BITFIELDS(&phy_ao->shu_misc_drving4,
		SHU_MISC_DRVING4_CMDODTP1, result[ca_term][ODTP],
		SHU_MISC_DRVING4_CMDODTN1, result[ca_term][ODTN]);

	SET32_BITFIELDS(&phy_ao->misc_shu_drving8, MISC_SHU_DRVING8_CS_DRVP, 0xF);
	SET32_BITFIELDS(&phy_ao->misc_shu_drving8, MISC_SHU_DRVING8_CS_DRVN, 0x14);

	dramc_set_broadcast(bc_bak);
}

static void dramc_phy_reset(u8 chn)
{
	SET32_BITFIELDS(&ch[chn].ao.rx_set0, RX_SET0_RDATRST, 1);
	SET32_BITFIELDS(&ch[chn].phy_ao.misc_ctrl1, MISC_CTRL1_R_DMPHYRST, 1);
	SET32_BITFIELDS(&ch[chn].phy_ao.dvs_b[0].b0_dq9,
		B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0, 0,
		B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0, 0);
	SET32_BITFIELDS(&ch[chn].phy_ao.dvs_b[1].b0_dq9,
		B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1, 0,
		B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1, 0);
	udelay(1);
	SET32_BITFIELDS(&ch[chn].phy_ao.dvs_b[1].b0_dq9,
		B1_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B1, 1,
		B1_DQ9_RG_RX_ARDQ_STBEN_RESETB_B1, 1);
	SET32_BITFIELDS(&ch[chn].phy_ao.dvs_b[0].b0_dq9,
		B0_DQ9_RG_RX_ARDQS0_STBEN_RESETB_B0, 1,
		B0_DQ9_RG_RX_ARDQ_STBEN_RESETB_B0, 1);
	SET32_BITFIELDS(&ch[chn].phy_ao.misc_ctrl1, MISC_CTRL1_R_DMPHYRST, 0);
	SET32_BITFIELDS(&ch[chn].ao.rx_set0, RX_SET0_RDATRST, 0);
}

static int dramc_8_phase_cal_find_best_dly(u8 phase_sm, u8 ph_dly, u8 ph_dly_back,
					   int *ph_dly_final, int *ph_dly_loop_break)
{
	const u8 early_break_cnt = 5;
	static u8 loop_cnt = 0;
	static u16 r0 = 0xffff, r180 = 0xffff, r_tmp = 0xffff, p_tmp = 0xffff;
	s16 err_code;
	static s16 err_code_min = 0x7fff;
	u16 dqs_dly;
	const u16 jm_dly_start = 0, jm_dly_end = 512, jm_dly_step = 1;
	u32 sample_cnt, ones_cnt;
	u8 dqs_level = DQS_LEVEL_UNKNOWN;

	dramc_dbg("8PH dly = %u\n", ph_dly);
	*ph_dly_loop_break = 0;

	SET32_BITFIELDS(&ch[0].phy_ao.byte[0].shu_b0_dq1,
		SHU_B0_DQ1_RG_ARPI_MIDPI_8PH_DLY_B0, ph_dly);
	SET32_BITFIELDS(&ch[0].phy_ao.byte[1].shu_b0_dq1,
		SHU_B1_DQ1_RG_ARPI_MIDPI_8PH_DLY_B1, ph_dly);
	SET32_BITFIELDS(&ch[0].phy_ao.shu_ca_cmd1,
		SHU_CA_CMD1_RG_ARPI_MIDPI_8PH_DLY_CA, ph_dly);

	for (dqs_dly = jm_dly_start; dqs_dly < jm_dly_end; dqs_dly += jm_dly_step) {
		/* Set DQS delay */
		SET32_BITFIELDS(&ch[0].phy_ao.dvs_b[0].b0_phy2,
			B0_PHY2_RG_RX_ARDQS_JM_DLY_B0, dqs_dly);
		SET32_BITFIELDS(&ch[0].phy_ao.dvs_b[1].b0_phy2,
			B1_PHY2_RG_RX_ARDQS_JM_DLY_B1, dqs_dly);
		dramc_phy_reset(0);
		SET32_BITFIELDS(&ch[0].phy_ao.misc_dutyscan1,
			MISC_DUTYSCAN1_REG_SW_RST, 1);
		SET32_BITFIELDS(&ch[0].phy_ao.misc_dutyscan1,
			MISC_DUTYSCAN1_REG_SW_RST, 0);
		SET32_BITFIELDS(&ch[0].phy_ao.misc_dutyscan1,
			MISC_DUTYSCAN1_RX_EYE_SCAN_EN, 1);
		udelay(10);
		SET32_BITFIELDS(&ch[0].phy_ao.misc_dutyscan1,
			MISC_DUTYSCAN1_RX_EYE_SCAN_EN, 0);
		sample_cnt = READ32_BITFIELD(&ch[0].phy_nao.misc_duty_toggle_cnt,
					     MISC_DUTY_TOGGLE_CNT_TOGGLE_CNT);
		ones_cnt = READ32_BITFIELD(&ch[0].phy_nao.misc_duty_dqs0_err_cnt,
					   MISC_DUTY_DQS0_ERR_CNT_DQS0_ERR_CNT);

		if (ones_cnt < sample_cnt / 2) {
			if (dqs_level == DQS_LEVEL_UNKNOWN)
				dramc_dbg("[L] %u, %8u\n", dqs_dly, ones_cnt);
			dqs_level = 0;
		} else if (dqs_level == 0) {
			dqs_level = 1;
			dramc_dbg("[H] %u, %8u\n", dqs_dly, ones_cnt);

			if (phase_sm == DQS_8PH_DEGREE_0) {
				r0 = dqs_dly;
				dramc_dbg("R0 = %u\n", r0);
			} else if (phase_sm == DQS_8PH_DEGREE_180) {
				r180 = dqs_dly;
				if (r180 <= r0) {
					dqs_level = DQS_LEVEL_UNKNOWN;
					continue;
				}

				r_tmp = r0 + ((r180 - r0) >> 2);
				dramc_dbg("R = %u, R180 = %u\n", r_tmp, r180);
			} else if (phase_sm == DQS_8PH_DEGREE_45) {
				p_tmp = dqs_dly;
				dramc_dbg("p_tmp = %u, R0 = %u\n", p_tmp, r0);
				if (p_tmp <= r0) {
					dqs_level = DQS_LEVEL_UNKNOWN;
					continue;
				}

				err_code = ABS(p_tmp - r_tmp);

				if (err_code == 0) {
					*ph_dly_final = ph_dly;
					*ph_dly_loop_break = 1;
				} else if (err_code < err_code_min) {
					err_code_min = err_code;
					*ph_dly_final = ph_dly;
					loop_cnt = 0;
				} else {
					loop_cnt++;
					if (loop_cnt > early_break_cnt)
						*ph_dly_loop_break = 1;
				}

				dramc_dbg("diff (P-R) = %d, min = %d, break count = %u\n",
					err_code, err_code_min, loop_cnt);
			} else {
				die("Invalid phase_sm: %u!\n", phase_sm);
			}

			break;
		}
	}

	if (dqs_level == DQS_LEVEL_UNKNOWN || dqs_level == 0) {
		*ph_dly_final = ph_dly_back;
		dramc_err("8-Phase SM_%u failed, falling back to default\n", phase_sm);
		return 0;
	} else if (*ph_dly_loop_break) {
		return -1;
	}

	return -2;
}

static void dramc_8_phase_cal_set_best_dly(const struct ddr_cali *cali, u8 ph_dly_back)
{
	u8 dqsien_pi;
	u8 phase_sm, ph_dly, ph_start, ph_end;
	int ph_dly_loop_break;
	int ph_dly_final = 0xff;	/* default delay: 0xff */

	for (phase_sm = DQS_8PH_DEGREE_0; phase_sm < DQS_8PH_DEGREE_MAX; phase_sm++) {
		switch (phase_sm) {
		case DQS_8PH_DEGREE_0:
			dqsien_pi = 16;
			ph_start = 0;
			ph_end = 1;
			break;
		case DQS_8PH_DEGREE_180:
			dqsien_pi = 48;
			ph_start = 0;
			ph_end = 1;
			break;
		case DQS_8PH_DEGREE_45:
			dqsien_pi = 24;
			ph_start = 0;
			ph_end = 32;
			break;
		default:
			die("Invalid phase_sm: %u!\n", phase_sm);
			return;
		}

		dramc_dbg("8-Phase SM_%u, 8PH_dly (%u~%u), DQSIEN PI = %u, 8PH_Dly = %u\n",
			phase_sm, ph_start, ph_end, dqsien_pi, ph_dly_back);

		for (u8 rk = RANK_0; rk < cali->support_ranks; rk++)
			SET32_BITFIELDS(&ch[0].phy_ao.byte[0].rk[rk].shu_rk_b0_dqsien_pi_dly,
				SHU_RK_B0_DQSIEN_PI_DLY_DQSIEN_PI_B0, dqsien_pi);

		for (ph_dly = ph_start; ph_dly < ph_end; ph_dly++) {
			int ret = dramc_8_phase_cal_find_best_dly(phase_sm, ph_dly,
					ph_dly_back, &ph_dly_final, &ph_dly_loop_break);
			if (ret == 0)
				goto final_found;
			if (ph_dly_loop_break)
				break;
		}
	}

final_found:
	dramc_dbg("8 phase calibration ph_dly_final = %u\n", ph_dly_final);
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		SET32_BITFIELDS(&ch[chn].phy_ao.byte[0].shu_b0_dq1,
			SHU_B0_DQ1_RG_ARPI_MIDPI_8PH_DLY_B0, ph_dly_final);
		SET32_BITFIELDS(&ch[chn].phy_ao.byte[1].shu_b0_dq1,
			SHU_B1_DQ1_RG_ARPI_MIDPI_8PH_DLY_B1, ph_dly_final);
		SET32_BITFIELDS(&ch[chn].phy_ao.shu_ca_cmd1,
			SHU_CA_CMD1_RG_ARPI_MIDPI_8PH_DLY_CA, ph_dly_final);
	}
}

void dramc_8_phase_cal(const struct ddr_cali *cali)
{
	u8 ph_dly_back = 0;
	dram_freq_grp freq_group = cali->freq_group;
	/* ch0 and ch1 of phy_ao */
	struct ddrphy_ao_regs *phy_ao = &ch[0].phy_ao;
	struct ddrphy_ao_regs *phy_ao_ch1 = &ch[1].phy_ao;

	if (freq_group < DDRFREQ_2133) {
		dramc_info("Freq %u < 2133, do not need do 8-Phase\n", get_frequency(cali));
		return;
	}

	struct reg_bak regs_bak[] = {
		{&phy_ao->misc_dutyscan1},
		{&phy_ao->dvs_b[0].b0_dq6},
		{&phy_ao->dvs_b[1].b0_dq6},
		{&phy_ao->dvs_b[0].b0_dq5},
		{&phy_ao->dvs_b[1].b0_dq5},
		{&phy_ao->dvs_b[0].b0_dq3},
		{&phy_ao->dvs_b[1].b0_dq3},
		{&phy_ao->misc_ctrl1},
		{&phy_ao->misc_ctrl4},
		{&phy_ao->dvs_b[0].b0_phy2},
		{&phy_ao->dvs_b[1].b0_phy2},
		{&phy_ao->byte[0].shu_b0_dll_arpi2},
		{&phy_ao->byte[1].shu_b0_dll_arpi2},
		{&phy_ao->byte[0].shu_b0_dq11},
		{&phy_ao->byte[1].shu_b0_dq11},
		{&phy_ao->shu_ca_cmd11},
		{&phy_ao->misc_stbcal},
		{&phy_ao->byte[0].rk[0].shu_rk_b0_dqsien_pi_dly},
		{&phy_ao->byte[0].rk[1].shu_rk_b0_dqsien_pi_dly},
		{&phy_ao->misc_jmeter},
		{&phy_ao->misc_shu_stbcal},
		{&phy_ao->shu_ca_dll1},
		{&phy_ao->byte[0].shu_b0_dll1},
		{&phy_ao->byte[1].shu_b0_dll1},
		{&phy_ao->dvs_b[0].b0_dq2},
		{&phy_ao->dvs_b[1].b0_dq2},
		{&phy_ao->ca_cmd2},
		{&phy_ao->byte[0].shu_b0_dq13},
		{&phy_ao->byte[1].shu_b0_dq13},
		{&phy_ao->shu_ca_cmd13},
		{&phy_ao_ch1->shu_ca_dll1},
		{&phy_ao_ch1->byte[0].shu_b0_dll1},
		{&phy_ao_ch1->byte[1].shu_b0_dll1},
		{&phy_ao_ch1->dvs_b[0].b0_dq2},
		{&phy_ao_ch1->dvs_b[1].b0_dq2},
		{&phy_ao_ch1->ca_cmd2},
		{&phy_ao_ch1->byte[0].shu_b0_dq13},
		{&phy_ao_ch1->byte[1].shu_b0_dq13},
		{&phy_ao_ch1->shu_ca_cmd13},
	};
	for (size_t i = 0; i < ARRAY_SIZE(regs_bak); i++)
		regs_bak[i].value = read32(regs_bak[i].addr);

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		phy_ao = &ch[chn].phy_ao;
		SET32_BITFIELDS(&phy_ao->dvs_b[0].b0_dq2,
			B0_DQ2_RG_TX_ARDQS_OE_TIE_SEL_B0, 0,
			B0_DQ2_RG_TX_ARDQS_OE_TIE_EN_B0, 1,
			B0_DQ2_RG_TX_ARWCK_OE_TIE_SEL_B0, 0,
			B0_DQ2_RG_TX_ARWCK_OE_TIE_EN_B0, 1,
			B0_DQ2_RG_TX_ARWCKB_OE_TIE_SEL_B0, 0,
			B0_DQ2_RG_TX_ARWCKB_OE_TIE_EN_B0, 1,
			B0_DQ2_RG_TX_ARDQM_OE_TIE_SEL_B0, 0,
			B0_DQ2_RG_TX_ARDQM_OE_TIE_EN_B0, 1,
			B0_DQ2_RG_TX_ARDQ_OE_TIE_SEL_B0, 0,
			B0_DQ2_RG_TX_ARDQ_OE_TIE_EN_B0, 0xff);
		SET32_BITFIELDS(&phy_ao->dvs_b[1].b0_dq2,
			B1_DQ2_RG_TX_ARDQS_OE_TIE_SEL_B1, 0,
			B1_DQ2_RG_TX_ARDQS_OE_TIE_EN_B1, 1,
			B1_DQ2_RG_TX_ARWCK_OE_TIE_SEL_B1, 0,
			B1_DQ2_RG_TX_ARWCK_OE_TIE_EN_B1, 1,
			B1_DQ2_RG_TX_ARWCKB_OE_TIE_SEL_B1, 0,
			B1_DQ2_RG_TX_ARWCKB_OE_TIE_EN_B1, 1,
			B1_DQ2_RG_TX_ARDQM_OE_TIE_SEL_B1, 0,
			B1_DQ2_RG_TX_ARDQM_OE_TIE_EN_B1, 1,
			B1_DQ2_RG_TX_ARDQ_OE_TIE_SEL_B1, 0,
			B1_DQ2_RG_TX_ARDQ_OE_TIE_EN_B1, 0xff);
		SET32_BITFIELDS(&phy_ao->ca_cmd2,
			CA_CMD2_RG_TX_ARCLK_OE_TIE_SEL_CA, 0,
			CA_CMD2_RG_TX_ARCLK_OE_TIE_EN_CA, 1,
			CA_CMD2_RG_TX_ARCS_OE_TIE_SEL_CA, 0,
			CA_CMD2_RG_TX_ARCS_OE_TIE_EN_CA, 1,
			CA_CMD2_RG_TX_ARCA_OE_TIE_SEL_CA, 0,
			CA_CMD2_RG_TX_ARCA_OE_TIE_EN_CA, 0xff);
		SET32_BITFIELDS(&phy_ao->byte[0].shu_b0_dq13,
			SHU_B0_DQ13_RG_TX_ARDQSB_OE_TIE_SEL_B0, 0,
			SHU_B0_DQ13_RG_TX_ARDQSB_OE_TIE_EN_B0, 1);
		SET32_BITFIELDS(&phy_ao->byte[1].shu_b0_dq13,
			SHU_B1_DQ13_RG_TX_ARDQSB_OE_TIE_SEL_B1, 0,
			SHU_B1_DQ13_RG_TX_ARDQSB_OE_TIE_EN_B1, 1);
		SET32_BITFIELDS(&phy_ao->shu_ca_cmd13,
			SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_SEL_CA, 0,
			SHU_CA_CMD13_RG_TX_ARCLKB_OE_TIE_EN_CA, 1);
	}

	phy_ao = &ch[0].phy_ao;
	ph_dly_back = READ32_BITFIELD(&phy_ao->byte[0].shu_b0_dq1,
		SHU_B0_DQ1_RG_ARPI_MIDPI_8PH_DLY_B0);
	SET32_BITFIELDS(&phy_ao->misc_shu_stbcal,
		MISC_SHU_STBCAL_STBCALEN, 0x0,
		MISC_SHU_STBCAL_STB_SELPHCALEN, 0x0);
	SET32_BITFIELDS(&phy_ao->byte[0].shu_b0_dq11,
		SHU_B0_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B0, 0);
	SET32_BITFIELDS(&phy_ao->byte[1].shu_b0_dq11,
		SHU_B1_DQ11_RG_RX_ARDQ_RANK_SEL_SER_EN_B1, 0);
	SET32_BITFIELDS(&phy_ao->shu_ca_cmd11,
		SHU_CA_CMD11_RG_RX_ARCA_RANK_SEL_SER_EN_CA, 0);

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		phy_ao = &ch[chn].phy_ao;
		SET32_BITFIELDS(&phy_ao->shu_ca_dll1,
			SHU_CA_DLL1_RG_ARDLL_PHDET_EN_CA, 0x0,
			SHU_CA_DLL1_RG_ARDLL_PHDET_OUT_SEL_CA, 0x0);
		SET32_BITFIELDS(&phy_ao->byte[0].shu_b0_dll1,
			SHU_B0_DLL1_RG_ARDLL_PHDET_EN_B0, 0x0,
			SHU_B0_DLL1_RG_ARDLL_PHDET_OUT_SEL_B0, 0x0);
		SET32_BITFIELDS(&phy_ao->byte[1].shu_b0_dll1,
			SHU_B1_DLL1_RG_ARDLL_PHDET_EN_B1, 0x0,
			SHU_B1_DLL1_RG_ARDLL_PHDET_OUT_SEL_B1, 0x0);
	}

	phy_ao = &ch[0].phy_ao;
	SET32_BITFIELDS(&phy_ao->misc_ctrl1, MISC_CTRL1_R_DMDQSIENCG_EN, 0);
	SET32_BITFIELDS(&phy_ao->byte[0].shu_b0_dll_arpi2,
		SHU_B0_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B0, 0);
	SET32_BITFIELDS(&phy_ao->byte[1].shu_b0_dll_arpi2,
		SHU_B1_DLL_ARPI2_RG_ARPI_CG_DQSIEN_B1, 0);
	SET32_BITFIELDS(&phy_ao->misc_dutyscan1,
		MISC_DUTYSCAN1_RX_EYE_SCAN_CG_EN, 1);
	SET32_BITFIELDS(&phy_ao->misc_ctrl4, MISC_CTRL4_R_OPT2_CG_DQSIEN, 0);
	SET32_BITFIELDS(&phy_ao->misc_stbcal, MISC_STBCAL_DQSIENCG_NORMAL_EN, 0);
	SET32_BITFIELDS(&phy_ao->dvs_b[0].b0_dq6,
		B0_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B0, 1);
	SET32_BITFIELDS(&phy_ao->dvs_b[1].b0_dq6,
		B1_DQ6_RG_RX_ARDQ_EYE_DLY_DQS_BYPASS_B1, 1);
	SET32_BITFIELDS(&phy_ao->misc_dutyscan1, MISC_DUTYSCAN1_RX_EYE_SCAN_EN, 1);
	SET32_BITFIELDS(&phy_ao->misc_dutyscan1,
		MISC_DUTYSCAN1_EYESCAN_DQS_SYNC_EN, 0x1,
		MISC_DUTYSCAN1_EYESCAN_NEW_DQ_SYNC_EN, 0x1,
		MISC_DUTYSCAN1_EYESCAN_DQ_SYNC_EN, 0x1);
	SET32_BITFIELDS(&phy_ao->dvs_b[0].b0_dq5, B0_DQ5_RG_RX_ARDQ_EYE_EN_B0, 1);
	SET32_BITFIELDS(&phy_ao->dvs_b[1].b0_dq5, B1_DQ5_RG_RX_ARDQ_EYE_EN_B1, 1);
	SET32_BITFIELDS(&phy_ao->dvs_b[0].b0_dq5, B0_DQ5_RG_RX_ARDQ_VREF_EN_B0, 1);
	SET32_BITFIELDS(&phy_ao->dvs_b[1].b0_dq5, B1_DQ5_RG_RX_ARDQ_VREF_EN_B1, 1);
	SET32_BITFIELDS(&phy_ao->dvs_b[0].b0_dq3, B0_DQ3_RG_RX_ARDQ_SMT_EN_B0, 1);
	SET32_BITFIELDS(&phy_ao->dvs_b[1].b0_dq3, B1_DQ3_RG_RX_ARDQ_SMT_EN_B1, 1);
	SET32_BITFIELDS(&phy_ao->dvs_b[0].b0_phy2, B0_PHY2_RG_RX_ARDQS_JM_EN_B0, 1);
	SET32_BITFIELDS(&phy_ao->dvs_b[1].b0_phy2, B1_PHY2_RG_RX_ARDQS_JM_EN_B1, 1);
	SET32_BITFIELDS(&phy_ao->misc_jmeter, MISC_JMETER_JMTR_EN, 1);
	SET32_BITFIELDS(&phy_ao->dvs_b[0].b0_phy2, B0_PHY2_RG_RX_ARDQS_JM_SEL_B0, 1);
	SET32_BITFIELDS(&phy_ao->dvs_b[1].b0_phy2, B1_PHY2_RG_RX_ARDQS_JM_SEL_B1, 1);
	SET32_BITFIELDS(&phy_ao->misc_dutyscan1, MISC_DUTYSCAN1_RX_MIOCK_JIT_EN, 1);
	SET32_BITFIELDS(&phy_ao->misc_dutyscan1, MISC_DUTYSCAN1_RX_EYE_SCAN_EN, 0);
	SET32_BITFIELDS(&phy_ao->misc_dutyscan1, MISC_DUTYSCAN1_DQSERRCNT_DIS, 0);

	dramc_8_phase_cal_set_best_dly(cali, ph_dly_back);

	for (size_t i = 0; i < ARRAY_SIZE(regs_bak); i++)
		write32(regs_bak[i].addr, regs_bak[i].value);
}
