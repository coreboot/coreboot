/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/dramc_param.h>
#include <soc/dramc_pi_api.h>
#include <soc/dramc_register.h>
#include <soc/emi.h>
#include <soc/infracfg.h>
#include <soc/mt6358.h>
#include <soc/spm.h>

static const u8 freq_shuffle[DRAM_DFS_SHUFFLE_MAX] = {
	[DRAM_DFS_SHUFFLE_1] = LP4X_DDR3200,
	[DRAM_DFS_SHUFFLE_2] = LP4X_DDR2400,
	[DRAM_DFS_SHUFFLE_3] = LP4X_DDR1600,
};

static const u8 freq_shuffle_emcp[DRAM_DFS_SHUFFLE_MAX] = {
	[DRAM_DFS_SHUFFLE_1] = LP4X_DDR3600,
	[DRAM_DFS_SHUFFLE_2] = LP4X_DDR3200,
	[DRAM_DFS_SHUFFLE_3] = LP4X_DDR1600,
};

static const u32 frequency_table[LP4X_DDRFREQ_MAX] = {
	[LP4X_DDR1600] = 1600,
	[LP4X_DDR2400] = 2400,
	[LP4X_DDR3200] = 3200,
	[LP4X_DDR3600] = 3600,
};

static const u32 vcore_lp4x[LP4X_DDRFREQ_MAX] = {
	[LP4X_DDR1600] = 725000,
	[LP4X_DDR2400] = 725000,
	[LP4X_DDR3200] = 762500,
	[LP4X_DDR3600] = 800000,
};

struct emi_regs *emi_regs = (void *)EMI_BASE;
const u8 phy_mapping[CHANNEL_MAX][16] = {
	[CHANNEL_A] = {
		1, 0, 2, 4, 3, 7, 5, 6,
		9, 8, 12, 11, 10, 15, 13, 14
	},

	[CHANNEL_B] = {
		0, 1, 5, 6, 3, 7, 4, 2,
		9, 8, 12, 15, 11, 14, 13, 10
	}
};

struct optimize_ac_time {
	u8 trfc;
	u8 trfrc_05t;
	u8 trfc_pb;
	u8 trfrc_pb05t;
	u16 tx_ref_cnt;
};

void dramc_set_broadcast(u32 onoff)
{
	write32(&mt8183_infracfg->dramc_wbr, onoff);
}

u32 dramc_get_broadcast(void)
{
	return read32(&mt8183_infracfg->dramc_wbr);
}

u32 get_shu_freq(u8 shu)
{
	const u8 *freq_tbl = CONFIG(MT8183_DRAM_EMCP) ?
		freq_shuffle_emcp : freq_shuffle;
	return frequency_table[freq_tbl[shu]];
}

static u64 get_ch_rank_size(u8 chn, u8 rank)
{
	u32 shift_for_16bit = 1;
	u32 col_bit, row_bit;
	u32 emi_cona = read32(&emi_regs->cona);

	shift_for_16bit = (emi_cona & 0x2) ? 0 : 1;

	col_bit = ((emi_cona >> (chn * 16 + rank * 2 + 4)) & 0x03) + 9;
	row_bit = ((((emi_cona >> (24 - chn * 20 + rank)) & 0x01) << 2) +
		((emi_cona >> (12 + chn * 16 + rank * 2)) & 0x03)) + 13;

	/* Data width (bytes) * 8 banks */
	return ((u64)(1 << (row_bit + col_bit))) *
		((u64)(4 >> shift_for_16bit) * 8);
}

