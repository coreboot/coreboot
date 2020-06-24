/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dramc_pi_api.h>
#include <soc/dramc_register.h>
#include <soc/pll.h>
#include <soc/pll_common.h>
#include <soc/regulator.h>

static void dramc_ac_timing_optimize(const struct ddr_cali *cali)
{
	u8 rf_group, cab_id;
	u8 trfc, trfc_05t, trfc_pb, trfrc_pb05t, tx_ref_cnt;

	enum {
		TRFCAB_130,
		TRFCAB_180,
		TRFCAB_280,
		TRFCAB_380,
		TRFCAB_NUM,
	};
	enum {
		GRP_DDR800_DIV4_ACTIM,
		GRP_DDR1200_ACTIM,
		GRP_DDR1600_ACTIM,
		GRP_DDR1866_ACTIM,
		GRP_DDR2400_ACTIM,
		GRP_DDR3200_ACTIM,
		GRP_DDR4266_ACTIM,
		GRP_ACTIM_NUM,
	};
	struct optimize_ac_time {
		u8 trfc;
		u8 trfc_05t;
		u8 trfc_pb;
		u8 trfrc_pb05t;
		u16 tx_ref_cnt;
	};

	const u8 density = cali->density;
	const dram_freq_grp freq_group = get_freq_group(cali);

	/* tRFCab */
	struct optimize_ac_time *ptr_trfcab_opt;
	struct optimize_ac_time trfcab_opt[GRP_ACTIM_NUM][TRFCAB_NUM] = {
		[GRP_DDR800_DIV4_ACTIM] = {
			{.trfc = 14, .trfc_05t = 0, .trfc_pb = 0,
				.trfrc_pb05t = 0, .tx_ref_cnt = 28},
			{.trfc = 24, .trfc_05t = 0, .trfc_pb = 6,
				.trfrc_pb05t = 0, .tx_ref_cnt = 38},
			{.trfc = 44, .trfc_05t = 0, .trfc_pb = 16,
				.trfrc_pb05t = 0, .tx_ref_cnt = 58},
			{.trfc = 64, .trfc_05t = 0, .trfc_pb = 26,
				.trfrc_pb05t = 0, .tx_ref_cnt = 78}
		},
		[GRP_DDR1200_ACTIM] = {
			{.trfc = 8, .trfc_05t = 0, .trfc_pb = 0,
				.trfrc_pb05t = 0, .tx_ref_cnt = 21},
			{.trfc = 15, .trfc_05t = 1, .trfc_pb = 2,
				.trfrc_pb05t = 0, .tx_ref_cnt = 29},
			{.trfc = 30, .trfc_05t = 1, .trfc_pb = 9,
				.trfrc_pb05t = 1, .tx_ref_cnt = 44},
			{.trfc = 45, .trfc_05t = 1, .trfc_pb = 17,
				.trfrc_pb05t = 0, .tx_ref_cnt = 59}
		},
		[GRP_DDR1600_ACTIM] = {
			{.trfc = 14, .trfc_05t = 0, .trfc_pb = 0,
				.trfrc_pb05t = 0, .tx_ref_cnt = 28},
			{.trfc = 24, .trfc_05t = 0, .trfc_pb = 6,
				.trfrc_pb05t = 0, .tx_ref_cnt = 38},
			{.trfc = 44, .trfc_05t = 0, .trfc_pb = 16,
				.trfrc_pb05t = 0, .tx_ref_cnt = 58},
			{.trfc = 64, .trfc_05t = 0, .trfc_pb = 26,
				.trfrc_pb05t = 0, .tx_ref_cnt = 78}
		},
		[GRP_DDR1866_ACTIM] = {
			{.trfc = 18, .trfc_05t = 1, .trfc_pb = 2,
				.trfrc_pb05t = 0, .tx_ref_cnt = 33},
			{.trfc = 30, .trfc_05t = 0, .trfc_pb = 9,
				.trfrc_pb05t = 0, .tx_ref_cnt = 44},
			{.trfc = 53, .trfc_05t = 1, .trfc_pb = 21,
				.trfrc_pb05t = 0, .tx_ref_cnt = 68},
			{.trfc = 77, .trfc_05t = 0, .trfc_pb = 32,
				.trfrc_pb05t = 1, .tx_ref_cnt = 91}
		},
		[GRP_DDR2400_ACTIM] = {
			{.trfc = 27, .trfc_05t = 1, .trfc_pb = 6,
				.trfrc_pb05t = 1, .tx_ref_cnt = 42},
			{.trfc = 42, .trfc_05t = 1, .trfc_pb = 15,
				.trfrc_pb05t = 1, .tx_ref_cnt = 57},
			{.trfc = 72, .trfc_05t = 1, .trfc_pb = 30,
				.trfrc_pb05t = 1, .tx_ref_cnt = 87},
			{.trfc = 102, .trfc_05t = 1, .trfc_pb = 45,
				.trfrc_pb05t = 1, .tx_ref_cnt = 117}
		},
		[GRP_DDR3200_ACTIM] = {
			{.trfc = 40, .trfc_05t = 0, .trfc_pb = 12,
				.trfrc_pb05t = 0, .tx_ref_cnt = 55},
			{.trfc = 60, .trfc_05t = 0, .trfc_pb = 24,
				.trfrc_pb05t = 0, .tx_ref_cnt = 75},
			{.trfc = 100, .trfc_05t = 0, .trfc_pb = 44,
				.trfrc_pb05t = 0, .tx_ref_cnt = 115},
			{.trfc = 140, .trfc_05t = 0, .trfc_pb = 64,
				.trfrc_pb05t = 0, .tx_ref_cnt = 155}
		},
		[GRP_DDR4266_ACTIM] = {
			{.trfc = 57, .trfc_05t = 1, .trfc_pb = 20,
				.trfrc_pb05t = 0, .tx_ref_cnt = 74},
			{.trfc = 84, .trfc_05t = 0, .trfc_pb = 36,
				.trfrc_pb05t = 0, .tx_ref_cnt = 100},
			{.trfc = 137, .trfc_05t = 1, .trfc_pb = 63,
				.trfrc_pb05t = 0, .tx_ref_cnt = 154},
			{.trfc = 191, .trfc_05t = 0, .trfc_pb = 89,
				.trfrc_pb05t = 1, .tx_ref_cnt = 207}
		}
	};

	switch (density) {
	case 0x0:
		rf_group = TRFCAB_130;
		break;
	case 0x1:
	case 0x2:
		rf_group = TRFCAB_180;
		break;
	case 0x3:
	case 0x4:
		rf_group = TRFCAB_280;
		break;
	case 0x5:
	case 0x6:
		rf_group = TRFCAB_380;
		break;
	default:
		die("Invalid DDR density %u\n", density);
		return;
	}

	switch (freq_group) {
	case DDRFREQ_400:
		cab_id = GRP_DDR800_DIV4_ACTIM;
		break;
	case DDRFREQ_600:
		cab_id = GRP_DDR1200_ACTIM;
		break;
	case DDRFREQ_800:
		cab_id = GRP_DDR1600_ACTIM;
		break;
	case DDRFREQ_933:
		cab_id = GRP_DDR1866_ACTIM;
		break;
	case DDRFREQ_1200:
		cab_id = GRP_DDR2400_ACTIM;
		break;
	case DDRFREQ_1600:
		cab_id = GRP_DDR3200_ACTIM;
		break;
	case DDRFREQ_2133:
		cab_id = GRP_DDR4266_ACTIM;
		break;
	default:
		die("Invalid DDR frequency group %u\n", freq_group);
		return;
	}

	ptr_trfcab_opt = &trfcab_opt[cab_id][0];
	trfc = ptr_trfcab_opt[rf_group].trfc;
	trfc_05t = ptr_trfcab_opt[rf_group].trfc_05t;
	trfc_pb = ptr_trfcab_opt[rf_group].trfc_pb;
	trfrc_pb05t = ptr_trfcab_opt[rf_group].trfrc_pb05t;
	tx_ref_cnt = ptr_trfcab_opt[rf_group].tx_ref_cnt;

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		SET32_BITFIELDS(&ch[chn].ao.shu_actim3,
				SHU_ACTIM3_TRFC, trfc);
		SET32_BITFIELDS(&ch[chn].ao.shu_ac_time_05t,
				SHU_AC_TIME_05T_TRFC_05T, trfc_05t);
		SET32_BITFIELDS(&ch[chn].ao.shu_actim4,
				SHU_ACTIM4_TXREFCNT, tx_ref_cnt);
		SET32_BITFIELDS(&ch[chn].ao.shu_actim3,
				SHU_ACTIM3_TRFCPB, trfc_pb);
		SET32_BITFIELDS(&ch[chn].ao.shu_ac_time_05t,
				SHU_AC_TIME_05T_TRFCPB_05T, trfrc_pb05t);
		dramc_dbg("Density (MR8 OP[5:2]) %u, TRFC %u, TRFC_05T %u, TXREFCNT %u, "
			  "TRFCpb %u, TRFCpb_05T %u\n",
			  density, trfc, trfc_05t, tx_ref_cnt, trfc_pb, trfrc_pb05t);
	}
}

