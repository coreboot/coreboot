/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#include <arch/barrier.h>
#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/dramc_common.h>
#include <soc/dramc_register.h>
#include <soc/dramc_pi_api.h>
#include <soc/emi.h>
#include <soc/mt6391.h>
#include <soc/pll.h>
#include <soc/spm.h>
#include <string.h>
#include <types.h>

struct mem_pll {
	u8 delay;
	u8 phase;
	u8 done;
};

inline u8 is_dual_rank(u32 channel,
		       const struct mt8173_sdram_params *sdram_params)
{
	/* judge ranks from EMI_CONA[17] (cha) and EMI_CONA[16] (chb) */
        return (sdram_params->emi_set.cona & (1 << (17 - channel))) ? 1 : 0;
}

static void mem_pll_pre_init(u32 channel)
{
	write32(&ch[channel].ddrphy_regs->lpddr2_3, 0x1 << 29 | 0x1 << 25 |
						    0xf << 16 | 0xffff);

	write32(&ch[channel].ddrphy_regs->lpddr2_4, 0x1 << 29 | 0x1 << 25 |
						    0xf << 16 | 0xffff);

	/* adjust DQS/DQM phase to get best margin */
	write32(&ch[channel].ddrphy_regs->selph12, 0x1 << 28 | 0xf << 20 |
						   0x1 << 12 | 0xf << 4);
	/* adjust DQ phase to get best margin */
	write32(&ch[channel].ddrphy_regs->selph13, 0xffffffff << 0);
	write32(&ch[channel].ddrphy_regs->selph14, 0xffffffff << 0);

	/* fix OCV effect */
	write32(&ch[channel].ddrphy_regs->selph15, 0x1 << 4 | 0xf << 0);

	/* pll register control by CPU and select internal pipe path */
	write32(&ch[channel].ddrphy_regs->peri[2], 0x11 << 24 | 0x11 << 16 |
						   0xff << 8  | 0x11 << 0);
	write32(&ch[channel].ddrphy_regs->peri[3], 0x11 << 24 | 0x51 << 16 |
						   0x11 << 8  | 0x11 << 0);

	/* enable clock sync and spm control clock */
	write32(&ch[channel].ddrphy_regs->mempll_divider, 0x9 << 24 |
							  0x1 << 15 |
							  0x2 << 4  |
							  0x1 << 1  |
							  0x1 << 0);
	/* pll2 enable from CPU control */
	write32(&ch[channel].ddrphy_regs->mempll05_divider, 0x1 << 27);

	/* enable chip top memory clock */
	setbits_le32(&ch[channel].ddrphy_regs->mempll_divider, 0x1 << 4);

	/* disable C/A and DQ M_CK clock gating */
	clrbits_le32(&ch[channel].ddrphy_regs->ddrphy_cg_ctrl, 0x1 << 2 |
							       0x1 << 1);

	/* enable spm control clock */
	clrbits_le32(&ch[channel].ddrphy_regs->mempll_divider, 0x1 << 15 |
							       0x1 << 0);
	/* enable dramc 2X mode */
	setbits_le32(&ch[channel].ao_regs->ddr2ctl, 1 << 0);

	/* select internal clock path */
	write32(&ch[channel].ddrphy_regs->peri[0], 0x21 << 24 | 0x27 << 16 |
						   0x1b << 8  | 0x3 << 0);

	write32(&ch[channel].ddrphy_regs->peri[1], 0x50 << 24 | 0x96 << 16 |
						   0x6 << 8   | 0x1e << 0);

	/* trigger to make memory clock correct phase */
	setbits_le32(&ch[channel].ddrphy_regs->mempll_divider, 0x1 << 24 |
							       0x1 << 7);

	if (channel == CHANNEL_A) {
		/* select memory clock sync for channel A (internal source) */
		clrbits_le32(&ch[channel].ddrphy_regs->mempll_divider, 0x1 << 3);
	}
}