void dramc_get_rank_size(u64 *dram_rank_size)
{
	u64 ch0_rank0_size, ch0_rank1_size, ch1_rank0_size, ch1_rank1_size;
	u64 ch_rank0_size = 0, ch_rank1_size = 0;
	u32 emi_cona = read32(&emi_regs->cona);
	u32 emi_conh = read32(&emi_regs->conh);

	dram_rank_size[0] = 0;
	dram_rank_size[1] = 0;

	ch0_rank0_size = (emi_conh >> 16) & 0xf;
	ch0_rank1_size = (emi_conh >> 20) & 0xf;
	ch1_rank0_size = (emi_conh >> 24) & 0xf;
	ch1_rank1_size = (emi_conh >> 28) & 0xf;

	/* CH0 EMI */
	if (ch0_rank0_size == 0)
		ch_rank0_size = get_ch_rank_size(CHANNEL_A, RANK_0);
	else
		ch_rank0_size = (ch0_rank0_size * 256 << 20);

	/* Dual rank enable */
	if ((emi_cona & (1 << 17)) != 0) {
		if (ch0_rank1_size == 0)
			ch_rank1_size = get_ch_rank_size(CHANNEL_A, RANK_1);
		else
			ch_rank1_size = (ch0_rank1_size * 256 << 20);
	}

	dram_rank_size[0] = ch_rank0_size;
	dram_rank_size[1] = ch_rank1_size;

	if (ch1_rank0_size == 0)
		ch_rank0_size = get_ch_rank_size(CHANNEL_B, RANK_0);
	else
		ch_rank0_size = (ch1_rank0_size * 256 << 20);

	if ((emi_cona & (1 << 16)) != 0) {
		if (ch1_rank1_size == 0)
			ch_rank1_size = get_ch_rank_size(CHANNEL_B, RANK_1);
		else
			ch_rank1_size = (ch1_rank1_size * 256 << 20);
	}
	dram_rank_size[0] += ch_rank0_size;
	dram_rank_size[1] += ch_rank1_size;
}

size_t sdram_size(void)
{
	size_t dram_size = 0;
	u64 rank_size[RANK_MAX];

	dramc_get_rank_size(rank_size);

	for (int i = 0; i < RANK_MAX; i++)
		dram_size += rank_size[i];

	return dram_size;
}

static void set_rank_info_to_conf(const struct sdram_params *params)
{
	bool is_dual_rank = (params->emi_cona_val & (0x1 << 17)) != 0;
	clrsetbits32(&ch[0].ao.rstmask, 0x1 << 12,
			(is_dual_rank ? 0 : 1) << 12);
}

void cbt_mrr_pinmux_mapping(void)
{
	for (size_t chn = 0; chn < CHANNEL_MAX; chn++) {
		const u8 *map = phy_mapping[chn];
		write32(&ch[chn].ao.mrr_bit_mux1,
			(map[8] << 0) | (map[9] << 8) |
			(map[10] << 16) | (map[11] << 24));

		write32(&ch[chn].ao.mrr_bit_mux2,
			(map[12] << 0) | (map[13] << 8));
	}
}

void set_mrr_pinmux_mapping(void)
{
	for (size_t chn = 0; chn < CHANNEL_MAX; chn++) {
		const u8 *map = phy_mapping[chn];
		write32(&ch[chn].ao.mrr_bit_mux1,
			(map[0] << 0) | (map[1] << 8) |
			(map[2] << 16) | (map[3] << 24));

		write32(&ch[chn].ao.mrr_bit_mux2,
			(map[4] << 0) | (map[5] << 8) |
			(map[6] << 16) | (map[7] << 24));

		write32(&ch[chn].ao.mrr_bit_mux3,
			(map[8] << 0) | (map[9] << 8) |
			(map[10] << 16) | (map[11] << 24));

		write32(&ch[chn].ao.mrr_bit_mux4,
			(map[12] << 0) | (map[13] << 8) |
			(map[14] << 16) | (map[15] << 24));
	}
}

static void global_option_init(const struct sdram_params *params)
{
	set_rank_info_to_conf(params);
	set_mrr_pinmux_mapping();
}

static void set_vcore_voltage(u8 freq_group)
{
	const u32 vcore = vcore_lp4x[freq_group];
	dramc_dbg("Set DRAM voltage (freq %d): vcore = %u\n",
		  frequency_table[freq_group], vcore);
	pmic_set_vcore_vol(vcore);
}

static void set_vdram1_vddq_voltage(void)
{
	const u32 vdram1 = 1125000;
	const u32 vddq = 600000;
	dramc_dbg("Set DRAM voltage: vdram1 = %u, vddq = %u\n",
		  vdram1, vddq);
	pmic_set_vdram1_vol(vdram1);
	pmic_set_vddq_vol(vddq);
}

