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

#include <assert.h>
#include <delay.h>
#include <arch/io.h>
#include <soc/emi.h>
#include <soc/dramc_register.h>
#include <soc/dramc_pi_api.h>

enum {
	GATING_START = 26,
	GATING_END = GATING_START + 24,
};

static void auto_refresh_switch(u8 chn, u8 option)
{
	clrsetbits_le32(&ch[chn].ao.refctrl0, 1 << REFCTRL0_REFDIS_SHIFT,
		(option ? 0 : 1) << REFCTRL0_REFDIS_SHIFT);

	if (option == DISABLE) {
		/*
		 * Because HW will actually disable autorefresh after
		 * refresh_queue empty, we need to wait until queue empty.
		 */
		udelay(((read32(&ch[chn].nao.misc_statusa) &
				MISC_STATUSA_REFRESH_QUEUE_CNT_MASK) >>
				MISC_STATUSA_REFRESH_QUEUE_CNT_SHIFT) * 4);
	}
}

static void dramc_cke_fix_onoff(int option, u8 chn)
{
	u8 on = 0, off = 0;

	/* If CKE is dynamic, set both CKE fix On and Off as 0. */
	if (option != CKE_DYNAMIC) {
		on = option;
		off = (1 - option);
	}

	clrsetbits_le32(&ch[chn].ao.ckectrl,
		(0x1 << 6) | (0x1 << 7), (on << 6) | (off << 7));
}

static void dramc_mode_reg_write(u8 chn, u8 mr_idx, u8 value)
{
	u32 ckectrl_bak = read32(&ch[chn].ao.ckectrl);

	dramc_cke_fix_onoff(CKE_FIXON, chn);
	clrsetbits_le32(&ch[chn].ao.mrs,
		MRS_MRSMA_MASK, mr_idx << MRS_MRSMA_SHIFT);
	clrsetbits_le32(&ch[chn].ao.mrs,
		MRS_MRSOP_MASK, value << MRS_MRSOP_SHIFT);
	setbits_le32(&ch[chn].ao.spcmd, 1 << SPCMD_MRWEN_SHIFT);

	/* Wait MRW command fired */
	while ((read32(&ch[chn].nao.spcmdresp) & 1) == 0)
		;

	clrbits_le32(&ch[chn].ao.spcmd, 1 << SPCMD_MRWEN_SHIFT);
	setbits_le32(&ch[chn].ao.ckectrl, ckectrl_bak);
}

static void dramc_mode_reg_write_by_rank(u8 chn, u8 rank,
		u8 mr_idx, u8 value)
{
	u32 mrs_back = read32(&ch[chn].ao.mrs) & MRS_MRSRK_MASK;

	clrsetbits_le32(&ch[chn].ao.mrs,
		MRS_MRSRK_MASK, rank << MRS_MRSRK_SHIFT);
	dramc_mode_reg_write(chn, mr_idx, value);
	clrsetbits_le32(&ch[chn].ao.mrs, MRS_MRSRK_MASK, mrs_back);
}

static void dramc_write_leveling(u8 chn, u8 rank,
		const u8 wr_level[CHANNEL_MAX][RANK_MAX][DQS_NUMBER])
{
	clrsetbits_le32(&ch[chn].phy.shu[0].rk[rank].ca_cmd[9],
		SHU1_CA_CMD9_RG_RK_ARFINE_TUNE_CLK_MASK, 0);

	for (size_t i = 0; i < DQS_NUMBER; i++) {
		s32 wrlevel_dq_delay = wr_level[chn][rank][i] + 0x10;
		assert(wrlevel_dq_delay < 0x40);

		clrsetbits_le32(&ch[chn].phy.shu[0].rk[rank].b[i].dq[7],
			FINE_TUNE_PBYTE_MASK | FINE_TUNE_DQM_MASK |
			FINE_TUNE_DQ_MASK,
			(wr_level[chn][rank][i] << FINE_TUNE_PBYTE_SHIFT) |
			(wrlevel_dq_delay << FINE_TUNE_DQM_SHIFT) |
			(wrlevel_dq_delay << FINE_TUNE_DQ_SHIFT));
	}
}

static void cmd_bus_training(u8 chn, u8 rank,
		const struct sdram_params *params)
{
	u32 cbt_cs, mr12_value;

	cbt_cs = params->cbt_cs[chn][rank];
	mr12_value = params->cbt_mr12[chn][rank];

	/* CBT adjust cs */
	clrsetbits_le32(&ch[chn].phy.shu[0].rk[rank].ca_cmd[9],
		SHU1_CA_CMD9_RG_RK_ARFINE_TUNE_CS_MASK, cbt_cs << 0);

	/* CBT set vref */
	dramc_mode_reg_write_by_rank(chn, rank, 12, mr12_value);
}

static void dramc_read_dbi_onoff(u8 onoff)
{
	for (size_t chn = 0; chn < CHANNEL_MAX; chn++)
		for (size_t b = 0; b < 2; b++)
			clrsetbits_le32(&ch[chn].phy.shu[0].b[b].dq[7],
				0x1 << SHU1_BX_DQ7_R_DMDQMDBI_SHU_SHIFT,
				onoff << SHU1_BX_DQ7_R_DMDQMDBI_SHU_SHIFT);
}

static void dramc_write_dbi_onoff(u8 onoff)
{
	for (size_t chn = 0; chn < CHANNEL_MAX; chn++)
		clrsetbits_le32(&ch[chn].ao.shu[0].wodt,
			0x1 << SHU1_WODT_DBIWR_SHIFT,
			onoff << SHU1_WODT_DBIWR_SHIFT);
}

static void dramc_phy_dcm_disable(u8 chn)
{
	clrsetbits_le32(&ch[chn].phy.misc_cg_ctrl0,
		(0x1 << 20) | (0x1 << 19) | 0x3ff << 8,
		(0x0 << 20) | (0x1 << 19) | 0x3ff << 8);

	for (size_t i = 0; i < DRAM_DFS_SHUFFLE_MAX; i++) {
		struct ddrphy_ao_shu *shu = &ch[chn].phy.shu[i];
		setbits_le32(&shu->b[0].dq[8], 0x1fff << 19);
		setbits_le32(&shu->b[1].dq[8], 0x1fff << 19);
		clrbits_le32(&shu->ca_cmd[8], 0x1fff << 19);
	}
	clrbits_le32(&ch[chn].phy.misc_cg_ctrl5, (0x7 << 16) | (0x7 << 20));
}