static void mem_pll_init_set_params(u32 channel)
{
	u32 pattern1, pattern2, pattern3;
	u32 mempll_ic_3_0, mempll_bp_3_0;
	u32 mempll_fbdiv_6_0, mempll_m4pdiv_1_0;
	u32 mempll_br_1_0, mempll_bc_1_0, mempll_ir_3_0;

	mempll_fbdiv_6_0 = 0x7 << 16;
	mempll_br_1_0 = 0x1 << 10;
	mempll_bc_1_0 = 0x0 << 8;
	mempll_ir_3_0 = 0xc << 28;
	mempll_ic_3_0 = 0x6 << 8;
	mempll_bp_3_0 = 0x1 << 12;
	mempll_m4pdiv_1_0 = 0x0 << 28;

	write32(&ch[channel].ddrphy_regs->mempll[14], 0x0);

	write32(&ch[channel].ddrphy_regs->mempll[3], 0x3 << 30 |
						     0x1 << 28);
	/* mempll 2 config */
	pattern1 = mempll_ir_3_0 | mempll_fbdiv_6_0 | mempll_ic_3_0;
	pattern2 = mempll_m4pdiv_1_0;
	pattern3 = mempll_bp_3_0 | mempll_br_1_0 | mempll_bc_1_0;

	/* mempll2_autok_en = 1, mempll2_autok_load = 1 */
	write32(&ch[channel].ddrphy_regs->mempll[5], 0x1 << 26 | 0x3 << 24 |
						     0x1 << 23 | pattern1);
	write32(&ch[channel].ddrphy_regs->mempll[6], 0x1 << 30 | 0x3 << 26 |
						     0x3 << 14 | pattern2);
	write32(&ch[channel].ddrphy_regs->mempll[7], 0x1 << 17 | 0x1 << 0  |
						     pattern3);
	/* mempll 4 */
	write32(&ch[channel].ddrphy_regs->mempll[11], 0x1 << 26 | 0x3 << 24 |
						      0x1 << 23 | pattern1);
	write32(&ch[channel].ddrphy_regs->mempll[12], 0x1 << 30 | 0x3 << 26 |
						      0x3 << 14 | pattern2);
	write32(&ch[channel].ddrphy_regs->mempll[13], 0x1 << 0  | pattern3);

	/* mempll 3 - enable signal tie together */
	write32(&ch[channel].ddrphy_regs->mempll[8], 0x1 << 26 | 0x3 << 24 |
						     0x1 << 23 | pattern1);
	write32(&ch[channel].ddrphy_regs->mempll[9], 0x1 << 30 | 0x3 << 26 |
						     0x3 << 14 | pattern2);
	write32(&ch[channel].ddrphy_regs->mempll[10], 0x1 << 17 | 0x1 << 0 |
						      pattern3);
}

static void mem_pll_init_phase_sync(u32 channel)
{
	write32(&ch[channel].ddrphy_regs->mempll_divider, BIT(27) | BIT(24) |
							  BIT(7)  | BIT(5)  |
							  BIT(4)  | BIT(0));
	/* spm control clock enable */
	clrsetbits_le32(&ch[channel].ddrphy_regs->mempll_divider, BIT(0),
								  BIT(1));

	clrsetbits_le32(&ch[channel].ddrphy_regs->mempll_divider, BIT(1),
								  BIT(0));
}

static void pll_phase_adjust(u32 channel, struct mem_pll *mempll, int reg_offs)
{
	switch (mempll->phase) {

	case MEMPLL_INIT:
		/* initial phase: zero out RG_MEPLL(2,3,4)_(REF_DL,FB)_DL */
		clrbits_le32(&ch[channel].ddrphy_regs->mempll[reg_offs],
			     0x1f << MEMPLL_REF_DL_SHIFT |
			     0x1f << MEMPLL_FB_DL_SHIFT);
		break;

	case MEMPLL_REF_LAG:
		/* REF lag FBK, delay FBK */
		clrsetbits_le32(&ch[channel].ddrphy_regs->mempll[reg_offs],
				0x1f << MEMPLL_REF_DL_SHIFT |
				0x1f << MEMPLL_FB_DL_SHIFT,
				mempll->delay << MEMPLL_FB_DL_SHIFT);
		break;

	case MEMPLL_REF_LEAD:
		/* REF lead FBK, delay REF */
		clrsetbits_le32(&ch[channel].ddrphy_regs->mempll[reg_offs],
				0x1f << MEMPLL_REF_DL_SHIFT |
				0x1f << MEMPLL_FB_DL_SHIFT,
				mempll->delay << MEMPLL_REF_DL_SHIFT);
	};
}

static void pll_phase_check(u32 channel, struct mem_pll *mempll, int idx)
{
	u32 value = read32(&ch[channel].ddrphy_regs->jmeter_pll_st[idx]);
	u16 one_count = (u16)((value >> 16) & 0xffff);
	u16 zero_count = (u16)(value & 0xffff);

	dramc_dbg_msg("PLL %d, phase %d, one_count %d, zero_count %d\n",
		      (idx + 2), mempll->phase, one_count, zero_count);

	switch (mempll->phase) {

	case MEMPLL_INIT:
		if ((one_count - zero_count) > JMETER_COUNT_N) {
			/* REF lag FBK */
			mempll->phase = MEMPLL_REF_LAG;
			mempll->delay++;
		} else if ((zero_count - one_count) > JMETER_COUNT_N) {
			/* REF lead FBK */
			mempll->phase = MEMPLL_REF_LEAD;
			mempll->delay++;
		} else {
			/* in-phase at initial */
			mempll->done = 1;
		}
		break;

	case MEMPLL_REF_LAG:
		if (JMETER_COUNT_N >= (one_count - zero_count)) {
			mempll->done = 1;
		} else {
			mempll->delay++;
		}
		break;

	case MEMPLL_REF_LEAD:
		if (JMETER_COUNT_N >= (zero_count - one_count)) {
			mempll->done = 1;
		} else {
			mempll->delay++;
		}
	}
}