static void emi_esl_setting1(void)
{
	dramc_set_broadcast(DRAMC_BROADCAST_ON);

	write32(&emi_regs->cona, 0xa053a154);
	write32(&emi_regs->conb, 0x17283544);
	write32(&emi_regs->conc, 0x0a1a0b1a);
	write32(&emi_regs->cond, 0x3657587a);
	write32(&emi_regs->cone, 0x80400148);
	write32(&emi_regs->conf, 0x00000000);
	write32(&emi_regs->cong, 0x2b2b2a38);
	write32(&emi_regs->conh, 0x00000000);
	write32(&emi_regs->coni, 0x00008803);
	write32(&emi_regs->conm, 0x000001ff);
	write32(&emi_regs->conn, 0x00000000);
	write32(&emi_regs->mdct, 0x11338c17);
	write32(&emi_regs->mdct_2nd, 0x00001112);
	write32(&emi_regs->iocl, 0xa8a8a8a8);
	write32(&emi_regs->iocl_2nd, 0x25252525);
	write32(&emi_regs->iocm, 0xa8a8a8a8);
	write32(&emi_regs->iocm_2nd, 0x25252525);
	write32(&emi_regs->testb, 0x00060037);
	write32(&emi_regs->testc, 0x38460000);
	write32(&emi_regs->testd, 0x00000000);
	write32(&emi_regs->arba, 0x4020524f);
	write32(&emi_regs->arbb, 0x4020504f);
	write32(&emi_regs->arbc, 0xa0a050c6);
	write32(&emi_regs->arbd, 0x000070cc);
	write32(&emi_regs->arbe, 0x40406045);
	write32(&emi_regs->arbf, 0xa0a070d5);
	write32(&emi_regs->arbg, 0xa0a0504f);
	write32(&emi_regs->arbh, 0xa0a0504f);
	write32(&emi_regs->arbi, 0x00007108);
	write32(&emi_regs->arbi_2nd, 0x00007108);
	write32(&emi_regs->slct, 0x0001ff00);

	write32(&ch[0].emi.chn_cona, 0x0400a051);
	write32(&ch[0].emi.chn_conb, 0x00ff2048);
	write32(&ch[0].emi.chn_conc, 0x00000000);
	write32(&ch[0].emi.chn_mdct, 0x88008817);
	write32(&ch[0].emi.chn_testb, 0x00030027);
	write32(&ch[0].emi.chn_testc, 0x38460002);
	write32(&ch[0].emi.chn_testd, 0x00000000);
	write32(&ch[0].emi.chn_md_pre_mask, 0x00000f00);
	write32(&ch[0].emi.chn_md_pre_mask_shf, 0x00000b00);
	write32(&ch[0].emi.chn_arbi, 0x20406188);
	write32(&ch[0].emi.chn_arbi_2nd, 0x20406188);
	write32(&ch[0].emi.chn_arbj, 0x3719595e);
	write32(&ch[0].emi.chn_arbj_2nd, 0x3719595e);
	write32(&ch[0].emi.chn_arbk, 0x64f3fc79);
	write32(&ch[0].emi.chn_arbk_2nd, 0x64f3fc79);
	write32(&ch[0].emi.chn_slct, 0x00080888);
	write32(&ch[0].emi.chn_arb_ref, 0x82410222);
	write32(&ch[0].emi.chn_emi_shf0, 0x8a228c17);
	write32(&ch[0].emi.chn_rkarb0, 0x0006002f);
	write32(&ch[0].emi.chn_rkarb1, 0x01010101);
	write32(&ch[0].emi.chn_rkarb2, 0x10100820);
	write32(&ch[0].emi.chn_eco3, 0x00000000);

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
}

static void emi_esl_setting2(void)
{
	dramc_set_broadcast(DRAMC_BROADCAST_ON);

	write32(&ch[0].emi.chn_conc, 0x01);
	write32(&emi_regs->conm, 0x05ff);

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
}