static void dramc_enable_phy_dcm(u8 en)
{
	u32 broadcast_bak = dramc_get_broadcast();
	dramc_set_broadcast(DRAMC_BROADCAST_OFF);

	for (size_t chn = 0; chn < CHANNEL_MAX ; chn++) {
		clrbits_le32(&ch[chn].phy.b[0].dll_fine_tune[1], 0x1 << 20);
		clrbits_le32(&ch[chn].phy.b[1].dll_fine_tune[1], 0x1 << 20);
		clrbits_le32(&ch[chn].phy.ca_dll_fine_tune[1], 0x1 << 20);

		for (size_t i = 0; i < DRAM_DFS_SHUFFLE_MAX; i++) {
			struct ddrphy_ao_shu *shu = &ch[chn].phy.shu[i];
			setbits_le32(&shu->b[0].dll[0], 0x1);
			setbits_le32(&shu->b[1].dll[0], 0x1);
			setbits_le32(&shu->ca_dll[0], 0x1);
		}

		clrsetbits_le32(&ch[chn].ao.dramc_pd_ctrl,
			(0x1 << 0) | (0x1 << 1) | (0x1 << 2) |
			(0x1 << 5) | (0x1 << 26) | (0x1 << 30) | (0x1 << 31),
			((en ? 0x1 : 0) << 0) | ((en ? 0x1 : 0) << 1) |
			((en ? 0x1 : 0) << 2) | ((en ? 0 : 0x1) << 5) |
			((en ? 0 : 0x1) << 26) | ((en ? 0x1 : 0) << 30) |
			((en ? 0x1 : 0) << 31));

		/* DCM on: CHANNEL_EMI free run; DCM off: mem_dcm */
		assert(en == 0 || en == 1);
		write32(&ch[chn].phy.misc_cg_ctrl2, 0x8060033e | (0x40 << en));
		write32(&ch[chn].phy.misc_cg_ctrl2, 0x8060033f | (0x40 << en));
		write32(&ch[chn].phy.misc_cg_ctrl2, 0x8060033e | (0x40 << en));

		clrsetbits_le32(&ch[chn].phy.misc_ctrl3, 0x3 << 26,
			(en ? 0 : 0x3) << 26);
		for (size_t i = 0; i < DRAM_DFS_SHUFFLE_MAX; i++) {
			u32 mask = 0x7 << 17;
			u32 value = (en ? 0x7 : 0) << 17;
			struct ddrphy_ao_shu *shu = &ch[chn].phy.shu[i];

			clrsetbits_le32(&shu->b[0].dq[7], mask, value);
			clrsetbits_le32(&shu->b[1].dq[7], mask, value);
			clrsetbits_le32(&shu->ca_cmd[7], mask, value);
		}

		if (!en)
			dramc_phy_dcm_disable(chn);
	}
	dramc_set_broadcast(broadcast_bak);
}

static void reset_delay_chain_before_calibration(void)
{
	for (size_t chn = 0; chn < CHANNEL_MAX; chn++)
		for (size_t rank = 0; rank < RANK_MAX; rank++) {
			struct dramc_ddrphy_regs_shu_rk *rk;
			rk = &ch[chn].phy.shu[0].rk[rank];
			clrbits_le32(&rk->ca_cmd[0], 0xffffff << 0);
			clrbits_le32(&rk->b[0].dq[0], 0xfffffff << 0);
			clrbits_le32(&rk->b[1].dq[0], 0xfffffff << 0);
			clrbits_le32(&rk->b[0].dq[1], 0xf << 0);
			clrbits_le32(&rk->b[1].dq[1], 0xf << 0);
		}
}

static void dramc_hw_gating_onoff(u8 chn, u8 onoff)
{
	clrsetbits_le32(&ch[chn].ao.shuctrl2, 0x3 << 14,
		(onoff << 14) | (onoff << 15));
	clrsetbits_le32(&ch[chn].ao.stbcal2, 0x1 << 28, onoff << 28);
	clrsetbits_le32(&ch[chn].ao.stbcal, 0x1 << 24, onoff << 24);
	clrsetbits_le32(&ch[chn].ao.stbcal, 0x1 << 22, onoff << 22);
}

static void dramc_rx_input_delay_tracking_init_by_freq(u8 chn)
{
	struct ddrphy_ao_shu *shu = &ch[chn].phy.shu[0];

	clrsetbits_le32(&shu->b[0].dq[5], 0x7 << 20, 0x3 << 20);
	clrsetbits_le32(&shu->b[1].dq[5], 0x7 << 20, 0x3 << 20);
	clrbits_le32(&shu->b[0].dq[7], (0x1 << 12) | (0x1 << 13));
	clrbits_le32(&shu->b[1].dq[7], (0x1 << 12) | (0x1 << 13));
}