static void mem_pll_phase_cali(u32 channel)
{
	u32 i;

	struct mem_pll mempll[3] =
	{
		{0, 0, 0},
		{0, 0, 0},
		{0, 0, 0},
	};

	dramc_dbg_msg("[PLL_Phase_Calib] ===== PLL Phase Calibration: ");
	dramc_dbg_msg("CHANNEL %d (0: CHA, 1: CHB) =====\n", channel);

	/* 1. set jitter meter count number to 1024 for mempll 2 3 4 */
	for (i = 0; i < 3; i++)
		clrsetbits_le32(&ch[channel].ddrphy_regs->jmeter[i],
				JMETER_COUNTER_MASK,
				JMETER_COUNT << JMETER_COUNTER_SHIFT);

	while (1) {

		for (i = 0; i < 3; i++) {
			if (!mempll[i].done) {
				pll_phase_adjust(channel, &mempll[i], (i + 2) * 3);
			}
		}

		udelay(20);  /* delay 20us for external loop pll stable */

		/* 2. enable mempll 2 3 4 jitter meter */
		for (i = 0; i < 3; i++)
			setbits_le32(&ch[channel].ddrphy_regs->jmeter[i],
				     JMETER_EN_BIT);

		/* 3. wait for jitter meter complete */
		udelay(JMETER_WAIT_DONE_US);

		/* 4. check jitter meter counter value for mempll 2 3 4 */
		for (i = 0; i < 3; i++) {
			if (!mempll[i].done) {
				pll_phase_check(channel, &mempll[i], i);
			}
		}

		/* 5. disable mempll 2 3 4 jitter meter */
		for (i = 0; i < 3; i++)
			clrbits_le32(&ch[channel].ddrphy_regs->jmeter[i],
				     JMETER_EN_BIT);

		/* 6. all done early break */
		if (mempll[0].done && mempll[1].done && mempll[2].done)
			break;

		/* 7. delay line overflow break */
		for (i = 0; i < 3; i++) {
			if (mempll[i].delay >= 32) {
				die("MEMPLL calibration fail\n");
			}
		}
	}

	dramc_dbg_msg("pll done: ");

	dramc_dbg_msg("%d, %d, %d\n",
		      mempll[0].done, mempll[1].done, mempll[2].done);
	dramc_dbg_msg("pll dl: %d, %d, %d\n",
		      mempll[0].delay, mempll[1].delay, mempll[2].delay);
}

