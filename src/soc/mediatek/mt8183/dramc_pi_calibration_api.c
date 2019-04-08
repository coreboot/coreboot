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
#include <device/mmio.h>
#include <soc/emi.h>
#include <soc/dramc_register.h>
#include <soc/dramc_pi_api.h>
#include <timer.h>

enum {
	RX_VREF_BEGIN = 0,
	RX_VREF_END = 12,
	RX_VREF_STEP = 1,
	TX_VREF_BEGIN = 8,
	TX_VREF_END = 18,
	TX_VREF_STEP = 2,
};

enum {
	FIRST_DQ_DELAY = 0,
	FIRST_DQS_DELAY = -16,
	MAX_DQDLY_TAPS = 16,
	MAX_RX_DQDLY_TAPS = 63,
};

enum {
	GATING_START = 26,
	GATING_END = GATING_START + 24,
};

enum CAL_TYPE {
	RX_WIN_RD_DQC = 0,
	RX_WIN_TEST_ENG,
	TX_WIN_DQ_ONLY,
	TX_WIN_DQ_DQM,
};

enum RX_TYPE {
	RX_DQ = 0,
	RX_DQM,
	RX_DQS,
};

struct dqdqs_perbit_dly {
	struct perbit_dly {
		s16 first;
		s16 last;
		s16 best_first;
		s16 best_last;
		s16 best;
	} dqdly, dqsdly;
};

struct vref_perbit_dly {
	u8 vref;
	u16 max_win;
	u16 min_win;
	struct dqdqs_perbit_dly perbit_dly[DQ_DATA_WIDTH];
};

struct tx_dly_tune {
	u8 fine_tune;
	u8 coarse_tune_large;
	u8 coarse_tune_small;
	u8 coarse_tune_large_oen;
	u8 coarse_tune_small_oen;
};

struct per_byte_dly {
	u16 max_center;
	u16 min_center;
	u16 final_dly;
};

static void dramc_auto_refresh_switch(u8 chn, bool option)
{
	clrsetbits_le32(&ch[chn].ao.refctrl0, 1 << REFCTRL0_REFDIS_SHIFT,
		(option ? 0 : 1) << REFCTRL0_REFDIS_SHIFT);

	if (!option) {
		/*
		 * Because HW will actually disable autorefresh after
		 * refresh_queue empty, we need to wait until queue empty.
		 */
		udelay(((read32(&ch[chn].nao.misc_statusa) &
				MISC_STATUSA_REFRESH_QUEUE_CNT_MASK) >>
				MISC_STATUSA_REFRESH_QUEUE_CNT_SHIFT) * 4);
	}
}

static void dramc_cke_fix_onoff(u8 chn, bool fix_on, bool fix_off)
{
	clrsetbits_le32(&ch[chn].ao.ckectrl, (0x1 << 6) | (0x1 << 7),
			((fix_on ? 1 : 0) << 6) | ((fix_off ? 1 : 0) << 7));
}

static void dramc_mode_reg_write(u8 chn, u8 mr_idx, u8 value)
{
	u32 ckectrl_bak = read32(&ch[chn].ao.ckectrl);

	dramc_cke_fix_onoff(chn, true, false);
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

static void dramc_cmd_bus_training(u8 chn, u8 rank,
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

static void dramc_read_dbi_onoff(bool on)
{
	for (size_t chn = 0; chn < CHANNEL_MAX; chn++)
		for (size_t b = 0; b < 2; b++)
			clrsetbits_le32(&ch[chn].phy.shu[0].b[b].dq[7],
				0x1 << SHU1_BX_DQ7_R_DMDQMDBI_SHIFT,
				(on ? 1 : 0) << SHU1_BX_DQ7_R_DMDQMDBI_SHIFT);
}

static void dramc_write_dbi_onoff(bool on)
{
	for (size_t chn = 0; chn < CHANNEL_MAX; chn++)
		clrsetbits_le32(&ch[chn].ao.shu[0].wodt,
			0x1 << SHU1_WODT_DBIWR_SHIFT,
			(on ? 1 : 0) << SHU1_WODT_DBIWR_SHIFT);
}

static void dramc_phy_dcm_2_channel(u8 chn, bool en)
{
	clrsetbits_le32(&ch[chn].phy.misc_cg_ctrl0, (0x3 << 19) | (0x3ff << 8),
		((en ? 0 : 0x1) << 19) | ((en ? 0 : 0x1ff) << 9) | (1 << 8));

	for (size_t i = 0; i < DRAM_DFS_SHUFFLE_MAX; i++) {
		struct ddrphy_ao_shu *shu = &ch[chn].phy.shu[i];
		for (size_t b = 0; b < 2; b++)
			clrsetbits_le32(&shu->b[b].dq[8], 0x1fff << 19,
				((en ? 0 : 0x7ff) << 22) | (0x1 << 21) |
				((en ? 0 : 0x3) << 19));
		clrbits_le32(&shu->ca_cmd[8], 0x1fff << 19);
	}
	clrsetbits_le32(&ch[chn].phy.misc_cg_ctrl5, (0x7 << 16) | (0x7 << 20),
		((en ? 0x7 : 0) << 16) | ((en ? 0x7 : 0) << 20));
}

void dramc_enable_phy_dcm(bool en)
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
		write32(&ch[chn].phy.misc_cg_ctrl2,
			0x8060033e | (0x40 << (en ? 0x1 : 0)));
		write32(&ch[chn].phy.misc_cg_ctrl2,
			0x8060033f | (0x40 << (en ? 0x1 : 0)));
		write32(&ch[chn].phy.misc_cg_ctrl2,
			0x8060033e | (0x40 << (en ? 0x1 : 0)));

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

		dramc_phy_dcm_2_channel(chn, en);
	}
	dramc_set_broadcast(broadcast_bak);
}

static void dramc_reset_delay_chain_before_calibration(void)
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

void dramc_hw_gating_onoff(u8 chn, bool on)
{
	clrsetbits_le32(&ch[chn].ao.shuctrl2, 0x3 << 14,
		(on ? 0x3 : 0) << 14);
	clrsetbits_le32(&ch[chn].ao.stbcal2, 0x1 << 28, (on ? 0x1 : 0) << 28);
	clrsetbits_le32(&ch[chn].ao.stbcal, 0x1 << 24, (on ? 0x1 : 0) << 24);
	clrsetbits_le32(&ch[chn].ao.stbcal, 0x1 << 22, (on ? 0x1 : 0) << 22);
}