void dramc_apply_pre_calibration_config(void)
{
	dramc_enable_phy_dcm(0);
	reset_delay_chain_before_calibration();

	setbits_le32(&ch[0].ao.shu[0].conf[3], 0x1ff << 16);
	setbits_le32(&ch[0].ao.spcmdctrl, 0x1 << 24);
	clrsetbits_le32(&ch[0].ao.shu[0].scintv, 0x1f << 1, 0x1b << 1);

	for (size_t shu = 0; shu < DRAM_DFS_SHUFFLE_MAX; shu++)
		setbits_le32(&ch[0].ao.shu[shu].conf[3], 0x1ff << 0);

	clrbits_le32(&ch[0].ao.dramctrl, 0x1 << 18);
	clrbits_le32(&ch[0].ao.spcmdctrl, 0x1 << 31);
	clrbits_le32(&ch[0].ao.spcmdctrl, 0x1 << 30);
	clrbits_le32(&ch[0].ao.dqsoscr, 0x1 << 26);
	clrbits_le32(&ch[0].ao.dqsoscr, 0x1 << 25);

	dramc_write_dbi_onoff(DBI_OFF);
	dramc_read_dbi_onoff(DBI_OFF);

	for (size_t chn = 0; chn < CHANNEL_MAX; chn++) {
		setbits_le32(&ch[chn].ao.spcmdctrl, 0x1 << 29);
		setbits_le32(&ch[chn].ao.dqsoscr, 0x1 << 24);
		for (size_t shu = 0; shu < DRAM_DFS_SHUFFLE_MAX; shu++)
			setbits_le32(&ch[chn].ao.shu[shu].scintv, 0x1 << 30);

		clrbits_le32(&ch[chn].ao.dummy_rd, (0x1 << 7) | (0x7 << 20));
		dramc_hw_gating_onoff(chn, GATING_OFF);
		clrbits_le32(&ch[chn].ao.stbcal2, 0x1 << 28);

		setbits_le32(&ch[chn].phy.misc_ctrl1,
			(0x1 << 7) | (0x1 << 11));
		clrbits_le32(&ch[chn].ao.refctrl0, 0x1 << 18);
		clrbits_le32(&ch[chn].ao.mrs, 0x3 << 24);
		setbits_le32(&ch[chn].ao.mpc_option, 0x1 << 17);
		clrsetbits_le32(&ch[chn].phy.b[0].dq[6], 0x3 << 0, 0x1 << 0);
		clrsetbits_le32(&ch[chn].phy.b[1].dq[6], 0x3 << 0, 0x1 << 0);
		clrsetbits_le32(&ch[chn].phy.ca_cmd[6], 0x3 << 0, 0x1 << 0);
		setbits_le32(&ch[chn].ao.dummy_rd, 0x1 << 25);
		setbits_le32(&ch[chn].ao.drsctrl, 0x1 << 0);
		clrbits_le32(&ch[chn].ao.shu[1].drving[1], 0x1 << 31);

		dramc_rx_input_delay_tracking_init_by_freq(chn);
	}

	for (size_t r = 0; r < 2; r++) {
		for (size_t b = 0; b < 2; b++)
			clrbits_le32(&ch[0].phy.r[r].b[b].rxdvs[2],
				(0x1 << 28) | (0x1 << 23) | (0x3 << 30));
		clrbits_le32(&ch[0].phy.r0_ca_rxdvs[2], 0x3 << 30);
	}
}

static void rx_dqs_isi_pulse_cg_switch(u8 chn, bool flag)
{
	for (size_t b = 0; b < 2; b++)
		clrsetbits_le32(&ch[chn].phy.b[b].dq[6], 1 << 5,
				(flag ? 1 : 0) << 5);
}

static void dramc_set_rank_engine2(u8 chn, u8 rank)
{
	setbits_le32(&ch[chn].ao.dramctrl, 0x1 << 1);
	clrbits_le32(&ch[chn].ao.test2_4, TEST2_4_TESTAGENTRKSEL_MASK);
	clrsetbits_le32(&ch[chn].ao.test2_4, TEST2_4_TESTAGENTRK_MASK,
		rank << TEST2_4_TESTAGENTRK_SHIFT);
}

static void dramc_engine2_init(u8 chn, u8 rank, u32 size, bool testaudpat)
{
	const u32 pat0 = 0x55;
	const u32 pat1 = 0xaa;
	const u32 addr = 0;

	dramc_set_rank_engine2(chn, rank);

	clrbits_le32(&ch[chn].ao.dummy_rd,
		(0x1 << DUMMY_RD_DQSG_DMYRD_EN_SHIFT) |
		(0x1 << DUMMY_RD_DQSG_DMYWR_EN_SHIFT) |
		(0x1 << DUMMY_RD_DUMMY_RD_EN_SHIFT) |
		(0x1 << DUMMY_RD_SREF_DMYRD_EN_SHIFT) |
		(0x1 << DUMMY_RD_DMY_RD_DBG_SHIFT) |
		(0x1 << DUMMY_RD_DMY_WR_DBG_SHIFT));
	clrbits_le32(&ch[chn].nao.testchip_dma1,
		0x1 << TESTCHIP_DMA1_DMA_LP4MATAB_OPT_SHIFT);
	clrbits_le32(&ch[chn].ao.test2_3,
		(0x1 << TEST2_3_TEST2W_SHIFT) |
		(0x1 << TEST2_3_TEST2R_SHIFT) |
		(0x1 << TEST2_3_TEST1_SHIFT));
	clrsetbits_le32(&ch[chn].ao.test2_0,
		TEST2_0_PAT0_MASK | TEST2_0_PAT1_MASK,
		(pat0 << TEST2_0_PAT0_SHIFT) |
		(pat1 << TEST2_0_PAT1_SHIFT));
	write32(&ch[chn].ao.test2_1, (addr << 4) & 0x00ffffff);
	write32(&ch[chn].ao.test2_2, (size << 4) & 0x00ffffff);

	clrsetbits_le32(&ch[chn].ao.test2_4,
		(0x1 << TEST2_4_TESTAUDMODE_SHIFT) |
		(0x1 << TEST2_4_TESTAUDBITINV_SHIFT) |
		(0x1 << TEST2_4_TESTXTALKPAT_SHIFT),
		((!testaudpat ? 1 : 0) << TEST2_4_TESTXTALKPAT_SHIFT) |
		((testaudpat ? 1 : 0) << TEST2_4_TESTAUDMODE_SHIFT) |
		((testaudpat ? 1 : 0) << TEST2_4_TESTAUDBITINV_SHIFT));

	if (!testaudpat) {
		clrbits_le32(&ch[chn].ao.test2_4,
			(0x1 << TEST2_4_TEST_REQ_LEN1_SHIFT) |
			(0x1 << TEST2_4_TESTSSOPAT_SHIFT) |
			(0x1 << TEST2_4_TESTSSOXTALKPAT_SHIFT));
		setbits_le32(&ch[chn].ao.perfctl0,
			0x1 << PERFCTL0_RWOFOEN_SHIFT);
	} else {
		clrsetbits_le32(&ch[chn].ao.test2_4,
			TEST2_4_TESTAUDINIT_MASK | TEST2_4_TESTAUDINC_MASK,
			(0x11 << TEST2_4_TESTAUDINIT_SHIFT) |
			(0xd << TEST2_4_TESTAUDINC_SHIFT));
	}
	clrsetbits_le32(&ch[chn].ao.test2_3,
		TEST2_3_TESTCNT_MASK | (0x1 << TEST2_3_TESTAUDPAT_SHIFT),
		(testaudpat ? 1 : 0) << TEST2_3_TESTAUDPAT_SHIFT);
}