void mem_pll_init(const struct mt8173_sdram_params *sdram_params)
{
	u32 channel;

	/* udelay waits for PLL to stabilize in this function */
	printk(BIOS_DEBUG, "[PLL] mempll_init and cali\n");

	/* mempll pre_init for two channels */
	for (channel = 0; channel < CHANNEL_NUM; channel++)
		mem_pll_pre_init(channel);

	/* only set once in MPLL */
	mt_mem_pll_config_pre(sdram_params);

	for (channel = 0; channel < CHANNEL_NUM; channel++)
		mem_pll_init_set_params(channel);

	udelay(1);  /* wait after da_mpll_sdm_iso_en goes low */

	/* only set once in MPLL */
	mt_mem_pll_config_post();

	udelay(100);

	for (channel = 0; channel < CHANNEL_NUM; channel++) {

		/* mempll_bias_en */
		write32(&ch[channel].ddrphy_regs->mempll[3], 0xd << 28 |
							     0x1 << 6);
		udelay(2);

		/* mempll2_en -> mempll4_en -> mempll3_en */
		setbits_le32(&ch[channel].ddrphy_regs->mempll[5], 1 << 0);
		setbits_le32(&ch[channel].ddrphy_regs->mempll[11], 1 << 0);
		setbits_le32(&ch[channel].ddrphy_regs->mempll[8], 1 << 0);

		udelay(100);

		/* mempll_bias_lpf_en */
		setbits_le32(&ch[channel].ddrphy_regs->mempll[3], 1 << 7);

		udelay(30);

		/* select mempll4 band register */
		setbits_le32(&ch[channel].ddrphy_regs->mempll[4], 1 << 26);
		clrbits_le32(&ch[channel].ddrphy_regs->mempll[4], 1 << 26);

		/* PLL ready */

		/* disable mempll2_en -> mempll4_en -> mempll3_en */
		clrbits_le32(&ch[channel].ddrphy_regs->mempll[5], 1 << 0);
		clrbits_le32(&ch[channel].ddrphy_regs->mempll[11], 1 << 0);
		clrbits_le32(&ch[channel].ddrphy_regs->mempll[8], 1 << 0);

		/* disable autok mempll2_en -> mempll4_en -> mempll3_en */
		clrbits_le32(&ch[channel].ddrphy_regs->mempll[5], 1 << 23);
		clrbits_le32(&ch[channel].ddrphy_regs->mempll[11], 1 << 23);
		clrbits_le32(&ch[channel].ddrphy_regs->mempll[8], 1 << 23);

		udelay(1);

		/* mempll[2->4->3]_fb_mck_sel=1 (switch to outer loop) */
		setbits_le32(&ch[channel].ddrphy_regs->mempll[6], 1 << 25);
		setbits_le32(&ch[channel].ddrphy_regs->mempll[12], 1 << 25);
		setbits_le32(&ch[channel].ddrphy_regs->mempll[9], 1 << 25);

		udelay(1);

		/* enable mempll2_en -> mempll4_en -> mempll3_en */
		setbits_le32(&ch[channel].ddrphy_regs->mempll[5], 1 << 0);
		setbits_le32(&ch[channel].ddrphy_regs->mempll[11], 1 << 0);
		setbits_le32(&ch[channel].ddrphy_regs->mempll[8], 1 << 0);
	}

	/* mempll new power-on */
	write32(&mtk_spm->poweron_config_set, 0x1 << 0 |
						 SPM_PROJECT_CODE << 16);
	/* request mempll reset/pdn mode */
	setbits_le32(&mtk_spm->power_on_val0, 0x1 << 27);

	udelay(2);

	/* unrequest mempll reset/pdn mode and wait settle */
	clrbits_le32(&mtk_spm->power_on_val0, 0x1 << 27);

	udelay(31);  /* PLL ready */

	for (channel = 0; channel < CHANNEL_NUM; channel++)
		mem_pll_init_phase_sync(channel);

	udelay(1);

	/* mempll calibration for two channels */
	for (channel = 0; channel < CHANNEL_NUM; channel++)
		mem_pll_phase_cali(channel);

	div2_phase_sync();  /* phase sync for channel B */

	mt_mem_pll_mux();
}

void dramc_pre_init(u32 channel, const struct mt8173_sdram_params *sdram_params)
{
	/* txdly_cs, txdly_cs1 */
	write32(&ch[channel].ao_regs->selph1, 0x0);
	/* txdly_dqsgate, txdly_dqsgate_p1 */
	write32(&ch[channel].ao_regs->selph2, 0x3 << 20 | 0x2 << 12);
	/* txldy_ra* */
	write32(&ch[channel].ao_regs->selph3, 0x0);
	/* txldy_ra* */
	write32(&ch[channel].ao_regs->selph4, 0x0);

	/* setting of write latency (WL=8) */
	write32(&ch[channel].ao_regs->selph7, 0x3333 << 16 | 0x3333);
	write32(&ch[channel].ao_regs->selph8, 0x3333 << 16 | 0x3333);
	write32(&ch[channel].ao_regs->selph9, 0x3333 << 16 | 0x3333);
	write32(&ch[channel].ao_regs->selph10, 0x5555 << 16 | 0xffff);
	write32(&ch[channel].ao_regs->selph11, 0x55 << 16 | 0xff);

	write32(&ch[channel].ao_regs->selph5, 0x1 << 26 | 0x2 << 22 |
					      0x1 << 20 | 0x5 << 16 |
					      0x5555);

	write32(&ch[channel].ao_regs->selph6_1, 0x4 << 8 | 0x3 << 4 |
						0x2 << 0);

	write32(&ch[channel].ao_regs->ac_time_05t,
		sdram_params->ac_timing.actim05t);
}

static void mrs_write(int channel, int rank, u32 mrs_value, unsigned int dly)
{
	write32(&ch[channel].ao_regs->mrs, rank << 28 | mrs_value);

	write32(&ch[channel].ao_regs->spcmd, 0x1);
	udelay(dly);
	write32(&ch[channel].ao_regs->spcmd, 0x0);
}