static void set_vcore_voltage(const struct ddr_cali *cali)
{
	u32 vcore = get_vcore_value(cali);

	dramc_info("Set DRAM vcore voltage to %u\n", vcore);
	mainboard_set_regulator_vol(MTK_REGULATOR_VCORE, vcore);
}

static void get_dram_info_after_cal(struct ddr_cali *cali)
{
	u8 vendor_id, density, max_density = 0;
	u32 size_gb, max_size = 0;

	vendor_id = dramc_mode_reg_read_by_rank(CHANNEL_A, RANK_0, 5) & 0xff;
	dramc_info("Vendor id is %#x\n", vendor_id);

	for (u8 rk = RANK_0; rk < cali->support_ranks; rk++) {
		density = dramc_mode_reg_read_by_rank(CHANNEL_A, rk, 8) & 0xff;
		dramc_dbg("MR8 %#x\n", density);
		density = (density >> 2) & 0xf;

		switch (density) {
		/* these case values are from JESD209-4C MR8 Density OP[5:2] */
		case 0x0:
			size_gb = 4;
			break;
		case 0x1:
			size_gb = 6;
			break;
		case 0x2:
			size_gb = 8;
			break;
		case 0x3:
			size_gb = 12;
			break;
		case 0x4:
			size_gb = 16;
			break;
		case 0x5:
			size_gb = 24;
			break;
		case 0x6:
			size_gb = 32;
			break;
		case 0xC:
			size_gb = 2;
			break;
		default:
			dramc_err("Unexpected mode register density value: %#x\n", density);
			size_gb = 0;
			break;
		}
		if (size_gb > max_size) {
			max_size = size_gb;
			max_density = density;
		}
		dramc_dbg("RK%u size %uGb, density:%u\n", rk, size_gb, max_density);
	}

	cali->density = max_density;
}