static void dramc_rx_input_delay_tracking_init_by_freq(u8 chn)
{
	struct ddrphy_ao_shu *shu = &ch[chn].phy.shu[0];

	clrsetbits_le32(&shu->b[0].dq[5], 0x7 << 20, 0x3 << 20);
	clrsetbits_le32(&shu->b[1].dq[5], 0x7 << 20, 0x3 << 20);
	clrbits_le32(&shu->b[0].dq[7], (0x1 << 12) | (0x1 << 13));
	clrbits_le32(&shu->b[1].dq[7], (0x1 << 12) | (0x1 << 13));
}

void dramc_apply_config_before_calibration(void)
{
	dramc_enable_phy_dcm(false);
	dramc_reset_delay_chain_before_calibration();

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

	dramc_write_dbi_onoff(false);
	dramc_read_dbi_onoff(false);

	for (size_t chn = 0; chn < CHANNEL_MAX; chn++) {
		setbits_le32(&ch[chn].ao.spcmdctrl, 0x1 << 29);
		setbits_le32(&ch[chn].ao.dqsoscr, 0x1 << 24);
		for (size_t shu = 0; shu < DRAM_DFS_SHUFFLE_MAX; shu++)
			setbits_le32(&ch[chn].ao.shu[shu].scintv, 0x1 << 30);

		clrbits_le32(&ch[chn].ao.dummy_rd, (0x1 << 7) | (0x7 << 20));
		dramc_hw_gating_onoff(chn, false);
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

static void dramc_set_mr13_vrcg_to_Normal(u8 chn)
{
	for (u8 rank = 0; rank < RANK_MAX; rank++)
		dramc_mode_reg_write_by_rank(chn, rank, 13, 0xd0);

	for (u8 shu = 0; shu < DRAM_DFS_SHUFFLE_MAX; shu++)
		clrbits_le32(&ch[chn].ao.shu[shu].hwset_vrcg, 0x1 << 19);
}

void dramc_apply_config_after_calibration(void)
{
	for (size_t chn = 0; chn < CHANNEL_MAX; chn++) {
		setbits_le32(&ch[chn].phy.misc_cg_ctrl4, 0x11400000);
		clrbits_le32(&ch[chn].ao.refctrl1, 0x1 << 7);
		clrbits_le32(&ch[chn].ao.shuctrl, 0x1 << 2);
		clrbits_le32(&ch[chn].phy.ca_cmd[6], 0x1 << 6);
		dramc_set_mr13_vrcg_to_Normal(chn);

		clrbits_le32(&ch[chn].phy.b[0].dq[6], 0x3);
		clrbits_le32(&ch[chn].phy.b[1].dq[6], 0x3);
		clrbits_le32(&ch[chn].phy.ca_cmd[6], 0x3);
		setbits_le32(&ch[chn].phy.b[0].dq[6], 0x1 << 5);
		setbits_le32(&ch[chn].phy.b[1].dq[6], 0x1 << 5);
		setbits_le32(&ch[chn].phy.ca_cmd[6], 0x1 << 5);

		clrbits_le32(&ch[chn].ao.impcal, 0x3 << 24);
		clrbits_le32(&ch[chn].phy.misc_imp_ctrl0, 0x7);

		clrbits_le32(&ch[chn].phy.misc_ctrl0, 0x1 << 31);
		clrbits_le32(&ch[chn].phy.misc_ctrl1, 0x1 << 25);

		setbits_le32(&ch[chn].ao.spcmdctrl, 1 << 29);
		setbits_le32(&ch[chn].ao.dqsoscr, 1 << 24);

		for (u8 shu = 0; shu < DRAM_DFS_SHUFFLE_MAX; shu++)
			clrbits_le32(&ch[chn].ao.shu[shu].scintv, 0x1 << 30);

		clrbits_le32(&ch[chn].ao.dummy_rd, (0x7 << 20) | (0x1 << 7));
		dramc_cke_fix_onoff(chn, false, false);
		clrbits_le32(&ch[chn].ao.dramc_pd_ctrl, 0x1 << 26);

		clrbits_le32(&ch[chn].ao.eyescan, 0x7 << 8);
		clrsetbits_le32(&ch[chn].ao.test2_4, 0x7 << 28, 0x4 << 28);
	}
}

static void dramc_rx_dqs_isi_pulse_cg_switch(u8 chn, bool flag)
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

static void dramc_engine2_init(u8 chn, u8 rank, u32 size, bool test_pat)
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
		((!test_pat ? 1 : 0) << TEST2_4_TESTXTALKPAT_SHIFT) |
		((test_pat ? 1 : 0) << TEST2_4_TESTAUDMODE_SHIFT) |
		((test_pat ? 1 : 0) << TEST2_4_TESTAUDBITINV_SHIFT));

	if (!test_pat) {
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
		(test_pat ? 1 : 0) << TEST2_3_TESTAUDPAT_SHIFT);
}