static void emi_init(const struct sdram_params *params)
{
	emi_esl_setting1();

	write32(&emi_regs->cona, params->emi_cona_val);
	write32(&emi_regs->conf, params->emi_conf_val);
	write32(&emi_regs->conh, params->emi_conh_val);

	for (size_t chn = CHANNEL_A; chn < CHANNEL_MAX; chn++) {
		write32(&ch[chn].emi.chn_cona, params->chn_emi_cona_val[chn]);
		write32(&ch[chn].emi.chn_conc, 0);
	}
}

static void emi_init2(const struct sdram_params *params)
{
	emi_esl_setting2();

	setbits32(&emi_mpu->mpu_ctrl_d[1], 0x1 << 4);
	setbits32(&emi_mpu->mpu_ctrl_d[7], 0x1 << 4);
	if (CONFIG(MT8183_DRAM_EMCP))
		write32(&emi_regs->bwct0, 0x0d000705);
	else
		write32(&emi_regs->bwct0, 0x0a000705);
	write32(&emi_regs->bwct0_3rd, 0x0);

	/* EMI QoS 0.5 */
	write32(&emi_regs->bwct0_2nd, 0x00030023);
	write32(&emi_regs->bwct0_4th, 0x00c00023);
	write32(&emi_regs->bwct0_5th, 0x00240023);
}

static void dramc_init_pre_settings(void)
{
	clrsetbits32(&ch[0].phy.ca_cmd[8],
		(0x1 << 21) | (0x1 << 20) | (0x1 << 19) | (0x1 << 18) |
		(0x1f << 8) | (0x1f << 0),
		(0x1 << 19) | (0xa << 8) | (0xa << 0));

	setbits32(&ch[0].phy.misc_ctrl1, 0x1 << 12);
	clrbits32(&ch[0].phy.misc_ctrl1, 0x1 << 13);
	setbits32(&ch[0].phy.misc_ctrl1, 0x1 << 31);
}

