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

static void dramc_read_dbi_onoff(u8 onoff)
{
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
		for (u8 b = 0; b < 2; b++)
			clrsetbits_le32(&ch[chn].phy.shu[0].b[b].dq[7],
				0x1 << SHU1_BX_DQ7_R_DMDQMDBI_SHU_SHIFT,
				onoff << SHU1_BX_DQ7_R_DMDQMDBI_SHU_SHIFT);
}

static void dramc_write_dbi_onoff(u8 onoff)
{
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
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
	u8 chn = 0;

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);

	for (chn = 0; chn < CHANNEL_MAX ; chn++) {
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
	}

	if (!en)
		dramc_phy_dcm_disable(chn);
	dramc_set_broadcast(broadcast_bak);
}

static void reset_delay_chain_before_calibration(void)
{
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
		for (u8 rank = 0; rank < RANK_MAX; rank++) {
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
	u8 shu = 0;

	dramc_enable_phy_dcm(0);
	reset_delay_chain_before_calibration();

	setbits_le32(&ch[0].ao.shu[0].conf[3], 0x1ff << 16);
	setbits_le32(&ch[0].ao.spcmdctrl, 0x1 << 24);
	clrsetbits_le32(&ch[0].ao.shu[0].scintv, 0x1f << 1, 0x1b << 1);

	for (shu = 0; shu < DRAM_DFS_SHUFFLE_MAX; shu++)
		setbits_le32(&ch[0].ao.shu[shu].conf[3], 0x1ff << 0);

	clrbits_le32(&ch[0].ao.dramctrl, 0x1 << 18);
	clrbits_le32(&ch[0].ao.spcmdctrl, 0x1 << 31);
	clrbits_le32(&ch[0].ao.spcmdctrl, 0x1 << 30);
	clrbits_le32(&ch[0].ao.dqsoscr, 0x1 << 26);
	clrbits_le32(&ch[0].ao.dqsoscr, 0x1 << 25);

	dramc_write_dbi_onoff(DBI_OFF);
	dramc_read_dbi_onoff(DBI_OFF);

	for (int chn = 0; chn < CHANNEL_MAX; chn++) {
		setbits_le32(&ch[chn].ao.spcmdctrl, 0x1 << 29);
		setbits_le32(&ch[chn].ao.dqsoscr, 0x1 << 24);
		for (shu = 0; shu < DRAM_DFS_SHUFFLE_MAX; shu++)
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