static void dramc_engine2_check_complete(u8 chn)
{
	u32 u4loop_count = 0;

	/* In some case test engine finished but the complete signal late come,
	 * system will wait very long time. Hence, we set a timeout here.
	 * After system receive complete signal or wait until time out
	 * it will return, the caller will check compare result to verify
	 * whether engine success.
	 */
	while ((read32(&ch[chn].nao.testrpt) & 0x1) == 0) {
		udelay(1);
		u4loop_count++;

		if (u4loop_count > MAX_CMP_CPT_WAIT_LOOP) {
			dramc_dbg("MEASURE_A timeout\n");
			break;
		}
	}
}

static u32 dramc_engine2_run(u8 chn, enum dram_te_op wr)
{
	u32 result;

	if (wr == TE_OP_READ_CHECK) {
		clrbits_le32(&ch[chn].ao.test2_4,
			0x1 << TEST2_4_TESTAUDMODE_SHIFT);
	} else if (wr == TE_OP_WRITE_READ_CHECK) {
		clrsetbits_le32(&ch[chn].ao.test2_3,
			(0x1 << TEST2_3_TEST2R_SHIFT) |
			(0x1 << TEST2_3_TEST1_SHIFT),
			0x1 << TEST2_3_TEST2W_SHIFT);

		dramc_engine2_check_complete(chn);
		clrbits_le32(&ch[chn].ao.test2_3,
			 (0x1 << TEST2_3_TEST2W_SHIFT) |
			 (0x1 << TEST2_3_TEST2R_SHIFT) |
			 (0x1 << TEST2_3_TEST1_SHIFT));
		udelay(1);
	}

	/* Do read test */
	clrsetbits_le32(&ch[chn].ao.test2_3,
		(0x1 << TEST2_3_TEST2W_SHIFT) | (0x1 << TEST2_3_TEST1_SHIFT),
		0x1 << TEST2_3_TEST2R_SHIFT);

	dramc_engine2_check_complete(chn);

	udelay(1);
	result = read32(&ch[chn].nao.cmp_err);
	clrbits_le32(&ch[chn].ao.test2_3,
		(0x1 << TEST2_3_TEST2W_SHIFT) |
		(0x1 << TEST2_3_TEST2R_SHIFT) |
		(0x1 << TEST2_3_TEST1_SHIFT));

	return result;
}

static void dramc_engine2_end(u8 chn)
{
	clrbits_le32(&ch[chn].ao.test2_4, 0x1 << 17);
}

static void find_gating_window(u32 result_r, u32 result_f, u32 *debug_cnt,
		u8 dly_coarse_large, u8 dly_coarse_0p5t, u8 *pass_begin,
		u8 *pass_count, u8 *dly_fine_xt, u32 *coarse_tune, u8 *dqs_high)
{
	u16 debug_cnt_perbyte;
	u8 pass_count_1[DQS_NUMBER];

	for (u8 dqs = 0; dqs < DQS_NUMBER; dqs++) {
		u8 dqs_result_r = (u8) ((result_r >> (8 * dqs)) & 0xff);
		u8 dqs_result_f = (u8) ((result_f >> (8 * dqs)) & 0xff);

		debug_cnt_perbyte = (u16) debug_cnt[dqs];
		if (dqs_result_r != 0 || dqs_result_f != 0 ||
		    debug_cnt_perbyte != GATING_GOLDEND_DQSCNT)
			continue;

		if (pass_begin[dqs] == 0) {
			pass_begin[dqs] = 1;
			pass_count_1[dqs] = 0;
			dramc_dbg("[Byte %d]First pass (%d, %d, %d)\n",
				  dqs, dly_coarse_large,
				  dly_coarse_0p5t, *dly_fine_xt);
		}

		if (pass_begin[dqs] == 1)
			pass_count_1[dqs]++;

		if (pass_begin[dqs] == 1 &&
		    pass_count_1[dqs] * DQS_GW_FINE_STEP > DQS_GW_FINE_END)
			dqs_high[dqs] = 0;

		if (pass_count_1[0] * DQS_GW_FINE_STEP > DQS_GW_FINE_END &&
		    pass_count_1[1] * DQS_GW_FINE_STEP > DQS_GW_FINE_END) {
			dramc_dbg("All bytes gating window > 1 coarse_tune,"
				  " Early break\n");
			*dly_fine_xt = DQS_GW_FINE_END;
			*coarse_tune = GATING_END;
		}
	}
}

static void find_dly_tune(u8 chn, u8 dly_coarse_large, u8 dly_coarse_0p5t,
		u8 dly_fine_xt, u8 *dqs_high, u8 *dly_coarse_large_cnt,
		u8 *dly_coarse_0p5t_cnt, u8 *dly_fine_tune_cnt, u8 *dqs_trans)
{
	for (size_t dqs = 0; dqs < DQS_NUMBER; dqs++) {
		u32 dqs_cnt = read32(&ch[chn].phy_nao.misc_phy_stben_b[dqs]);
		dqs_cnt = (dqs_cnt >> 16) & 3;

		if (dqs_cnt == 3)
			dqs_high[dqs]++;

		if (dqs_high[dqs] * DQS_GW_FINE_STEP <= 16)
			continue;

		switch (dqs_cnt) {
		case 3:
			dly_coarse_large_cnt[dqs] = dly_coarse_large;
			dly_coarse_0p5t_cnt[dqs] = dly_coarse_0p5t;
			dly_fine_tune_cnt[dqs] = dly_fine_xt;
			dqs_trans[dqs] = 1;
			break;
		case 2:
		case 1:
			dqs_trans[dqs]++;
			break;
		case 0:
			dqs_high[dqs] = 0;
			break;
		}
	}
}

static void dram_phy_reset(u8 chn)
{
	setbits_le32(&ch[chn].ao.ddrconf0, 1 << DDRCONF0_RDATRST_SHIFT);
	setbits_le32(&ch[chn].phy.misc_ctrl1, 1 << MISC_CTRL1_R_DMPHYRST_SHIFT);
	clrbits_le32(&ch[chn].phy.b[0].dq[9], (1 << 4) | (1 << 0));
	clrbits_le32(&ch[chn].phy.b[1].dq[9], (1 << 4) | (1 << 0));

	udelay(1);
	setbits_le32(&ch[chn].phy.b[1].dq[9], (1 << 4) | (1 << 0));
	setbits_le32(&ch[chn].phy.b[0].dq[9], (1 << 4) | (1 << 0));
	clrbits_le32(&ch[chn].phy.misc_ctrl1, 1 << MISC_CTRL1_R_DMPHYRST_SHIFT);
	clrbits_le32(&ch[chn].ao.ddrconf0, 1 << DDRCONF0_RDATRST_SHIFT);
}