static void dramc_ac_timing_optimize(u8 freq_group, u8 density)
{
	u8 rfcab_grp = 0;
	u8 trfc, trfrc_05t, trfc_pb, trfrc_pb05t, tx_ref_cnt;
	enum tRFCAB {
		tRFCAB_130 = 0,
		tRFCAB_180,
		tRFCAB_280,
		tRFCAB_380,
		tRFCAB_NUM
	};

	const struct optimize_ac_time rf_cab_opt[LP4X_DDRFREQ_MAX][tRFCAB_NUM] = {
		[LP4X_DDR1600] = {
			[tRFCAB_130] = {.trfc = 14, .trfrc_05t = 0, .trfc_pb = 0,
				.trfrc_pb05t = 0, .tx_ref_cnt = 32},
			[tRFCAB_180] = {.trfc = 24, .trfrc_05t = 0, .trfc_pb = 6,
				.trfrc_pb05t = 0, .tx_ref_cnt = 42},
			[tRFCAB_280] = {.trfc = 44, .trfrc_05t = 0, .trfc_pb = 16,
				.trfrc_pb05t = 0, .tx_ref_cnt = 62},
			[tRFCAB_380] = {.trfc = 64, .trfrc_05t = 0, .trfc_pb = 26,
				.trfrc_pb05t = 0, .tx_ref_cnt = 82}
		},
		[LP4X_DDR2400] = {
			[tRFCAB_130] = {.trfc = 27, .trfrc_05t = 0, .trfc_pb = 6,
				.trfrc_pb05t = 0, .tx_ref_cnt = 46},
			[tRFCAB_180] = {.trfc = 42, .trfrc_05t = 0, .trfc_pb = 15,
				.trfrc_pb05t = 0, .tx_ref_cnt = 61},
			[tRFCAB_280] = {.trfc = 72, .trfrc_05t = 0, .trfc_pb = 30,
				.trfrc_pb05t = 0, .tx_ref_cnt = 91},
			[tRFCAB_380] = {.trfc = 102, .trfrc_05t = 0, .trfc_pb = 45,
				.trfrc_pb05t = 0, .tx_ref_cnt = 121}
		},
		[LP4X_DDR3200] = {
			[tRFCAB_130] = {.trfc = 40, .trfrc_05t = 0, .trfc_pb = 12,
				.trfrc_pb05t = 0, .tx_ref_cnt = 59},
			[tRFCAB_180] = {.trfc = 60, .trfrc_05t = 0, .trfc_pb = 24,
				.trfrc_pb05t = 0, .tx_ref_cnt = 79},
			[tRFCAB_280] = {.trfc = 100, .trfrc_05t = 0, .trfc_pb = 44,
				.trfrc_pb05t = 0, .tx_ref_cnt = 119},
			[tRFCAB_380] = {.trfc = 140, .trfrc_05t = 0, .trfc_pb = 64,
				.trfrc_pb05t = 0, .tx_ref_cnt = 159}
		},
		[LP4X_DDR3600] = {
			[tRFCAB_130] = {.trfc = 48, .trfrc_05t = 1, .trfc_pb = 16,
				.trfrc_pb05t = 0, .tx_ref_cnt = 68},
			[tRFCAB_180] = {.trfc = 72, .trfrc_05t = 0, .trfc_pb = 30,
				.trfrc_pb05t = 0, .tx_ref_cnt = 92},
			[tRFCAB_280] = {.trfc = 118, .trfrc_05t = 1, .trfc_pb = 53,
				.trfrc_pb05t = 1, .tx_ref_cnt = 138},
			[tRFCAB_380] = {.trfc = 165, .trfrc_05t = 0, .trfc_pb = 76,
				.trfrc_pb05t = 1, .tx_ref_cnt = 185}
		},
	};

	switch (density) {
	case 0x0:
		rfcab_grp = tRFCAB_130;
		break;
	case 0x1:
	case 0x2:
		rfcab_grp = tRFCAB_180;
		break;
	case 0x3:
	case 0x4:
		rfcab_grp = tRFCAB_280;
		break;
	case 0x5:
	case 0x6:
		rfcab_grp = tRFCAB_380;
		break;
	default:
		dramc_err("density err!\n");
		break;
	}

	const struct optimize_ac_time *ac_tim = &rf_cab_opt[freq_group][rfcab_grp];
	trfc = ac_tim->trfc;
	trfrc_05t = ac_tim->trfrc_05t;
	trfc_pb = ac_tim->trfc_pb;
	trfrc_pb05t = ac_tim->trfrc_pb05t;
	tx_ref_cnt = ac_tim->tx_ref_cnt;
	dramc_dbg("Density %d, trfc %u, trfrc_05t %d, tx_ref_cnt %d, trfc_pb %d, trfrc_pb05t %d\n",
		density, trfc, trfrc_05t, tx_ref_cnt, trfc_pb, trfrc_pb05t);

	for (size_t chn = 0; chn < CHANNEL_MAX; chn++) {
		clrsetbits32(&ch[chn].ao.shu[0].actim[3],
			0xff << 16, trfc << 16);
		clrsetbits32(&ch[chn].ao.shu[0].ac_time_05t,
			0x1 << 2, trfrc_05t << 2);
		clrsetbits32(&ch[chn].ao.shu[0].actim[4],
			0x3ff << 0, tx_ref_cnt << 0);
		clrsetbits32(&ch[chn].ao.shu[0].actim[3],
			0xff << 0, trfc_pb << 0);
		clrsetbits32(&ch[chn].ao.shu[0].ac_time_05t,
			0x1 << 1, trfrc_pb05t << 1);
	}
}

static void spm_pinmux_setting(void)
{
	clrsetbits32(&mtk_spm->poweron_config_set,
		(0xffff << 16) | (0x1 << 0), (0xb16 << 16) | (0x1 << 0));
	clrbits32(&mtk_spm->pcm_pwr_io_en, (0xff << 0) | (0xff << 16));
	write32(&mtk_spm->dramc_dpy_clk_sw_con_sel, 0xffffffff);
	write32(&mtk_spm->dramc_dpy_clk_sw_con_sel2, 0xffffffff);
}

static void dfs_init_for_calibration(const struct sdram_params *params,
				     u8 freq_group,
				     struct dram_shared_data *shared)
{
	dramc_init(params, freq_group, shared);
	dramc_apply_config_before_calibration(freq_group, params->cbt_mode_extern);
}