static void dramc_set_mrs_value(int channel, int rank,
			        const struct mt8173_sdram_params *sdram_params)
{
	/* MR63 -> Reset, Wait >=10us if not check DAI */
	mrs_write(channel, rank, sdram_params->mrs_set.mrs_63, 10);
	/* MR10 -> ZQ Init, tZQINIT>=1us */
	mrs_write(channel, rank, sdram_params->mrs_set.mrs_10, 1);
	/* MR3 driving stregth set to max */
	mrs_write(channel, rank, sdram_params->mrs_set.mrs_3, 1);
	/* MR1 */
	mrs_write(channel, rank, sdram_params->mrs_set.mrs_1, 1);
	/* MR2 */
	mrs_write(channel, rank, sdram_params->mrs_set.mrs_2, 1);
	/* MR11 ODT disable */
	mrs_write(channel, rank, sdram_params->mrs_set.mrs_11, 1);
}

void dramc_init(u32 channel, const struct mt8173_sdram_params *sdram_params)
{
	u32 bit, dual_rank_set;

	const struct mt8173_calib_params *calib_params;

	dual_rank_set = is_dual_rank(channel, sdram_params);
	calib_params = &sdram_params->calib_params;

	write32(&ch[channel].ddrphy_regs->peri[2], 0x1 << 12 |
						   0x1 << 4);

	write32(&ch[channel].ddrphy_regs->peri[3], 0x0);

	write32(&ch[channel].ao_regs->test2_4,
		sdram_params->ac_timing.test2_4);

	write32(&ch[channel].ao_regs->clk1delay, 0x1 << 23 |
						 0x1 << 22 |
						 0x1 << 21);

	/* rank config */
	assert((sdram_params->ac_timing.rkcfg & 0x1) == dual_rank_set);
	write32(&ch[channel].ao_regs->rkcfg,
		sdram_params->ac_timing.rkcfg);

	/* pimux */
	write32(&ch[channel].ao_regs->mckdly, 0x1 << 30 |
					      0x1 << 20 |
					      0x1 << 4);

	write32(&ch[channel].ddrphy_regs->mckdly, 0x1 << 8);

	write32(&ch[channel].ao_regs->padctl4, 0x1 << 0);

	/* tCKEH/tCKEL extend 1T */
	write32(&ch[channel].ao_regs->dummy, 0x1 << 31 |
					     0x3 << 10 |
					     0x1 << 4);

	/* driving control */
	write32(&ch[channel].ao_regs->iodrv6, DEFAULT_DRIVING |
					      DRIVING_DS2_0 << 20 |
					      DRIVING_DS2_0 << 4);

	write32(&ch[channel].ddrphy_regs->drvctl1, DEFAULT_DRIVING |
						   DRIVING_DS2_0 << 20);

	write32(&ch[channel].ao_regs->drvctl1, DEFAULT_DRIVING |
					       DRIVING_DS2_0 << 4);

	/* enable dqs signal output */
	write32(&ch[channel].ddrphy_regs->ioctl, 0x0);

	/* rank 0 dqs gating delay */
	write32(&ch[channel].ao_regs->dqsien[0], 0x40 << 24 |
						 0x40 << 16 |
						 0x40 << 8  |
						 0x40 << 0);

	write32(&ch[channel].ao_regs->dqsctl1, 0x1 << 28 |
					       0x5 << 24);

	write32(&ch[channel].ao_regs->dqsctl2, 0x5 << 0);
	write32(&ch[channel].ao_regs->phyctl1, 0x1 << 25);
	write32(&ch[channel].ao_regs->gddr3ctl1, 0x1 << 24);
	write32(&ch[channel].ddrphy_regs->gddr3ctl1, 0x1 << 28);
	write32(&ch[channel].ao_regs->arbctl0, 0x80 << 0);

	/* enable clock pad 0 */
	write32(&ch[channel].ao_regs->clkctl, 0x1 << 28);

	udelay(1);

	write32(&ch[channel].ao_regs->conf1,
		sdram_params->ac_timing.conf1);

	/* bit 17,18 would bypass some dummy path */
	write32(&ch[channel].ddrphy_regs->dqsgctl, 0x1 << 31 |
						   0x1 << 30 |
						   0x1 << 17 |
						   0x1 << 18 |
						   0x1 << 4  |
						   0x1 << 0);

	write32(&ch[channel].ao_regs->dqscal0, 0x0);
	write32(&ch[channel].ddrphy_regs->dqscal0, 0x0);

	write32(&ch[channel].ao_regs->actim0,
		sdram_params->ac_timing.actim);

	write32(&ch[channel].ao_regs->misctl0,
		sdram_params->ac_timing.misctl0);
	write32(&ch[channel].ddrphy_regs->misctl0,
		sdram_params->ac_timing.misctl0);

	write32(&ch[channel].ao_regs->perfctl0, 0x1 << 20);

	write32(&ch[channel].ao_regs->ddr2ctl,
		sdram_params->ac_timing.ddr2ctl);
	write32(&ch[channel].ddrphy_regs->ddr2ctl,
		sdram_params->ac_timing.ddr2ctl);

	write32(&ch[channel].ao_regs->misc, 0xb << 8 |
					    0x1 << 7 |
					    0x1 << 6 |
					    0x1 << 5);

	write32(&ch[channel].ao_regs->dllconf, 0xf << 28 |
					       0x1 << 24);

	write32(&ch[channel].ao_regs->actim1,
		sdram_params->ac_timing.actim1);

	write32(&ch[channel].ddrphy_regs->dqsisel, 0x0);

	/* disable ODT before ZQ calibration */
	write32(&ch[channel].ao_regs->wodt, 0x1 << 0);

	write32(&ch[channel].ao_regs->padctl4, 0x1 << 2 |
					       0x1 << 0);

	udelay(200);  /* tINIT3 > 200us */

	write32(&ch[channel].ao_regs->gddr3ctl1, 0x1 << 24 |
						 0x1 << 20);

	write32(&ch[channel].ddrphy_regs->gddr3ctl1, 0x1 << 28);

	/* set mode register value */
	dramc_set_mrs_value(channel, 0, sdram_params);

	if (dual_rank_set)
		dramc_set_mrs_value(channel, 1, sdram_params);

	write32(&ch[channel].ao_regs->gddr3ctl1,
		sdram_params->ac_timing.gddr3ctl1);
	write32(&ch[channel].ddrphy_regs->gddr3ctl1,
		sdram_params->ac_timing.gddr3ctl1);

	write32(&ch[channel].ao_regs->dramc_pd_ctrl,
		sdram_params->ac_timing.pd_ctrl);

	write32(&ch[channel].ao_regs->padctl4, 0x1 << 0);
	write32(&ch[channel].ao_regs->perfctl0, 0x1 << 20 | 0x1 << 0);
	write32(&ch[channel].ao_regs->zqcs, 0xa << 8 | 0x56 << 0);
	write32(&ch[channel].ddrphy_regs->padctl1, 0x0);

	write32(&ch[channel].ao_regs->test2_3,
		sdram_params->ac_timing.test2_3);

	write32(&ch[channel].ao_regs->conf2,
		sdram_params->ac_timing.conf2);

	write32(&ch[channel].ddrphy_regs->padctl2, 0x0);

	/* DISABLE_DRVREF */
	write32(&ch[channel].ao_regs->ocdk, 0x0);
	write32(&ch[channel].ddrphy_regs->ocdk, 0x0);

	write32(&ch[channel].ao_regs->r1deldly, 0x12 << 24 |
						0x12 << 16 |
						0x12 << 8  |
						0x12 << 0);

	write32(&ch[channel].ao_regs->padctl7, 0x0);

	/* CLKTDN, DS0TDN, DS1TDN, DS2TDN, DS3TDN */
	setbits_le32(&ch[channel].ddrphy_regs->tdsel[2], 0x1 << 31 |
							 0x1 << 29 |
							 0x1 << 27 |
							 0x1 << 25 |
							 0x1 << 1);
	/* DISABLE_PERBANK_REFRESH */
	clrbits_le32(&ch[channel].ao_regs->rkcfg, 0x1 << 7);

	/* clear R_DMREFTHD to reduce MR4 wait refresh queue time */
	clrbits_le32(&ch[channel].ao_regs->conf2, 0x7 << 24);

	/* duty default value */
	write32(&ch[channel].ddrphy_regs->phyclkduty, 0x1 << 28 |
						      0x1 << 16);

	if (!dual_rank_set) {
		/* single rank, CKE1 always off */
		setbits_le32(&ch[channel].ao_regs->gddr3ctl1, 0x1 << 21);
	}

	/* default dqs rx perbit input delay */
	write32(&ch[channel].ao_regs->r0deldly,
		calib_params->rx_dqs_dly[channel]);

	write32(&ch[channel].ao_regs->r1deldly,
		calib_params->rx_dqs_dly[channel]);

	for (bit = 0; bit < DQS_BIT_NUMBER; bit++)
		write32(&ch[channel].ao_regs->dqidly[bit],
			calib_params->rx_dq_dly[channel][bit]);
}