static void dramc_set_gating_mode(u8 chn, bool mode)
{
	u8 vref = 0, burst = 0;

	if (mode) {
		vref = 2;
		burst = 1;
	}

	clrsetbits_le32(&ch[chn].ao.stbcal1, 0x1 << 5, burst << 5);
	setbits_le32(&ch[chn].ao.stbcal, 0x1 << 30);

	for (size_t b = 0; b < 2; b++) {
		clrsetbits_le32(&ch[chn].phy.b[b].dq[6], 0x3 << 14, vref << 14);
		setbits_le32(&ch[chn].phy.b[b].dq[9], 0x1 << 5);
		clrbits_le32(&ch[chn].phy.b[b].dq[9], (0x1 << 4) | (0x1 << 0));
		setbits_le32(&ch[chn].phy.b[b].dq[9], (0x1 << 4) | (0x1 << 0));
	}
}

static void dramc_rx_dqs_gating_cal_pre(u8 chn, u8 rank)
{
	rx_dqs_isi_pulse_cg_switch(chn, DISABLE);
	clrbits_le32(&ch[chn].ao.refctrl0, 1 << REFCTRL0_PBREFEN_SHIFT);

	dramc_hw_gating_onoff(chn, GATING_OFF);

	setbits_le32(&ch[chn].ao.stbcal1, 1 << STBCAL1_STBENCMPEN_SHIFT);
	setbits_le32(&ch[chn].ao.stbcal1, 1 << STBCAL1_STBCNT_LATCH_EN_SHIFT);
	clrbits_le32(&ch[chn].ao.ddrconf0, 1 << DDRCONF0_DM4TO1MODE_SHIFT);
	setbits_le32(&ch[chn].ao.spcmd, 1 << SPCMD_DQSGCNTEN_SHIFT);

	udelay(4);
	setbits_le32(&ch[chn].ao.spcmd, 1 << SPCMD_DQSGCNTRST_SHIFT);
	udelay(1);
	clrbits_le32(&ch[chn].ao.spcmd, 1 << SPCMD_DQSGCNTRST_SHIFT);
	clrsetbits_le32(&ch[chn].phy.misc_ctrl1,
		1 << MISC_CTRL1_R_DMSTBENCMP_RK_OPT_SHIFT,
		rank << MISC_CTRL1_R_DMSTBENCMP_RK_OPT_SHIFT);

}

static void dramc_write_dqs_gating_result(u8 chn, u8 rank,
		u8 *best_coarse_tune2t, u8 *best_coarse_tune0p5t,
		u8 *best_coarse_tune2t_p1, u8 *best_coarse_tune0p5t_p1,
		u8 *best_fine_tune)
{
	u8 best_coarse_rodt[DQS_NUMBER], best_coarse_0p5t_rodt[DQS_NUMBER];
	u8 best_coarse_rodt_p1[DQS_NUMBER];
	u8 best_coarse_0p5t_rodt_p1[DQS_NUMBER];

	rx_dqs_isi_pulse_cg_switch(chn, ENABLE);

	write32(&ch[chn].ao.shu[0].rk[rank].selph_dqsg0,
		((u32) best_coarse_tune2t[0] <<
		 SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_SHIFT) |
		((u32) best_coarse_tune2t[1] <<
		 SHURK_SELPH_DQSG0_TX_DLY_DQS1_GATED_SHIFT) |
		((u32) best_coarse_tune2t_p1[0] <<
		 SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1_SHIFT) |
		((u32) best_coarse_tune2t_p1[1] <<
		 SHURK_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1_SHIFT));
	write32(&ch[chn].ao.shu[0].rk[rank].selph_dqsg1,
		((u32) best_coarse_tune0p5t[0] <<
		 SHURK_SELPH_DQSG1_REG_DLY_DQS0_GATED_SHIFT) |
		((u32) best_coarse_tune0p5t[1] <<
		 SHURK_SELPH_DQSG1_REG_DLY_DQS1_GATED_SHIFT) |
		((u32) best_coarse_tune0p5t_p1[0] <<
		 SHURK_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1_SHIFT) |
		((u32) best_coarse_tune0p5t_p1[1] <<
		 SHURK_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1_SHIFT));

	for (size_t dqs = 0; dqs < DQS_NUMBER; dqs++) {
		u8 tmp_value = (best_coarse_tune2t[dqs] << 3)
			+ best_coarse_tune0p5t[dqs];

		if (tmp_value >= 11) {
			tmp_value -= 11;
			best_coarse_rodt[dqs] = tmp_value >> 3;
			best_coarse_0p5t_rodt[dqs] =
				tmp_value - (best_coarse_rodt[dqs] << 3);

			tmp_value = (best_coarse_tune2t_p1[dqs] << 3) +
				best_coarse_tune0p5t_p1[dqs] - 11;
			best_coarse_rodt_p1[dqs] = tmp_value >> 3;
			best_coarse_0p5t_rodt_p1[dqs] =
				tmp_value - (best_coarse_rodt_p1[dqs] << 3);

			dramc_dbg("Best RODT dly(2T, 0.5T) = (%d, %d)\n",
				  best_coarse_rodt[dqs],
				  best_coarse_0p5t_rodt[dqs]);
		} else {
			best_coarse_rodt[dqs] = 0;
			best_coarse_0p5t_rodt[dqs] = 0;
			best_coarse_rodt_p1[dqs] = 4;
			best_coarse_0p5t_rodt_p1[dqs] = 4;
			dramc_dbg("RxdqsGatingCal error: best_coarse_tune2t:%d"
				  " is already 0. RODT cannot be -1 coarse\n",
				  dqs);
		}
	}

	write32(&ch[chn].ao.shu[0].rk[rank].selph_odten0,
		((u32) best_coarse_rodt[0] <<
		 SHURK_SELPH_ODTEN0_TXDLY_B0_RODTEN_SHIFT) |
		((u32) best_coarse_rodt[1] <<
		 SHURK_SELPH_ODTEN0_TXDLY_B1_RODTEN_SHIFT) |
		((u32) best_coarse_rodt_p1[0] <<
		 SHURK_SELPH_ODTEN0_TXDLY_B0_RODTEN_P1_SHIFT) |
		((u32) best_coarse_rodt_p1[1] <<
		 SHURK_SELPH_ODTEN0_TXDLY_B1_RODTEN_P1_SHIFT));
	write32(&ch[chn].ao.shu[0].rk[rank].selph_odten1,
		((u32) best_coarse_0p5t_rodt[0] <<
		 SHURK_SELPH_ODTEN1_DLY_B0_RODTEN_SHIFT) |
		((u32) best_coarse_0p5t_rodt[1] <<
		 SHURK_SELPH_ODTEN1_DLY_B1_RODTEN_SHIFT) |
		((u32) best_coarse_0p5t_rodt_p1[0] <<
		 SHURK_SELPH_ODTEN1_DLY_B0_RODTEN_P1_SHIFT) |
		((u32) best_coarse_0p5t_rodt_p1[1] <<
		 SHURK_SELPH_ODTEN1_DLY_B1_RODTEN_P1_SHIFT));

	write32(&ch[chn].ao.shu[0].rk[rank].dqsien,
		best_fine_tune[0] | (best_fine_tune[1] << 8));
}