static void init_dram(const struct sdram_params *params, u8 freq_group,
		      struct dram_shared_data *shared)
{
	global_option_init(params);
	emi_init(params);

	dramc_set_broadcast(DRAMC_BROADCAST_ON);
	dramc_init_pre_settings();
	spm_pinmux_setting();

	dramc_sw_impedance_cal(params, ODT_OFF, &shared->impedance);
	dramc_sw_impedance_cal(params, ODT_ON, &shared->impedance);

	dramc_init(params, freq_group, shared);
	dramc_apply_config_before_calibration(freq_group, params->cbt_mode_extern);
	emi_init2(params);
}

void enable_emi_dcm(void)
{
	clrbits32(&emi_regs->conm, 0xff << 24);
	clrbits32(&emi_regs->conn, 0xff << 24);

	for (size_t chn = 0; chn < CHANNEL_MAX; chn++)
		clrbits32(&ch[chn].emi.chn_conb, 0xff << 24);
}

struct shuffle_reg_addr {
	u32 start;
	u32 end;
};

#define AO_SHU_ADDR(s, e) \
	{ \
		.start = offsetof(struct dramc_ao_regs_shu, s), \
		.end = offsetof(struct dramc_ao_regs_shu, e), \
	}

static const struct shuffle_reg_addr dramc_regs[] = {
	AO_SHU_ADDR(actim, hwset_vrcg),
	AO_SHU_ADDR(rk[0], rk[0].dqs2dq_cal5),
	AO_SHU_ADDR(rk[1], rk[1].dqs2dq_cal5),
	AO_SHU_ADDR(rk[2], rk[2].dqs2dq_cal5),
	AO_SHU_ADDR(dqsg_retry, dqsg_retry),
};

#define PHY_SHU_ADDR(s, e) \
	{ \
		.start = offsetof(struct ddrphy_ao_shu, s), \
		.end = offsetof(struct ddrphy_ao_shu, e), \
	}

static const struct shuffle_reg_addr phy_regs[] = {
	PHY_SHU_ADDR(b[0], b[0].dll[1]),
	PHY_SHU_ADDR(b[1], b[1].dll[1]),
	PHY_SHU_ADDR(ca_cmd, ca_dll[1]),
	PHY_SHU_ADDR(pll[0], pll[15]),
	PHY_SHU_ADDR(pll20, misc0),
	PHY_SHU_ADDR(rk[0].b[0], rk[0].b[0].rsvd_20[3]),
	PHY_SHU_ADDR(rk[0].b[1], rk[0].b[1].rsvd_20[3]),
	PHY_SHU_ADDR(rk[0].ca_cmd, rk[0].rsvd_22[1]),
	PHY_SHU_ADDR(rk[1].b[0], rk[1].b[0].rsvd_20[3]),
	PHY_SHU_ADDR(rk[1].b[1], rk[1].b[1].rsvd_20[3]),
	PHY_SHU_ADDR(rk[1].ca_cmd, rk[1].rsvd_22[1]),
	PHY_SHU_ADDR(rk[2].b[0], rk[2].b[0].rsvd_20[3]),
	PHY_SHU_ADDR(rk[2].b[1], rk[2].b[1].rsvd_20[3]),
	PHY_SHU_ADDR(rk[2].ca_cmd, rk[2].rsvd_22[1]),
};