void div2_phase_sync(void)
{
	clrbits_le32(&ch[CHANNEL_B].ddrphy_regs->mempll_divider,
			1 << MEMCLKENB_SHIFT);
	udelay(1);

	setbits_le32(&ch[CHANNEL_B].ddrphy_regs->mempll_divider,
			1 << MEMCLKENB_SHIFT);
}

void dramc_phy_reset(u32 channel)
{
	/* reset phy */
	setbits_le32(&ch[channel].ddrphy_regs->phyctl1,
		     1 << PHYCTL1_PHYRST_SHIFT);

	/* read data counter reset */
	setbits_le32(&ch[channel].ao_regs->gddr3ctl1,
		     1 << GDDR3CTL1_RDATRST_SHIFT);

	udelay(1);  /* delay 1ns */

	clrbits_le32(&ch[channel].ao_regs->gddr3ctl1,
		     1 << GDDR3CTL1_RDATRST_SHIFT);

	clrbits_le32(&ch[channel].ddrphy_regs->phyctl1,
		     1 << PHYCTL1_PHYRST_SHIFT);
}

void dramc_runtime_config(u32 channel,
			  const struct mt8173_sdram_params *sdram_params)
{
	setbits_le32(&ch[channel].ddrphy_regs->dqsgctl,
		BIT(17)|BIT(18));

	/* enable hw gating */
	setbits_le32(&ch[channel].ao_regs->dqscal0,
		     1 << DQSCAL0_STBCALEN_SHIFT);

	/* if frequency >1600, tCKE should >7 clk */
	setbits_le32(&ch[channel].ao_regs->dummy, 0x1 << 4);

	if (sdram_params->dram_freq * 2 < 1600 * MHz)
		die("set tCKE error in runtime config");

	/* DDRPHY C/A and DQ M_CK clock gating enable */
	setbits_le32(&ch[channel].ddrphy_regs->ddrphy_cg_ctrl, 0x1 << 2 |
							       0x1 << 1);

	setbits_le32(&ch[channel].ao_regs->perfctl0, BIT(19) | BIT(14) |
						     BIT(11) | BIT(10) |
						     BIT(9)  | BIT(8)  |
						     BIT(4)  | BIT(0));
	/* ZQCS_ENABLE */
	if (sdram_params->emi_set.cona & 0x1) {
		/* dual channel, clear ZQCSCNT */
		clrbits_le32(&ch[channel].ao_regs->spcmd, 0xff << 16);
		/* set ZQCSMASK for different channels */
		if (channel == CHANNEL_A) {
			clrbits_le32(&ch[channel].ao_regs->perfctl0, 0x1 << 24);
		} else {
			setbits_le32(&ch[channel].ao_regs->perfctl0, 0x1 << 24);
		}
		/* enable ZQCSDUAL */
		setbits_le32(&ch[channel].ao_regs->perfctl0, 0x1 << 25);
	} else {
		/* single channel, set ZQCSCNT */
		setbits_le32(&ch[channel].ao_regs->spcmd, 0x8 << 16);
	}
}