static void dramc_engine2_check_complete(u8 chn)
{
	/* In some case test engine finished but the complete signal late come,
	 * system will wait very long time. Hence, we set a timeout here.
	 * After system receive complete signal or wait until time out
	 * it will return, the caller will check compare result to verify
	 * whether engine success.
	 */
	if (!wait_us(10000, read32(&ch[chn].nao.testrpt) & 0x1))
		dramc_dbg("MEASURE_A timeout\n");
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

static void dramc_find_gating_window(u32 result_r, u32 result_f, u32 *debug_cnt,
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

static void dramc_find_dly_tune(u8 chn, u8 dly_coarse_large, u8 dly_coarse_0p5t,
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
	dramc_rx_dqs_isi_pulse_cg_switch(chn, false);
	clrbits_le32(&ch[chn].ao.refctrl0, 1 << REFCTRL0_PBREFEN_SHIFT);

	dramc_hw_gating_onoff(chn, false);

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

	dramc_rx_dqs_isi_pulse_cg_switch(chn, true);

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
			dramc_dbg("RxdqsGatingCal error: best_coarse_tune2t:%zd"
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
	dramc_engine2_init(chn, rank, 0x23, true);

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

			dramc_find_dly_tune(chn, dly_coarse_large,
				dly_coarse_0p5t, dly_fine_xt, dqs_high,
				dly_coarse_large_cnt, dly_coarse_0p5t_cnt,
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
			dramc_find_gating_window(result_r, result_f, debug_cnt,
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

static void dramc_rd_dqc_init(u8 chn, u8 rank)
{
	const u8 *lpddr_phy_mapping = phy_mapping[chn];
	u16 temp_value = 0;

	for (size_t b = 0; b < 2; b++)
		clrbits_le32(&ch[chn].phy.shu[0].b[b].dq[7],
		     0x1 << SHU1_BX_DQ7_R_DMDQMDBI_SHIFT);

	clrsetbits_le32(&ch[chn].ao.mrs,
		MRS_MRSRK_MASK, rank << MRS_MRSRK_SHIFT);
	setbits_le32(&ch[chn].ao.mpc_option,
		0x1 << MPC_OPTION_MPCRKEN_SHIFT);

	for (size_t i = 0; i < 16; i++)
		temp_value |= ((0x5555 >> i) & 0x1) << lpddr_phy_mapping[i];

	u16 mr15_golden_value = temp_value & 0xff;
	u16 mr20_golden_value = (temp_value >> 8) & 0xff;
	clrsetbits_le32(&ch[chn].ao.mr_golden,
		MR_GOLDEN_MR15_GOLDEN_MASK | MR_GOLDEN_MR20_GOLDEN_MASK,
		(mr15_golden_value << 8) | mr20_golden_value);
}

static u32 dramc_rd_dqc_run(u8 chn)
{
	setbits_le32(&ch[chn].ao.spcmdctrl, 1 << SPCMDCTRL_RDDQCDIS_SHIFT);
	setbits_le32(&ch[chn].ao.spcmd, 1 << SPCMD_RDDQCEN_SHIFT);

	if (!wait_us(100, read32(&ch[chn].nao.spcmdresp) &
			 (0x1 << SPCMDRESP_RDDQC_RESPONSE_SHIFT)))
		dramc_dbg("[RDDQC] resp fail (time out)\n");

	u32 result = read32(&ch[chn].nao.rdqc_cmp);
	clrbits_le32(&ch[chn].ao.spcmd, 1 << SPCMD_RDDQCEN_SHIFT);
	clrbits_le32(&ch[chn].ao.spcmdctrl, 1 << SPCMDCTRL_RDDQCDIS_SHIFT);

	return result;
}

static void dramc_rd_dqc_end(u8 chn)
{
	clrbits_le32(&ch[chn].ao.mrs, MRS_MRSRK_MASK);
}

static void dramc_rx_vref_enable(u8 chn)
{
	setbits_le32(&ch[chn].phy.b[0].dq[5],
		0x1 << B0_DQ5_RG_RX_ARDQ_VREF_EN_B0_SHIFT);
	setbits_le32(&ch[chn].phy.b[1].dq[5],
		0x1 << B1_DQ5_RG_RX_ARDQ_VREF_EN_B1_SHIFT);
}

static void dramc_set_rx_vref(u8 chn, u8 value)
{
	for (size_t b = 0; b < 2; b++)
		clrsetbits_le32(&ch[chn].phy.shu[0].b[b].dq[5],
			SHU1_BX_DQ5_RG_RX_ARDQ_VREF_SEL_B0_MASK,
			value << SHU1_BX_DQ5_RG_RX_ARDQ_VREF_SEL_B0_SHIFT);
}

static void dramc_set_tx_vref(u8 chn, u8 rank, u8 value)
{
	dramc_mode_reg_write_by_rank(chn, rank, 14, value);
}

static void dramc_set_vref(u8 chn, u8 rank, enum CAL_TYPE type, u8 vref)
{
	if (type == RX_WIN_TEST_ENG)
		dramc_set_rx_vref(chn, vref);
	else
		dramc_set_tx_vref(chn, rank, vref);
}

static void dramc_transfer_dly_tune(
		u8 chn, u32 dly, struct tx_dly_tune *dly_tune)
{
	u16 tmp_val;

	dly_tune->fine_tune = dly & (TX_DQ_COARSE_TUNE_TO_FINE_TUNE_TAP - 1);

	tmp_val = (dly / TX_DQ_COARSE_TUNE_TO_FINE_TUNE_TAP) << 1;
	dly_tune->coarse_tune_small = tmp_val - ((tmp_val >> 3) << 3);
	dly_tune->coarse_tune_large = tmp_val >> 3;

	tmp_val -= 4;
	dly_tune->coarse_tune_small_oen = tmp_val - ((tmp_val >> 3) << 3);
	dly_tune->coarse_tune_large_oen = tmp_val >> 3;
}

static void dramc_set_rx_dly_factor(u8 chn, u8 rank, enum RX_TYPE type, u32 val)
{
	u32 tmp, mask;

	switch (type) {
	case RX_DQ:
		tmp = (val << 24 | val << 16 | val << 8 | val);
		for (size_t i = 2; i < 6; i++) {
			write32(&ch[chn].phy.shu[0].rk[rank].b[0].dq[i], tmp);
			write32(&ch[chn].phy.shu[0].rk[rank].b[1].dq[i], tmp);
		}
		break;

	case RX_DQM:
		tmp = (val << 8 | val);
		mask = SHU1_B0_DQ6_RK_RX_ARDQM0_F_DLY_B0_MASK |
		    SHU1_B0_DQ6_RK_RX_ARDQM0_R_DLY_B0_MASK;
		clrsetbits_le32(&ch[chn].phy.shu[0].rk[rank].b[0].dq[6],
			mask, tmp);
		clrsetbits_le32(&ch[chn].phy.shu[0].rk[rank].b[1].dq[6],
			mask, tmp);
		break;

	case RX_DQS:
		tmp = (val << 24 | val << 16);
		mask = SHU1_B0_DQ6_RK_RX_ARDQS0_F_DLY_B0_MASK |
			SHU1_B0_DQ6_RK_RX_ARDQS0_R_DLY_B0_MASK;
		clrsetbits_le32(&ch[chn].phy.shu[0].rk[rank].b[0].dq[6],
			mask, tmp);
		clrsetbits_le32(&ch[chn].phy.shu[0].rk[rank].b[1].dq[6],
			mask, tmp);
		break;
	}
}

static void dramc_set_tx_dly_factor(u8 chn, u8 rank,
		enum CAL_TYPE type, u32 val)
{
	struct tx_dly_tune dly_tune = {0};
	u32 coarse_tune_large = 0, coarse_tune_large_oen = 0;
	u32 coarse_tune_small = 0, coarse_tune_small_oen = 0;

	dramc_transfer_dly_tune(chn, val, &dly_tune);

	for (u8 i = 0; i < 4; i++) {
		coarse_tune_large += dly_tune.coarse_tune_large << (i * 4);
		coarse_tune_large_oen +=
			dly_tune.coarse_tune_large_oen << (i * 4);
		coarse_tune_small += dly_tune.coarse_tune_small << (i * 4);
		coarse_tune_small_oen +=
			dly_tune.coarse_tune_small_oen << (i * 4);
	}
	if (type == TX_WIN_DQ_DQM)
		dramc_dbg("%3d |%d  %d  %2d | [0]",
			  val, dly_tune.coarse_tune_large,
			  dly_tune.coarse_tune_small, dly_tune.fine_tune);

	if (type != TX_WIN_DQ_DQM && type != TX_WIN_DQ_ONLY)
		return;

	write32(&ch[chn].ao.shu[0].rk[rank].selph_dq[0],
		(coarse_tune_large_oen << 16) | coarse_tune_large);
	write32(&ch[chn].ao.shu[0].rk[rank].selph_dq[2],
		(coarse_tune_small_oen << 16) | coarse_tune_small);
	for (size_t b = 0; b < 2; b++)
		clrsetbits_le32(&ch[chn].phy.shu[0].rk[rank].b[b].dq[7],
			FINE_TUNE_DQ_MASK, dly_tune.fine_tune << 8);

	if (type == TX_WIN_DQ_DQM) {
		/* Large coarse_tune setting */
		write32(&ch[chn].ao.shu[0].rk[rank].selph_dq[1],
			(coarse_tune_large_oen << 16) | coarse_tune_large);
		write32(&ch[chn].ao.shu[0].rk[rank].selph_dq[3],
			(coarse_tune_small_oen << 16) | coarse_tune_small);
		/* Fine_tune delay setting */
		for (size_t b = 0; b < 2; b++)
			clrsetbits_le32(&ch[chn].phy.shu[0].rk[rank].b[b].dq[7],
				FINE_TUNE_DQM_MASK, dly_tune.fine_tune << 16);
	}
}

static u32 dramc_get_smallest_dqs_dly(
		u8 chn, u8 rank, const struct sdram_params *params)
{
	u32 min_dly = 0xffff;

	for (size_t i = 0; i < DQS_NUMBER; i++)
		min_dly = MIN(min_dly, params->wr_level[chn][rank][i]);

	return DQS_DELAY + min_dly + 40;
}

static void dramc_get_dly_range(u8 chn, u8 rank, enum CAL_TYPE type,
		u16 *pre_cal, s16 *begin, s16 *end,
		const struct sdram_params *params)
{
	u16 pre_dq_dly;
	switch (type) {
	case RX_WIN_RD_DQC:
		*begin = FIRST_DQS_DELAY;
		*end = MAX_RX_DQDLY_TAPS;
		break;

	case RX_WIN_TEST_ENG:
		*begin = FIRST_DQ_DELAY;
		*end = MAX_RX_DQDLY_TAPS;
		break;

	case TX_WIN_DQ_DQM:
		*begin = dramc_get_smallest_dqs_dly(chn, rank, params);
		*end = *begin + 256;
		break;

	case TX_WIN_DQ_ONLY:
		pre_dq_dly = MIN(pre_cal[0], pre_cal[1]);
		pre_dq_dly = (pre_dq_dly > 24) ? (pre_dq_dly - 24) : 0;
		*begin = pre_dq_dly;
		*end = *begin + 64;
		break;
	}
}
static int dramc_check_dqdqs_win(
		struct dqdqs_perbit_dly *p, s16 dly_pass, s16 last_step,
		bool fail, bool is_dq)
{
	s16 best_pass_win;
	struct perbit_dly *dly = is_dq ? &p->dqdly : &p->dqsdly;

	if (!fail && dly->first == -1)
		dly->first = dly_pass;

	if (!fail && dly->last == -2 && dly_pass == last_step)
		dly->last = dly_pass;
	else if (fail && dly->first != -1 && dly->last == -2)
		dly->last = dly_pass - 1;

	if (dly->last == -2)
		return 0;

	int pass_win = dly->last - dly->first;
	best_pass_win = dly->best_last - dly->best_first;
	if (pass_win > best_pass_win) {
		dly->best_last = dly->last;
		dly->best_first = dly->first;
	}
	/* Clear to find the next pass range if it has */
	dly->first = -1;
	dly->last = -2;

	return pass_win;
}

static void dramc_set_vref_dly(struct vref_perbit_dly *vref_dly,
		u8 vref, u32 win_size_sum, struct dqdqs_perbit_dly delay[])
{
	struct dqdqs_perbit_dly *perbit_dly = vref_dly->perbit_dly;

	vref_dly->max_win = win_size_sum;
	vref_dly->vref = vref;
	for (size_t bit = 0; bit < DQ_DATA_WIDTH; bit++) {
		perbit_dly[bit].dqdly.best = delay[bit].dqdly.best;
		perbit_dly[bit].dqdly.best_first = delay[bit].dqdly.best_first;
		perbit_dly[bit].dqdly.best_last = delay[bit].dqdly.best_last;
		perbit_dly[bit].dqsdly.best_first =
			delay[bit].dqsdly.best_first;
		perbit_dly[bit].dqsdly.best_last = delay[bit].dqsdly.best_last;
	}
}

static bool dramk_calc_best_vref(enum CAL_TYPE type, u8 vref,
		struct vref_perbit_dly *vref_dly,
		struct dqdqs_perbit_dly delay[])
{
	u32 win_size;
	u32 win_size_sum = 0;
	static u32 min_win_size_vref;

	switch (type) {
	case RX_WIN_TEST_ENG:
		for (size_t bit = 0; bit < DQ_DATA_WIDTH; bit++) {
			win_size_sum += delay[bit].dqdly.best_last -
					 delay[bit].dqdly.best_first + 1;
			win_size_sum += delay[bit].dqsdly.best_last -
					 delay[bit].dqsdly.best_first + 1;
		}

		if (win_size_sum > vref_dly->max_win)
			dramc_set_vref_dly(vref_dly, vref, win_size_sum, delay);

		if (win_size_sum < (vref_dly->max_win * 95 / 100))
			return true;

		break;
	case TX_DQ_DQS_MOVE_DQ_ONLY:
		for (size_t bit = 0; bit < DQ_DATA_WIDTH; bit++) {
			win_size = delay[bit].dqdly.best_last -
					delay[bit].dqdly.best_first + 1;
			vref_dly->min_win = MIN(vref_dly->min_win, win_size);
			win_size_sum += win_size;
		}

		if (win_size_sum > vref_dly->max_win
		    && vref_dly->min_win >= min_win_size_vref) {
			min_win_size_vref = vref_dly->min_win;
			dramc_set_vref_dly(vref_dly, vref, win_size_sum, delay);
		}

		break;
	default:
		dramc_set_vref_dly(vref_dly, vref, win_size_sum, delay);
		break;
	}

	return false;
}

static void dramc_calc_tx_perbyte_dly(
		struct dqdqs_perbit_dly *p, s16 *win,
		struct per_byte_dly *byte_delay_prop)
{
	s16 win_center = (p->dqdly.best_first + p->dqdly.best_last) >> 1;
	*win = win_center;

	if (win_center < byte_delay_prop->min_center)
		byte_delay_prop->min_center = win_center;
	if (win_center > byte_delay_prop->max_center)
		byte_delay_prop->max_center = win_center;
}

static void dramc_set_rx_dly(u8 chn, u8 rank, s32 dly)
{
	if (dly <= 0) {
		/* Hold time calibration */
		dramc_set_rx_dly_factor(chn, rank, RX_DQS, -dly);
		dram_phy_reset(chn);
	} else {
		/* Setup time calibration */
		dramc_set_rx_dly_factor(chn, rank, RX_DQS, 0);
		dramc_set_rx_dly_factor(chn, rank, RX_DQM, dly);
		dram_phy_reset(chn);
		dramc_set_rx_dly_factor(chn, rank, RX_DQ, dly);
	}
}

static void dramc_set_tx_best_dly_factor(u8 chn, u8 rank_start,
		struct per_byte_dly *tx_perbyte_dly, u16 dq_precal_result[])
{
	u32 coarse_tune_large = 0;
	u32 coarse_tune_large_oen = 0;
	u32 coarse_tune_small = 0;
	u32 coarse_tune_small_oen = 0;
	u16 dq_oen[DQS_NUMBER] = {0}, dqm_oen[DQS_NUMBER] = {0};
	struct tx_dly_tune dqdly_tune[DQS_NUMBER] = {0};
	struct tx_dly_tune dqmdly_tune[DQS_NUMBER] = {0};

	for (size_t i = 0; i < DQS_NUMBER; i++) {
		dramc_transfer_dly_tune(chn, tx_perbyte_dly[i].final_dly,
			&dqdly_tune[i]);
		dramc_transfer_dly_tune(chn, dq_precal_result[i],
			&dqmdly_tune[i]);

		coarse_tune_large += dqdly_tune[i].coarse_tune_large << (i * 4);
		coarse_tune_large_oen +=
			dqdly_tune[i].coarse_tune_large_oen << (i * 4);
		coarse_tune_small += dqdly_tune[i].coarse_tune_small << (i * 4);
		coarse_tune_small_oen +=
			dqdly_tune[i].coarse_tune_small_oen << (i * 4);

		dq_oen[i] = (dqdly_tune[i].coarse_tune_large_oen << 3) +
				(dqdly_tune[i].coarse_tune_small_oen << 5) +
				dqdly_tune[i].fine_tune;
		dqm_oen[i] = (dqmdly_tune[i].coarse_tune_large_oen << 3) +
				(dqmdly_tune[i].coarse_tune_small_oen << 5) +
				dqmdly_tune[i].fine_tune;
	}

	for (size_t rank = rank_start; rank < RANK_MAX; rank++) {
		write32(&ch[chn].ao.shu[0].rk[rank].selph_dq[0],
			(coarse_tune_large_oen << 16) | coarse_tune_large);
		write32(&ch[chn].ao.shu[0].rk[rank].selph_dq[2],
			(coarse_tune_small_oen << 16) | coarse_tune_small);
		write32(&ch[chn].ao.shu[0].rk[rank].selph_dq[1],
			(coarse_tune_large_oen << 16) | coarse_tune_large);
		write32(&ch[chn].ao.shu[0].rk[rank].selph_dq[3],
			(coarse_tune_small_oen << 16) | coarse_tune_small);
	}

	for (size_t rank = rank_start; rank < RANK_MAX; rank++)
		for (size_t b = 0; b < 2; b++)
			clrsetbits_le32(&ch[chn].phy.shu[0].rk[rank].b[b].dq[7],
				FINE_TUNE_DQ_MASK | FINE_TUNE_DQM_MASK,
				(dqdly_tune[b].fine_tune <<
				FINE_TUNE_DQ_SHIFT) |
				(dqmdly_tune[b].fine_tune <<
				FINE_TUNE_DQM_SHIFT));
}

static void dramc_set_rx_best_dly_factor(u8 chn, u8 rank,
		struct dqdqs_perbit_dly *dqdqs_perbit_dly,
		u32 *max_dqsdly_byte, u32 *ave_dqm_dly)
{
	u32 value;

	for (size_t i = 0; i < DQS_NUMBER; i++) {
		value = (max_dqsdly_byte[i] << 24) |
			(max_dqsdly_byte[i] << 16) |
			(ave_dqm_dly[i] << 8) | (ave_dqm_dly[i] << 0);
		write32(&ch[chn].phy.shu[0].rk[rank].b[i].dq[6], value);
	}
	dram_phy_reset(chn);

	for (size_t i = 0; i < DQ_DATA_WIDTH; i += 2) {
		u32 byte = i / DQS_BIT_NUMBER;
		u32 index = 2 + ((i % 8) * 2) / 4;
		value = dqdqs_perbit_dly[i + 1].dqdly.best << 24 |
			dqdqs_perbit_dly[i + 1].dqdly.best << 16 |
			dqdqs_perbit_dly[i].dqdly.best << 8 |
			dqdqs_perbit_dly[i].dqdly.best;
		write32(&ch[chn].phy.shu[0].rk[rank].b[byte].dq[index], value);
	}
}

static bool dramc_calc_best_dly(u8 bit,
		struct dqdqs_perbit_dly *p, u32 *p_max_byte)
{
	u8 fail = 0, hold, setup;

	hold = p->dqsdly.best_last - p->dqsdly.best_first + 1;
	setup = p->dqdly.best_last - p->dqdly.best_first + 1;

	if (hold > setup) {
		p->dqdly.best = 0;
		p->dqsdly.best = (setup != 0) ? (hold - setup) / 2 :
		    (hold - setup) / 2 + p->dqsdly.best_first;

		if (p->dqsdly.best > *p_max_byte)
			*p_max_byte = p->dqsdly.best;

	} else if (hold < setup) {
		p->dqsdly.best = 0;
		p->dqdly.best = (hold != 0) ? (setup - hold) / 2 :
		    (setup - hold) / 2 + p->dqdly.best_first;

	} else {		/* Hold time == setup time */
		p->dqsdly.best = 0;
		p->dqdly.best = 0;

		if (hold == 0) {
			dramc_dbg("Error bit %d, setup = hold = 0\n", bit);
			fail = 1;
		}
	}

	dramc_dbg("bit#%d : dq =%d dqs=%d win=%d (%d, %d)\n", bit, setup,
		  hold, setup + hold, p->dqdly.best, p->dqsdly.best);

	return fail;
}

static void dramc_set_dqdqs_dly(u8 chn, u8 rank, enum CAL_TYPE type, s32 dly)
{
	if ((type == RX_WIN_RD_DQC) || (type == RX_WIN_TEST_ENG))
		dramc_set_rx_dly(chn, rank, dly);
	else
		dramc_set_tx_dly_factor(chn, rank, type, dly);
}

static void dramc_set_tx_best_dly(u8 chn, u8 rank,
		struct dqdqs_perbit_dly *tx_dly, u16 *tx_dq_precal_result,
		const struct sdram_params *params)
{
	s16 dq_win_center[DQ_DATA_WIDTH];
	u16 pi_diff;
	u32 byte_dly_cell[DQS_NUMBER] = {0};
	struct per_byte_dly tx_perbyte_dly[DQS_NUMBER];
	u16 dly_cell_unit = params->delay_cell_unit;
	int index, bit;
	u16 dq_delay_cell[DQ_DATA_WIDTH];

	for (size_t i = 0; i < DQS_NUMBER; i++) {
		tx_perbyte_dly[i].min_center = 0xffff;
		tx_perbyte_dly[i].max_center = 0;
	}

	for (size_t i = 0; i < DQ_DATA_WIDTH; i++) {
		index = i / DQS_BIT_NUMBER;
		dramc_calc_tx_perbyte_dly(&tx_dly[i],
			&dq_win_center[i], &tx_perbyte_dly[index]);
	}

	for (size_t i = 0; i < DQS_NUMBER; i++) {
		tx_perbyte_dly[i].final_dly = tx_perbyte_dly[i].min_center;
		tx_dq_precal_result[i] = (tx_perbyte_dly[i].max_center
					+ tx_perbyte_dly[i].min_center) >> 1;

		for (bit = 0; bit < DQS_BIT_NUMBER; bit++) {
			pi_diff = dq_win_center[i * 8 + bit]
				- tx_perbyte_dly[i].min_center;
			dq_delay_cell[i * 8 + bit] =
				((pi_diff * 1000000) / (16 * 64))
				/ dly_cell_unit;
			byte_dly_cell[i] |=
				(dq_delay_cell[i * 8 + bit] << (bit * 4));
		}

		write32(&ch[chn].phy.shu[0].rk[rank].b[i].dq[0],
			byte_dly_cell[i]);
	}

	dramc_set_tx_best_dly_factor(chn, rank, tx_perbyte_dly,
		tx_dq_precal_result);
}

static int dramc_set_rx_best_dly(u8 chn, u8 rank,
		struct dqdqs_perbit_dly *rx_dly)
{
	s16 dly;
	bool fail = false;
	u8 index, max_limit;
	static u32 max_dqsdly_byte[DQS_NUMBER];
	static u32 ave_dqmdly_byte[DQS_NUMBER];

	for (size_t i = 0; i < DQS_NUMBER; i++) {
		max_dqsdly_byte[i] = 0;
		ave_dqmdly_byte[i] = 0;
	}

	for (size_t i = 0; i < DQ_DATA_WIDTH; i++) {
		index = i / DQS_BIT_NUMBER;
		fail |= dramc_calc_best_dly(i, &rx_dly[i],
			&max_dqsdly_byte[index]);
	}

	for (size_t i = 0; i < DQ_DATA_WIDTH; i++) {
		index = i / DQS_BIT_NUMBER;
		/* Set DQS to max for 8-bit */
		if (rx_dly[i].dqsdly.best < max_dqsdly_byte[index]) {
			/* Delay DQ to compensate extra DQS delay */
			dly = max_dqsdly_byte[index] - rx_dly[i].dqsdly.best;
			rx_dly[i].dqdly.best += dly;
			max_limit = MAX_DQDLY_TAPS - 1;
			if (rx_dly[i].dqdly.best > max_limit)
				rx_dly[i].dqdly.best = max_limit;
		}

		ave_dqmdly_byte[index] += rx_dly[i].dqdly.best;
		if ((i + 1) % DQS_BIT_NUMBER == 0)
			ave_dqmdly_byte[index] /= DQS_BIT_NUMBER;
	}

	if (fail) {
		dramc_dbg("Fail on perbit_window_cal()\n");
		return -1;
	}

	dramc_set_rx_best_dly_factor(chn, rank, rx_dly, max_dqsdly_byte,
			       ave_dqmdly_byte);
	return 0;
}

static void dramc_get_vref_prop(u8 rank, enum CAL_TYPE type,
		u8 *vref_scan_en, u8 *vref_begin, u8 *vref_end)
{
	if (type == RX_WIN_TEST_ENG && rank == RANK_0) {
		*vref_scan_en = 1;
		*vref_begin = RX_VREF_BEGIN;
		*vref_end  = RX_VREF_END;
	} else if (type == TX_WIN_DQ_ONLY) {
		*vref_scan_en = 1;
		*vref_begin = TX_VREF_BEGIN;
		*vref_end = TX_VREF_END;
	} else {
		*vref_scan_en = 0;
	}
}

static void dramc_engine2_setpat(u8 chn, bool test_pat)
{
	clrbits_le32(&ch[chn].ao.test2_4,
		(0x1 << TEST2_4_TESTXTALKPAT_SHIFT) |
		(0x1 << TEST2_4_TESTAUDMODE_SHIFT) |
		(0x1 << TEST2_4_TESTAUDBITINV_SHIFT));

	if (!test_pat) {
		setbits_le32(&ch[chn].ao.perfctl0, 1 << PERFCTL0_RWOFOEN_SHIFT);

		clrbits_le32(&ch[chn].ao.test2_4,
			(0x1 << TEST2_4_TEST_REQ_LEN1_SHIFT) |
			(0x1 << TEST2_4_TESTSSOPAT_SHIFT) |
			(0x1 << TEST2_4_TESTSSOXTALKPAT_SHIFT) |
			(0x1 << TEST2_4_TESTXTALKPAT_SHIFT));
	} else {
		clrsetbits_le32(&ch[chn].ao.test2_4,
			TEST2_4_TESTAUDINIT_MASK | TEST2_4_TESTAUDINC_MASK,
			(0x11 << 8) | (0xd << 0) | (0x1 << 14));
	}
	clrsetbits_le32(&ch[chn].ao.test2_3,
		(0x1 << TEST2_3_TESTAUDPAT_SHIFT) | TEST2_3_TESTCNT_MASK,
		(test_pat ? 1 : 0) << TEST2_3_TESTAUDPAT_SHIFT);
}

static u32 dram_k_perbit(u8 chn, enum CAL_TYPE type)
{
	u32 err_value;

	if (type == RX_WIN_RD_DQC) {
		err_value = dramc_rd_dqc_run(chn);
	} else {
		dramc_engine2_setpat(chn, true);
		err_value = dramc_engine2_run(chn, TE_OP_WRITE_READ_CHECK);
		dramc_engine2_setpat(chn, false);
		err_value |= dramc_engine2_run(chn, TE_OP_WRITE_READ_CHECK);
	}
	return err_value;
}

static u8 dramc_window_perbit_cal(u8 chn, u8 rank,
		enum CAL_TYPE type, const struct sdram_params *params)
{
	u8 vref = 0, vref_begin = 0, vref_end = 1, vref_step = 1;
	u8 dly_step = 2, vref_scan_enable = 0;
	s16 dly, dly_begin = 0, dly_end = 0, last_step;
	s16 dly_pass;
	u32 dummy_rd_backup = 0, err_value, finish_bit;
	static u16 dq_precal_result[DQS_NUMBER];
	static struct vref_perbit_dly vref_dly;
	struct dqdqs_perbit_dly dq_perbit_dly[DQ_DATA_WIDTH];

	dramc_get_vref_prop(rank, type,
		&vref_scan_enable, &vref_begin, &vref_end);
	if (vref_scan_enable && type == RX_WIN_RD_DQC)
		dramc_rx_vref_enable(chn);

	dramc_dbg("[channel %d] [rank %d] type:%d, vref_enable:%d\n",
		  chn, rank, type, vref_scan_enable);

	if ((type == TX_WIN_DQ_ONLY) || (type == TX_WIN_DQ_DQM)) {
		for (size_t i = 0; i < 2; i++) {
			write32(&ch[chn].phy.shu[0].rk[rank].b[i].dq[0], 0);
			clrbits_le32(&ch[chn].phy.shu[0].rk[rank].b[i].dq[1],
				0xf);
		}
		setbits_le32(&ch[chn].phy.misc_ctrl1,
			0x1 << MISC_CTRL1_R_DMAR_FINE_TUNE_DQ_SW_SHIFT);
		setbits_le32(&ch[chn].ao.dqsoscr,
			0x1 << DQSOSCR_AR_COARSE_TUNE_DQ_SW_SHIFT);
		vref_step = 2;
	}

	if (type == RX_WIN_RD_DQC) {
		dramc_rd_dqc_init(chn, rank);
	} else {
		dummy_rd_backup = read32(&ch[chn].ao.dummy_rd);
		dramc_engine2_init(chn, rank, 0x400, false);
	}

	vref_dly.max_win = 0;
	vref_dly.min_win = 0xffff;
	for (vref = vref_begin; vref < vref_end; vref += vref_step) {
		vref_dly.vref = vref;
		finish_bit = 0;
		for (size_t i = 0; i < DQ_DATA_WIDTH; i++) {
			dq_perbit_dly[i].dqdly.first = -1;
			dq_perbit_dly[i].dqdly.last = -2;
			dq_perbit_dly[i].dqsdly.first = -1;
			dq_perbit_dly[i].dqsdly.last = -2;
			dq_perbit_dly[i].dqdly.best_first = -1;
			dq_perbit_dly[i].dqdly.best_last = -2;
			dq_perbit_dly[i].dqsdly.best_first = -1;
			dq_perbit_dly[i].dqsdly.best_last = -2;
		}

		if (vref_scan_enable)
			dramc_set_vref(chn, rank, type, vref_dly.vref);

		if ((type == RX_WIN_RD_DQC) || (type == RX_WIN_TEST_ENG)) {
			dramc_set_rx_dly_factor(chn, rank,
				RX_DQM, FIRST_DQ_DELAY);
			dramc_set_rx_dly_factor(chn, rank,
				RX_DQ, FIRST_DQ_DELAY);
		}

		dramc_get_dly_range(chn, rank, type, dq_precal_result,
			&dly_begin, &dly_end, params);
		for (dly = dly_begin; dly < dly_end; dly += dly_step) {
			dramc_set_dqdqs_dly(chn, rank, type, dly);
			err_value = dram_k_perbit(chn, type);
			finish_bit = 0;
			if (!vref_scan_enable)
				dramc_dbg("%d ", dly);

			for (size_t bit = 0; bit < DQ_DATA_WIDTH; bit++) {
				bool flag;
				bool fail = (err_value & ((u32) 1 << bit)) != 0;

				if (dly < 0) {
					dly_pass = -dly;
					last_step = -FIRST_DQS_DELAY;
					flag = false;
				} else {
					dly_pass = dly;
					last_step = dly_end;
					flag = true;
				}

				/* pass window bigger than 7,
				   consider as real pass window */
				if (dramc_check_dqdqs_win(&(dq_perbit_dly[bit]),
							  dly_pass, last_step,
							  fail, flag) > 7)
					finish_bit |= (1 << bit);

				if (vref_scan_enable)
					continue;
				dramc_dbg("%s", !fail ? "o" : "x");
			}

			if (!vref_scan_enable)
				dramc_dbg(" [MSB]\n");
			if (finish_bit == ((1 << DQ_DATA_WIDTH) - 1)) {
				dramc_dbg("all bits window found, break!\n");
				break;
			}
		}

		for (size_t bit = 0; bit < DQ_DATA_WIDTH; bit++)
			dramc_dbg("Dq[%zd] win(%d ~ %d)\n", bit,
				  dq_perbit_dly[bit].dqdly.best_first,
				  dq_perbit_dly[bit].dqdly.best_last);

		if (dramk_calc_best_vref(type, vref, &vref_dly, dq_perbit_dly))
			break;

		if (finish_bit == ((1 << DQ_DATA_WIDTH) - 1)) {
			dramc_dbg("all bits window found, break!\n");
			break;
		}
	}

	if (type == RX_WIN_RD_DQC) {
		dramc_rd_dqc_end(chn);
	} else {
		dramc_engine2_end(chn);
		write32(&ch[chn].ao.dummy_rd, dummy_rd_backup);
	}

	if (vref_scan_enable)
		dramc_set_vref(chn, rank, type, vref_dly.vref);

	if ((type == RX_WIN_RD_DQC) || (type == RX_WIN_TEST_ENG))
		dramc_set_rx_best_dly(chn, rank, vref_dly.perbit_dly);
	else
		dramc_set_tx_best_dly(chn, rank, vref_dly.perbit_dly,
			dq_precal_result, params);
	return 0;
}

static void dramc_dle_factor_handler(u8 chn, u8 val)
{
	val = MAX(val, 2);
	clrsetbits_le32(&ch[chn].ao.shu[0].conf[1],
		SHU_CONF1_DATLAT_MASK | SHU_CONF1_DATLAT_DSEL_MASK |
		SHU_CONF1_DATLAT_DSEL_PHY_MASK,
		(val << SHU_CONF1_DATLAT_SHIFT) |
		((val - 2) << SHU_CONF1_DATLAT_DSEL_SHIFT) |
		((val - 2) << SHU_CONF1_DATLAT_DSEL_PHY_SHIFT));
	dram_phy_reset(chn);
}

static u8 dramc_rx_datlat_cal(u8 chn, u8 rank)
{
	s32 datlat, first = -1, sum = 0, best_step;

	best_step = read32(&ch[chn].ao.shu[0].conf[1]) & SHU_CONF1_DATLAT_MASK;

	dramc_dbg("[DATLAT] start. CH%d RK%d DATLAT Default: %2x\n",
		   chn, rank, best_step);

	u32 dummy_rd_backup = read32(&ch[chn].ao.dummy_rd);
	dramc_engine2_init(chn, rank, 0x400, false);

	for (datlat = 12; datlat < DATLAT_TAP_NUMBER; datlat++) {
		dramc_dle_factor_handler(chn, datlat);

		u32 err = dramc_engine2_run(chn, TE_OP_WRITE_READ_CHECK);

		if (err != 0 && first != -1)
			break;

		if (sum >= 4)
			break;

		if (err == 0) {
			if (first == -1)
				first = datlat;
			sum++;
		}

		dramc_dbg("Datlat=%2d, err_value=0x%8x, sum=%d\n",
			   datlat, err, sum);
	}

	dramc_engine2_end(chn);
	write32(&ch[chn].ao.dummy_rd, dummy_rd_backup);

	best_step = first + (sum >> 1);
	dramc_dbg("First_step=%d, total pass=%d, best_step=%d\n",
		  first, sum, best_step);

	assert(sum != 0);

	dramc_dle_factor_handler(chn, best_step);

	clrsetbits_le32(&ch[chn].ao.padctrl, PADCTRL_DQIENQKEND_MASK,
		(0x1 << PADCTRL_DQIENQKEND_SHIFT) |
		(0x1 << PADCTRL_DQIENLATEBEGIN_SHIFT));

	return (u8) best_step;
}

static void dramc_dual_rank_rx_datlat_cal(u8 chn, u8 datlat0, u8 datlat1)
{
	u8 final_datlat = MAX(datlat0, datlat1);
	dramc_dle_factor_handler(chn, final_datlat);
}

static void dramc_rx_dqs_gating_post_process(u8 chn)
{
	u8 rank_rx_dvs, dqsinctl;
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
		for (size_t dqs = 0; dqs < DQS_NUMBER; dqs++) {
			best_coarse_tune2t[rank][dqs] =
			   (dqsg0 >> (dqs * 8)) &
			    SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_MASK;
			best_coarse_tune2t_p1[rank][dqs] =
			    ((dqsg0 >> (dqs * 8)) &
			     SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1_MASK) >>
			    SHURK_SELPH_DQSG0_TX_DLY_DQS0_GATED_P1_SHIFT;
			dramc_dbg("Rank%zd best DQS%zd dly(2T,(P1)2T)=(%d, %d)\n",
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
			dramc_dbg("Rank: %zd\n", rank);
			for (size_t dqs = 0; dqs < DQS_NUMBER; dqs++) {
				best_coarse_tune2t[rank][dqs] += dqsinctl;
				best_coarse_tune2t_p1[rank][dqs] += dqsinctl;

				dramc_dbg("Best DQS%zd dly(2T) = (%d)\n",
					  dqs, best_coarse_tune2t[rank][dqs]);
				dramc_dbg("Best DQS%zd P1 dly(2T) = (%d)\n",
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
	u8 rx_datlat[RANK_MAX] = {0};
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		for (u8 rk = RANK_0; rk < RANK_MAX; rk++) {
			dramc_show("Start K ch:%d, rank:%d\n", chn, rk);
			dramc_auto_refresh_switch(chn, false);
			dramc_cmd_bus_training(chn, rk, pams);
			dramc_write_leveling(chn, rk, pams->wr_level);
			dramc_auto_refresh_switch(chn, true);
			dramc_rx_dqs_gating_cal(chn, rk);
			dramc_window_perbit_cal(chn, rk, RX_WIN_RD_DQC, pams);
			dramc_window_perbit_cal(chn, rk, TX_WIN_DQ_DQM, pams);
			dramc_window_perbit_cal(chn, rk, TX_WIN_DQ_ONLY, pams);
			rx_datlat[rk] = dramc_rx_datlat_cal(chn, rk);
			dramc_window_perbit_cal(chn, rk, RX_WIN_TEST_ENG, pams);
		}

		dramc_rx_dqs_gating_post_process(chn);
		dramc_dual_rank_rx_datlat_cal(chn, rx_datlat[0], rx_datlat[1]);
	}
}