static void dramc_rx_dqs_gating_cal(u8 chn, u8 rank)
{
	u8 dqs;
	const u8 mr1_value = 0x56;
	u8 pass_begin[DQS_NUMBER] = {0}, pass_count[DQS_NUMBER] = {0};
	u8 min_coarse_tune2t[DQS_NUMBER], min_coarse_tune0p5t[DQS_NUMBER],
		min_fine_tune[DQS_NUMBER];
	u8 best_fine_tune[DQS_NUMBER], best_coarse_tune0p5t[DQS_NUMBER],
		best_coarse_tune2t[DQS_NUMBER];
	u8 best_coarse_tune0p5t_p1[DQS_NUMBER],
		best_coarse_tune2t_p1[DQS_NUMBER];
	u8 dqs_high[DQS_NUMBER] = {0}, dqs_transition[DQS_NUMBER] = {0};
	u8 dly_coarse_large_cnt[DQS_NUMBER] = {0},
		dly_coarse_0p5t_cnt[DQS_NUMBER] = {0},
		dly_fine_tune_cnt[DQS_NUMBER] = {0};
	u32 coarse_start = GATING_START, coarse_end = GATING_END;
	u32 debug_cnt[DQS_NUMBER];

	struct reg_value regs_bak[] = {
		{&ch[chn].ao.stbcal, 0x0},
		{&ch[chn].ao.stbcal1, 0x0},
		{&ch[chn].ao.ddrconf0, 0x0},
		{&ch[chn].ao.spcmd, 0x0},
		{&ch[chn].ao.refctrl0, 0x0},
		{&ch[chn].phy.b[0].dq[6], 0x0},
		{&ch[chn].phy.b[1].dq[6], 0x0},
	};
	for (size_t i = 0; i < ARRAY_SIZE(regs_bak); i++)
		regs_bak[i].value = read32(regs_bak[i].addr);

	dramc_mode_reg_write_by_rank(chn, rank, 0x1, mr1_value | 0x80);
	dramc_rx_dqs_gating_cal_pre(chn, rank);

	u32 dummy_rd_backup = read32(&ch[chn].ao.dummy_rd);
	dramc_engine2_init(chn, rank, 0x23, 1);

	dramc_dbg("[Gating]\n");
	for (u32 coarse_tune = coarse_start; coarse_tune < coarse_end;
	     coarse_tune += DQS_GW_COARSE_STEP) {
		u32 dly_coarse_large_rodt = 0, dly_coarse_0p5t_rodt = 0;
		u32 dly_coarse_large_rodt_p1 = 4, dly_coarse_0p5t_rodt_p1 = 4;
		u8 dly_coarse_large = coarse_tune / RX_DQS_CTL_LOOP;
		u8 dly_coarse_0p5t = coarse_tune % RX_DQS_CTL_LOOP;
		u32 dly_coarse_large_p1 =
			(coarse_tune + DQS_GW_FREQ_DIV) / RX_DQS_CTL_LOOP;
		u32 dly_coarse_0p5t_p1 =
			(coarse_tune + DQS_GW_FREQ_DIV) % RX_DQS_CTL_LOOP;
		u32 value = (dly_coarse_large << 3) + dly_coarse_0p5t;

		if (value >= 11) {
			value -= 11;
			dly_coarse_large_rodt = value >> 3;
			dly_coarse_0p5t_rodt =
				value - (dly_coarse_large_rodt << 3);

			value = (dly_coarse_large << 3) + dly_coarse_0p5t - 11;
			dly_coarse_large_rodt_p1 = value >> 3;
			dly_coarse_0p5t_rodt_p1 =
				value - (dly_coarse_large_rodt_p1 << 3);
		}

		write32(&ch[chn].ao.shu[0].rk[rank].selph_dqsg0,
			((u32) dly_coarse_large <<
			 SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_SHIFT) |
			((u32) dly_coarse_large <<
			 SHURK_SELPH_DQSG0_TX_DLY_DQS1_GATED_SHIFT) |
			(dly_coarse_large_p1 <<
			 SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1_SHIFT) |
			(dly_coarse_large_p1 <<
			 SHURK_SELPH_DQSG0_TX_DLY_DQS1_GATED_P1_SHIFT));
		write32(&ch[chn].ao.shu[0].rk[rank].selph_dqsg1,
			((u32) dly_coarse_0p5t <<
			 SHURK_SELPH_DQSG1_REG_DLY_DQS0_GATED_SHIFT) |
			((u32) dly_coarse_0p5t <<
			 SHURK_SELPH_DQSG1_REG_DLY_DQS1_GATED_SHIFT) |
			(dly_coarse_0p5t_p1 <<
			 SHURK_SELPH_DQSG1_REG_DLY_DQS0_GATED_P1_SHIFT) |
			(dly_coarse_0p5t_p1 <<
			 SHURK_SELPH_DQSG1_REG_DLY_DQS1_GATED_P1_SHIFT));
		write32(&ch[chn].ao.shu[0].rk[rank].selph_odten0,
			(dly_coarse_large_rodt <<
			 SHURK_SELPH_ODTEN0_TXDLY_B0_RODTEN_SHIFT) |
			(dly_coarse_large_rodt <<
			 SHURK_SELPH_ODTEN0_TXDLY_B1_RODTEN_SHIFT) |
			(dly_coarse_large_rodt_p1 <<
			 SHURK_SELPH_ODTEN0_TXDLY_B0_RODTEN_P1_SHIFT) |
			(dly_coarse_large_rodt_p1 <<
			 SHURK_SELPH_ODTEN0_TXDLY_B1_RODTEN_P1_SHIFT));
		write32(&ch[chn].ao.shu[0].rk[rank].selph_odten1,
			(dly_coarse_0p5t_rodt <<
			 SHURK_SELPH_ODTEN1_DLY_B0_RODTEN_SHIFT) |
			(dly_coarse_0p5t_rodt <<
			 SHURK_SELPH_ODTEN1_DLY_B1_RODTEN_SHIFT) |
			(dly_coarse_0p5t_rodt_p1 <<
			 SHURK_SELPH_ODTEN1_DLY_B0_RODTEN_P1_SHIFT) |
			(dly_coarse_0p5t_rodt_p1 <<
			 SHURK_SELPH_ODTEN1_DLY_B1_RODTEN_P1_SHIFT));

		for (u8 dly_fine_xt = DQS_GW_FINE_START;
			dly_fine_xt < DQS_GW_FINE_END;
			dly_fine_xt += DQS_GW_FINE_STEP) {

			dramc_set_gating_mode(chn, 0);

			write32(&ch[chn].ao.shu[0].rk[rank].dqsien,
				dly_fine_xt | (dly_fine_xt << 8));

			dram_phy_reset(chn);
			setbits_le32(&ch[chn].ao.spcmd,
				1 << SPCMD_DQSGCNTRST_SHIFT);
			udelay(1);
			clrbits_le32(&ch[chn].ao.spcmd,
				1 << SPCMD_DQSGCNTRST_SHIFT);

			dramc_engine2_run(chn, TE_OP_READ_CHECK);

			u32 result_r = read32(&ch[chn].phy.misc_stberr_rk0_r) &
				    MISC_STBERR_RK_R_STBERR_RK_R_MASK;
			u32 result_f = read32(&ch[chn].phy.misc_stberr_rk0_f) &
				    MISC_STBERR_RK_F_STBERR_RK_F_MASK;
			debug_cnt[0] = read32(&ch[chn].nao.dqsgnwcnt[0]);
			debug_cnt[1] = (debug_cnt[0] >> 16) & 0xffff;
			debug_cnt[0] &= 0xffff;

			dramc_set_gating_mode(chn, 1);
			dramc_engine2_run(chn, TE_OP_READ_CHECK);

			find_dly_tune(chn, dly_coarse_large, dly_coarse_0p5t,
				dly_fine_xt, dqs_high, dly_coarse_large_cnt,
				dly_coarse_0p5t_cnt,
				dly_fine_tune_cnt, dqs_transition);

			dramc_dbg("%d %d %d |", dly_coarse_large,
				  dly_coarse_0p5t, dly_fine_xt);
			for (dqs = 0; dqs < DQS_NUMBER; dqs++)
				dramc_dbg("%X ", debug_cnt[dqs]);

			dramc_dbg(" |");
			for (dqs = 0; dqs < DQS_NUMBER; dqs++) {
				dramc_dbg("(%X %X)",
				(result_f >> (DQS_BIT_NUMBER * dqs)) & 0xff,
				(result_r >> (DQS_BIT_NUMBER * dqs)) & 0xff);
			}

			dramc_dbg("\n");
			find_gating_window(result_r, result_f, debug_cnt,
				dly_coarse_large, dly_coarse_0p5t, pass_begin,
				pass_count, &dly_fine_xt, &coarse_tune,
				dqs_high);
		}
	}

	dramc_engine2_end(chn);
	write32(&ch[chn].ao.dummy_rd, dummy_rd_backup);

	for (dqs = 0; dqs < DQS_NUMBER; dqs++) {
		pass_count[dqs] = dqs_transition[dqs];
		min_fine_tune[dqs] = dly_fine_tune_cnt[dqs];
		min_coarse_tune0p5t[dqs] = dly_coarse_0p5t_cnt[dqs];
		min_coarse_tune2t[dqs] = dly_coarse_large_cnt[dqs];

		u8 tmp_offset = pass_count[dqs] * DQS_GW_FINE_STEP / 2;
		u8 tmp_value = min_fine_tune[dqs] + tmp_offset;
		best_fine_tune[dqs] = tmp_value % RX_DLY_DQSIENSTB_LOOP;

		tmp_offset = tmp_value / RX_DLY_DQSIENSTB_LOOP;
		tmp_value = min_coarse_tune0p5t[dqs] + tmp_offset;
		best_coarse_tune0p5t[dqs] = tmp_value % RX_DQS_CTL_LOOP;

		tmp_offset = tmp_value / RX_DQS_CTL_LOOP;
		best_coarse_tune2t[dqs] = min_coarse_tune2t[dqs] + tmp_offset;
		tmp_value = best_coarse_tune0p5t[dqs] + DQS_GW_FREQ_DIV;
		best_coarse_tune0p5t_p1[dqs] = tmp_value % RX_DQS_CTL_LOOP;

		tmp_offset = tmp_value / RX_DQS_CTL_LOOP;
		best_coarse_tune2t_p1[dqs] =
			best_coarse_tune2t[dqs] + tmp_offset;
	}

	for (dqs = 0; dqs < DQS_NUMBER; dqs++)
		dramc_show("Best DQS%d dly(2T, 0.5T, fine tune)"
			   " = (%d, %d, %d)\n", dqs, best_coarse_tune2t[dqs],
			   best_coarse_tune0p5t[dqs], best_fine_tune[dqs]);

	for (dqs = 0; dqs < DQS_NUMBER; dqs++)
		dramc_show("Best DQS%d coarse dly(2T, 0.5T, fine tune)"
			   " = (%d, %d, %d)\n", dqs, best_coarse_tune2t_p1[dqs],
			   best_coarse_tune0p5t_p1[dqs], best_fine_tune[dqs]);

	for (size_t i = 0; i < ARRAY_SIZE(regs_bak); i++)
		write32(regs_bak[i].addr, regs_bak[i].value);

	dramc_mode_reg_write_by_rank(chn, rank, 0x1, mr1_value & 0x7f);

	dramc_write_dqs_gating_result(chn, rank, best_coarse_tune2t,
		best_coarse_tune0p5t, best_coarse_tune2t_p1,
		best_coarse_tune0p5t_p1, best_fine_tune);

	dram_phy_reset(chn);
}