void transfer_to_spm_control(void)
{
	u32 msk;

	msk = BIT(7) | BIT(11) | BIT(15);
	clrbits_le32(&mtk_apmixed->ap_pll_con3, msk);

	msk = BIT(0) | BIT(4) | BIT(8);
	clrbits_le32(&ch[CHANNEL_A].ddrphy_regs->peri[3], msk);

	msk = BIT(0) | BIT(8);
	clrbits_le32(&ch[CHANNEL_B].ddrphy_regs->peri[3], msk);

	msk = BIT(0) | BIT(9) | BIT(10) | BIT(11) | BIT(16) | BIT(24);
	clrbits_le32(&ch[CHANNEL_A].ddrphy_regs->peri[2], msk);
	clrbits_le32(&ch[CHANNEL_B].ddrphy_regs->peri[2], msk);
}

void transfer_to_reg_control(void)
{
	u32 val;

	val = BIT(7) | BIT(11) | BIT(15);
	setbits_le32(&mtk_apmixed->ap_pll_con3, val);

	val = BIT(0) | BIT(4) | BIT(8);
	setbits_le32(&ch[CHANNEL_A].ddrphy_regs->peri[3], val);

	val = BIT(0) | BIT(8);
	write32(&ch[CHANNEL_B].ddrphy_regs->peri[3], val);

	val = BIT(0) | BIT(9) | BIT(10) | BIT(11) | BIT(16) | BIT(24);
	setbits_le32(&ch[CHANNEL_A].ddrphy_regs->peri[2], val);
	setbits_le32(&ch[CHANNEL_B].ddrphy_regs->peri[2], val);
}