static void dramc_save_result_to_shuffle(u32 src_shuffle, u32 dst_shuffle)
{
	u32 offset, chn, index, value;
	u8 *src_addr, *dst_addr;

	if (src_shuffle == dst_shuffle)
		return;

	dramc_dbg("Save shuffle %u to shuffle %u\n", src_shuffle, dst_shuffle);

	for (chn = 0; chn < CHANNEL_MAX; chn++) {
		/* DRAMC */
		for (index = 0; index < ARRAY_SIZE(dramc_regs); index++) {
			for (offset = dramc_regs[index].start;
				offset <= dramc_regs[index].end; offset += 4) {
				src_addr = (u8 *)&ch[chn].ao.shu[src_shuffle] +
					offset;
				dst_addr = (u8 *)&ch[chn].ao.shu[dst_shuffle] +
					offset;
				write32(dst_addr, read32(src_addr));

			}
		}

		/* DRAMC-exception-1 */
		src_addr = (u8 *)&ch[chn].ao.shuctrl2;
		dst_addr = (u8 *)&ch[chn].ao.dvfsdll;
		value = read32(src_addr) & 0x7f;

		if (dst_shuffle == DRAM_DFS_SHUFFLE_2)
			clrsetbits32(dst_addr, 0x7f << 8, value << 8);
		else if (dst_shuffle == DRAM_DFS_SHUFFLE_3)
			clrsetbits32(dst_addr, 0x7f << 16, value << 16);

		/* DRAMC-exception-2 */
		src_addr = (u8 *)&ch[chn].ao.dvfsdll;
		value = (read32(src_addr) >> 1) & 0x1;

		if (dst_shuffle == DRAM_DFS_SHUFFLE_2)
			clrsetbits32(src_addr, 0x1 << 2, value << 2);
		else if (dst_shuffle == DRAM_DFS_SHUFFLE_3)
			clrsetbits32(src_addr, 0x1 << 3, value << 3);

		/* PHY */
		for (index = 0; index < ARRAY_SIZE(phy_regs); index++) {
			for (offset = phy_regs[index].start;
				offset <= phy_regs[index].end; offset += 4) {
				src_addr = (u8 *)&ch[chn].phy.shu[src_shuffle] +
					offset;
				dst_addr = (u8 *)&ch[chn].phy.shu[dst_shuffle] +
					offset;
				write32(dst_addr, read32(src_addr));

			}
		}
	}
}

static int run_calib(const struct dramc_param *dparam,
		     struct dram_shared_data *shared,
		     const int shuffle, bool *first_run)
{
	u8 density;
	const u8 *freq_tbl;

	if (CONFIG(MT8183_DRAM_EMCP))
		freq_tbl = freq_shuffle_emcp;
	else
		freq_tbl = freq_shuffle;

	const u8 freq_group = freq_tbl[shuffle];
	const struct sdram_params *params = &dparam->freq_params[shuffle];

	set_vcore_voltage(freq_group);

	dramc_show("Run calibration (freq: %u, first: %d)\n",
		   frequency_table[freq_group], *first_run);

	if (*first_run)
		init_dram(params, freq_group, shared);
	else
		dfs_init_for_calibration(params, freq_group, shared);
	*first_run = false;

	dramc_dbg("Start K (current clock: %u\n", params->frequency);
	if (dramc_calibrate_all_channels(params, freq_group, &shared->mr,
					 !!(dparam->header.config & DRAMC_CONFIG_DVFS)) != 0)
		return -1;
	get_dram_info_after_cal(&density, params->rank_num);
	dramc_ac_timing_optimize(freq_group, density);
	dramc_dbg("K finished (current clock: %u\n", params->frequency);

	dramc_save_result_to_shuffle(DRAM_DFS_SHUFFLE_1, shuffle);
	return 0;
}

static void after_calib(const struct mr_value *mr, u32 rk_num)
{
	dramc_apply_config_after_calibration(mr, rk_num);
	dramc_runtime_config(rk_num);
}

int mt_set_emi(const struct dramc_param *dparam)
{
	struct dram_shared_data shared;
	bool first_run = true;
	set_vdram1_vddq_voltage();

	if (dparam->header.config & DRAMC_CONFIG_DVFS) {
		if (run_calib(dparam, &shared, DRAM_DFS_SHUFFLE_3,
			      &first_run) != 0)
			return -1;
		if (run_calib(dparam, &shared, DRAM_DFS_SHUFFLE_2,
			      &first_run) != 0)
			return -1;
	}

	if (run_calib(dparam, &shared, DRAM_DFS_SHUFFLE_1, &first_run) != 0)
		return -1;

	after_calib(&shared.mr, dparam->freq_params[DRAM_DFS_SHUFFLE_1].rank_num);
	return 0;
}