static void dramc_calibration_all_channels(struct ddr_cali *cali)
{
}

static void mem_pll_init(void)
{
	SET32_BITFIELDS(&mtk_apmixed->mpll_con3, PLL_POWER_ISO_ENABLE, 3);

	udelay(30);
	SET32_BITFIELDS(&mtk_apmixed->mpll_con3, PLL_ISO_ENABLE, 0);

	udelay(1);
	SET32_BITFIELDS(&mtk_apmixed->mpll_con1, PLL_CON1, MPLL_CON1_FREQ);
	SET32_BITFIELDS(&mtk_apmixed->mpll_con0, PLL_ENABLE, 1);

	udelay(20);
	SET32_BITFIELDS(&mtk_apmixed->pllon_con0, MPLL_IOS_SEL, 0);
	SET32_BITFIELDS(&mtk_apmixed->pllon_con0, MPLL_EN_SEL, 0);
	SET32_BITFIELDS(&mtk_apmixed->pllon_con1, MPLL_PWR_SEL, 0);
	SET32_BITFIELDS(&mtk_apmixed->pllon_con2, MPLL_BY_ISO_DLY, 0);
	SET32_BITFIELDS(&mtk_apmixed->pllon_con3, MPLL_BY_PWR_DLY, 0);
}

void init_dram(const struct dramc_data *dparam)
{
	u32 bc_bak;
	u8 k_shuffle, k_shuffle_end;
	u8 pll_mode = 0;
	bool first_freq_k = true;

	struct ddr_cali cali = {0};
	struct mr_values mr_value;
	const struct ddr_base_info *ddr_info = &dparam->ddr_info;

	cali.pll_mode = &pll_mode;
	cali.mr_value = &mr_value;
	cali.support_ranks = ddr_info->support_ranks;
	cali.cbt_mode[RANK_0] = ddr_info->cbt_mode[RANK_0];
	cali.cbt_mode[RANK_1] = ddr_info->cbt_mode[RANK_1];
	cali.emi_config = &ddr_info->emi_config;

	dramc_set_broadcast(DRAMC_BROADCAST_ON);
	mem_pll_init();

	global_option_init(&cali);
	bc_bak = dramc_get_broadcast();
	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	emi_mdl_init(cali.emi_config);
	dramc_set_broadcast(bc_bak);

	dramc_sw_impedance_cal(ODT_OFF, &cali.impedance);
	dramc_sw_impedance_cal(ODT_ON, &cali.impedance);

	if (ddr_info->config_dvfs == DRAMC_ENABLE_DVFS)
		k_shuffle_end = CALI_SEQ_MAX;
	else
		k_shuffle_end = CALI_SEQ1;

	for (k_shuffle = CALI_SEQ0; k_shuffle < k_shuffle_end; k_shuffle++) {
		set_cali_datas(&cali, dparam, k_shuffle);
		set_vcore_voltage(&cali);
		dfs_init_for_calibration(&cali);

		if (first_freq_k)
			emi_init2();

		dramc_calibration_all_channels(&cali);

		/* only need to do once to get DDR's base information */
		if (first_freq_k)
			get_dram_info_after_cal(&cali);

		dramc_ac_timing_optimize(&cali);
		dramc_save_result_to_shuffle(DRAM_DFS_SHU0, cali.shu);

		/* for frequency switch in dramc_mode_reg_init phase */
		if (first_freq_k)
			dramc_load_shuffle_to_dramc(cali.shu, DRAM_DFS_SHU1);

		first_freq_k = false;
	}
}
