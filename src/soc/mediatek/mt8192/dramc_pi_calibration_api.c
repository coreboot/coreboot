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
