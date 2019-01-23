/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/mmio.h>
#include <soc/emi.h>
#include <soc/infracfg.h>
#include <soc/dramc_pi_api.h>
#include <soc/dramc_register.h>

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

void dramc_set_broadcast(u32 onoff)
{
	write32(&mt8183_infracfg->dramc_wbr, onoff);
}

u32 dramc_get_broadcast(void)
{
	return read32(&mt8183_infracfg->dramc_wbr);
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
	clrsetbits_le32(&ch[0].ao.rstmask, 0x1 << 12,
			(is_dual_rank ? 0 : 1) << 12);
}

static void set_MRR_pinmux_mapping(void)
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
	set_MRR_pinmux_mapping();
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

	setbits_le32(&emi_mpu->mpu_ctrl_d[1], 0x1 << 4);
	setbits_le32(&emi_mpu->mpu_ctrl_d[7], 0x1 << 4);

	write32(&emi_regs->bwct0, 0x0a000705);
	write32(&emi_regs->bwct0_3rd, 0x0);

	/* EMI QoS 0.5 */
	write32(&emi_regs->bwct0_2nd, 0x00030023);
	write32(&emi_regs->bwct0_4th, 0x00c00023);
	write32(&emi_regs->bwct0_5th, 0x00240023);
}

static void dramc_init_pre_settings(void)
{
	clrsetbits_le32(&ch[0].phy.ca_cmd[8],
		(0x1 << 21) | (0x1 << 20) | (0x1 << 19) | (0x1 << 18) |
		(0x1f << 8) | (0x1f << 0),
		(0x1 << 19) | (0xa << 8) | (0xa << 0));

	setbits_le32(&ch[0].phy.misc_ctrl1, 0x1 << 12);
	clrbits_le32(&ch[0].phy.misc_ctrl1, 0x1 << 13);
	setbits_le32(&ch[0].phy.misc_ctrl1, 0x1 << 31);
}

static void dramc_ac_timing_optimize(void)
{
	for (size_t chn = 0; chn < CHANNEL_MAX; chn++) {
		clrsetbits_le32(&ch[chn].ao.shu[0].actim[3],
			0xff << 16, 0x64 << 16);
		clrbits_le32(&ch[chn].ao.shu[0].ac_time_05t, 0x1 << 2);
		clrsetbits_le32(&ch[chn].ao.shu[0].actim[4],
			0x3ff << 0, 0x77 << 0);
	}
}

static void init_dram(const struct sdram_params *params)
{
	global_option_init(params);
	emi_init(params);

	dramc_set_broadcast(DRAMC_BROADCAST_ON);
	dramc_init_pre_settings();
	dramc_sw_impedance(params);

	dramc_init();
	emi_init2(params);
}

void enable_emi_dcm(void)
{
	clrbits_le32(&emi_regs->conm, 0xff << 24);
	clrbits_le32(&emi_regs->conn, 0xff << 24);

	for (size_t chn = 0; chn < CHANNEL_MAX; chn++)
		clrbits_le32(&ch[chn].emi.chn_conb, 0xff << 24);
}

static void do_calib(const struct sdram_params *params)
{
	dramc_apply_config_before_calibration();
	dramc_calibrate_all_channels(params);
	dramc_ac_timing_optimize();
	dramc_apply_config_after_calibration();
	dramc_runtime_config();
}

void mt_set_emi(const struct sdram_params *params)
{
	init_dram(params);
	do_calib(params);
}