static void dramc_rx_dqs_gating_post_process(u8 chn)
{
	u8 dqs, rank_rx_dvs, dqsinctl;
	u32 read_dqsinctl, rankinctl_root, xrtr2r, reg_tx_dly_dqsgated_min = 3;
	u8 txdly_cal_min = 0xff, txdly_cal_max = 0, tx_dly_dqs_gated = 0;
	u32 best_coarse_tune2t[RANK_MAX][DQS_NUMBER];
	u32 best_coarse_tune2t_p1[RANK_MAX][DQS_NUMBER];

	rank_rx_dvs = reg_tx_dly_dqsgated_min - 1;

	for (size_t b = 0; b < 2; b++)
		clrsetbits_le32(&ch[chn].phy.shu[0].b[b].dq[7],
			SHU1_BX_DQ7_R_DMRANKRXDVS_MASK,
			rank_rx_dvs << SHU1_BX_DQ7_R_DMRANKRXDVS_SHIFT);

	for (size_t rank = 0; rank < RANK_MAX; rank++) {
		u32 dqsg0 = read32(&ch[chn].ao.shu[0].rk[rank].selph_dqsg0);
		for (dqs = 0; dqs < DQS_NUMBER; dqs++) {
			best_coarse_tune2t[rank][dqs] =
			   (dqsg0 >> (dqs * 8)) &
			    SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_MASK;
			best_coarse_tune2t_p1[rank][dqs] =
			    ((dqsg0 >> (dqs * 8)) &
			     SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1_MASK) >>
			    SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1_SHIFT;
			dramc_dbg("Rank%d best DQS%d dly(2T,(P1)2T)=(%d, %d)\n",
			      rank, dqs, best_coarse_tune2t[rank][dqs],
			      best_coarse_tune2t_p1[rank][dqs]);

			tx_dly_dqs_gated = best_coarse_tune2t[rank][dqs];
			txdly_cal_min = MIN(txdly_cal_min, tx_dly_dqs_gated);

			tx_dly_dqs_gated = best_coarse_tune2t_p1[rank][dqs];
			txdly_cal_max = MAX(txdly_cal_max, tx_dly_dqs_gated);
		}
	}

	dqsinctl = reg_tx_dly_dqsgated_min - txdly_cal_min;
	dramc_dbg("Dqsinctl:%d, tx_dly_dqsgated_min %d, txdly_cal_min %d\n",
		  dqsinctl, reg_tx_dly_dqsgated_min, txdly_cal_min);

	if (dqsinctl != 0) {
		txdly_cal_min += dqsinctl;
		txdly_cal_max += dqsinctl;

		for (size_t rank = 0; rank < RANK_MAX; rank++) {
			dramc_dbg("Rank: %d\n", rank);
			for (dqs = 0; dqs < DQS_NUMBER; dqs++) {
				best_coarse_tune2t[rank][dqs] += dqsinctl;
				best_coarse_tune2t_p1[rank][dqs] += dqsinctl;

				dramc_dbg("Best DQS%d dly(2T) = (%d)\n",
					  dqs, best_coarse_tune2t[rank][dqs]);
				dramc_dbg("Best DQS%d P1 dly(2T) = (%d)\n",
					  dqs,
					  best_coarse_tune2t_p1[rank][dqs]);
			}

			write32(&ch[chn].ao.shu[0].rk[rank].selph_dqsg0,
				(best_coarse_tune2t[rank][0] << 0) |
				(best_coarse_tune2t[rank][1] << 8) |
				(best_coarse_tune2t_p1[rank][0] << 4) |
				(best_coarse_tune2t_p1[rank][1] << 12));
		}
	}

	read_dqsinctl = (read32(&ch[chn].ao.shu[0].rk[0].dqsctl) &
				SHURK_DQSCTL_DQSINCTL_MASK) - dqsinctl;
	rankinctl_root = (read_dqsinctl >= 3) ? (read_dqsinctl - 3) : 0;

	clrsetbits_le32(&ch[chn].ao.shu[0].rk[0].dqsctl,
		SHURK_DQSCTL_DQSINCTL_MASK,
		read_dqsinctl << SHURK_DQSCTL_DQSINCTL_SHIFT);
	clrsetbits_le32(&ch[chn].ao.shu[0].rk[1].dqsctl,
		SHURK_DQSCTL_DQSINCTL_MASK,
		read_dqsinctl << SHURK_DQSCTL_DQSINCTL_SHIFT);
	clrsetbits_le32(&ch[chn].ao.shu[0].rankctl,
		SHU_RANKCTL_RANKINCTL_PHY_MASK |
		SHU_RANKCTL_RANKINCTL_MASK | SHU_RANKCTL_RANKINCTL_ROOT1_MASK,
		(read_dqsinctl << SHU_RANKCTL_RANKINCTL_PHY_SHIFT) |
		(rankinctl_root << SHU_RANKCTL_RANKINCTL_SHIFT) |
		(rankinctl_root << SHU_RANKCTL_RANKINCTL_ROOT1_SHIFT));

	xrtr2r = MIN(8 + txdly_cal_max + 1, 12);
	clrsetbits_le32(&ch[chn].ao.shu[0].actim_xrt,
		SHU_ACTIM_XRT_XRTR2R_MASK,
		xrtr2r << SHU_ACTIM_XRT_XRTR2R_SHIFT);

	dramc_dbg("Tx_dly_DQS gated check: min %d max %d, changeDQSINCTL=%d,"
		  " DQSINCTL=%d, RANKINCTL=%d, XRTR2R=%d\n",
		  txdly_cal_min, txdly_cal_max, dqsinctl,
		  read_dqsinctl, rankinctl_root, xrtr2r);
}

void dramc_calibrate_all_channels(const struct sdram_params *pams)
{
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		for (u8 rk = RANK_0; rk < RANK_MAX; rk++) {
			dramc_show("Start K ch:%d, rank:%d\n", chn, rk);
			auto_refresh_switch(chn, 0);
			cmd_bus_training(chn, rk, pams);
			dramc_write_leveling(chn, rk, pams->wr_level);
			auto_refresh_switch(chn, 1);
			dramc_rx_dqs_gating_cal(chn, rk);
		}

		dramc_rx_dqs_gating_post_process(chn);
	}
}