u32 dramc_engine2(u32 channel, enum dram_tw_op wr, u32 test2_1, u32 test2_2,
		  u8 testaudpat, u8 log2loopcount)
{
	u32 value;

	if (log2loopcount > 15)
		die("Invalid loopcount of engine2!");

	/* Disable Test Agent1, Test Agent2 write/read */
	clrbits_le32(&ch[channel].ao_regs->conf2, CONF2_TEST1_EN |
						  CONF2_TEST2R_EN |
						  CONF2_TEST2W_EN);

	/* 1. set pattern, base address, offset address */
	write32(&ch[channel].nao_regs->test2_1, test2_1);
	write32(&ch[channel].nao_regs->test2_2, test2_2);

	/* 2. select test pattern */
	/*       TESTXTALKPAT | TESTAUDPAT
	 * ISI              0 | 0
	 * AUD              0 | 1
	 * XTALK            1 | 0
	 * UNKNOW           1 | 1
	 */
	switch (testaudpat) {
	case XTALK:
		/* TESTAUDPAT = 0 */
		clrbits_le32(&ch[channel].ao_regs->test2_3,
			     TEST2_3_TESTAUDPAT_EN);
		/* TESTXTALKPAT  = 1, select xtalk pattern
		 * TESTAUDMODE   = 0, read only
		 * TESTAUDBITINV = 0, no bit inversion
		 */
		clrsetbits_le32(&ch[channel].ao_regs->test2_4,
				TEST2_4_TESTAUDBITINV_EN |
				TEST2_4_TESTAUDMODE_EN,
				TEST2_4_TESTXTALKPAT_EN);
		break;
	case AUDIO:
		/* TESTAUDPAT = 1 */
		setbits_le32(&ch[channel].ao_regs->test2_3,
			     TEST2_3_TESTAUDPAT_EN);
		/* TESTXTALKPAT  = 0
		 * TESTAUDINIT   = 0x11
		 * TESTAUDINC    = 0x0d
		 * TESTAUDBITINV = 1
		 * TESTAUDMODE   = 1
		 */
		clrsetbits_le32(&ch[channel].ao_regs->test2_4,
				TEST2_4_TESTXTALKPAT_EN |
				TEST2_4_TESTAUDINIT_MASK |
				TEST2_4_TESTAUDINC_MASK,
				TEST2_4_TESTAUDMODE_EN |
				TEST2_4_TESTAUDBITINV_EN |
				0x11 << TEST2_4_TESTAUDINIT_SHIFT |
				0xd << TEST2_4_TESTAUDINC_SHIFT);

		break;
	case ISI:
		/* TESTAUDPAT = 0 */
		clrbits_le32(&ch[channel].ao_regs->test2_3,
			     TEST2_3_TESTAUDPAT_EN);
		/* TESTXTALKPAT = 0 */
		clrbits_le32(&ch[channel].ao_regs->test2_4,
			     TEST2_4_TESTXTALKPAT_EN);
	}

	/* 3. set loop number */
	clrsetbits_le32(&ch[channel].ao_regs->test2_3, TEST2_3_TESTCNT_MASK,
			log2loopcount << TEST2_3_TESTCNT_SHIFT);

	/* 4. enable read/write test */
	if (wr == TE_OP_READ_CHECK) {
			if ((testaudpat == 1) || (testaudpat == 2)) {
			/* if audio pattern, enable read only */
			/* (disable write after read), */
			/* AUDMODE=0x48[15]=0 */
			clrbits_le32(&ch[channel].ao_regs->test2_4,
				     TEST2_4_TESTAUDMODE_EN);
		}

		/* enable read, 0x008[30:30] */
		setbits_le32(&ch[channel].ao_regs->conf2, CONF2_TEST2R_EN);
	} else if (wr == TE_OP_WRITE_READ_CHECK) {
		/* enable write, 0x008[31:31] */
		setbits_le32(&ch[channel].ao_regs->conf2, CONF2_TEST2W_EN);

		/* check "read data compare ready" bit */
		do {
			value = read32(&ch[channel].nao_regs->testrpt);
		} while ((value & (1 << TESTRPT_DM_CMP_CPT_SHIFT)) == 0);

		/* Disable Test Agent2 write and enable Test Agent2 read */
		clrbits_le32(&ch[channel].ao_regs->conf2, CONF2_TEST2W_EN);
		setbits_le32(&ch[channel].ao_regs->conf2, CONF2_TEST2R_EN);
	}

	/* 5 check "read data compare ready" bit */
	do {
		value = read32(&ch[channel].nao_regs->testrpt);
	} while ((value & (1 << TESTRPT_DM_CMP_CPT_SHIFT)) == 0);

	/* delay 10ns after ready check from DE suggestion (1us here) */
	udelay(1);

	/* read CMP_ERR result */
	value = read32(&ch[channel].nao_regs->cmp_err);

	/* 6 disable read */
	clrbits_le32(&ch[channel].ao_regs->conf2, CONF2_TEST2R_EN);

	/* return CMP_ERR result, pass: 0, failure: otherwise */
	return value;
}
