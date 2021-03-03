/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/dramc_common.h>
#include <soc/dramc_register.h>
#include <soc/dramc_pi_api.h>
#include <soc/dramc_soc.h>
#include <soc/emi.h>

static u8 opt_gw_coarse_value[CHANNEL_NUM][DUAL_RANKS];
static u8 opt_gw_fine_value[CHANNEL_NUM][DUAL_RANKS];
static s8 wrlevel_dqs_dly[CHANNEL_NUM][DQS_NUMBER];

void sw_impedance_cal(u32 channel,
		      const struct mt8173_sdram_params *sdram_params)
{
	u32 mask, value;

	const struct mt8173_calib_params *params = &sdram_params->calib_params;

	dramc_dbg("[Imp Calibration] DRVP:%d\n", params->impedance_drvp);
	dramc_dbg("[Imp Calibration] DRVN:%d\n", params->impedance_drvn);

	mask = 0xf << 28 | 0xf << 24 | 0xf << 12 | 0xf << 8;  /* driving */

	value =  params->impedance_drvp << 28 | params->impedance_drvn << 24 |
		 params->impedance_drvp << 12 | params->impedance_drvn << 8;

	/* DQS and DQ */
	clrsetbits32(&ch[channel].ao_regs->iodrv6, mask, value);
	/* CLK and CMD */
	clrsetbits32(&ch[channel].ao_regs->drvctl1, mask, value);
	clrsetbits32(&ch[channel].ddrphy_regs->drvctl1, mask, value);
	/* DQ_2 and CMD_2 */
	clrsetbits32(&ch[channel].ao_regs->iodrv4, mask, value);
	/* disable impcal calibration */
	clrbits32(&ch[channel].ao_regs->impcal, 1 << IMP_CALI_ENP_SHIFT |
						   1 << IMP_CALI_ENN_SHIFT |
						   1 << IMP_CALI_EN_SHIFT  |
						   0xf << IMP_CALI_DRVP_SHIFT |
						   0xf << IMP_CALI_DRVN_SHIFT);
}

void ca_training(u32 channel, const struct mt8173_sdram_params *sdram_params)
{
	const struct mt8173_calib_params *params = &sdram_params->calib_params;

	u32 i, ca_shift_avg32 = 0;
	s8 ca_max_center, ca_shift_avg8 = 0, order, ca_shift[CATRAINING_NUM];

	s8 shift[CATRAINING_NUM] = {
		CMDDLY0_RA2_SHIFT, CMDDLY1_RA7_SHIFT, CMDDLY3_BA0_SHIFT,
		CMDDLY3_BA1_SHIFT, CMDDLY3_BA2_SHIFT, CMDDLY4_RAS_SHIFT,
		CMDDLY4_CAS_SHIFT, CMDDLY5_RA13_SHIFT, CMDDLY5_WE_SHIFT
	};

	s8 ca_order[CHANNEL_NUM][CATRAINING_NUM] = {
		{ 7, 5, 6, 1, 3, 0, 9, 8, 2, 4},
		{ 2, 0, 3, 7, 5, 9, 4, 1, 6, 8}
	};

	s8 cmd_order[CATRAINING_NUM] = {
		0, 1, 3, 3, 3, 4, 4, 5, 5
	};

	for (i = 0; i < CATRAINING_NUM; i++) {
		ca_shift[i] = params->ca_train[channel][i];
		ca_shift_avg8 += ca_shift[i];
	}

	/* CA pins align the center */
	ca_max_center = params->ca_train_center[channel];

	/* set CA pins output delay */
	for (i = 0; i < (CATRAINING_NUM - 1); i++) {
		order = ca_order[channel][i];
		clrsetbits32(&ch[channel].ddrphy_regs->cmddly[cmd_order[i]],
			     0xf << shift[i], ca_shift[order] << shift[i]);
	}

	order = ca_order[channel][9];
	clrsetbits32(&ch[channel].ddrphy_regs->dqscal0,
		     0xf << DQSCAL0_RA14_SHIFT,
		     ca_shift[order] << DQSCAL0_RA14_SHIFT);

	/* CKE and CS delay */
	ca_shift_avg32 = (u32)(ca_shift_avg8 + (CATRAINING_NUM >> 1));
	ca_shift_avg32 /= (u32)CATRAINING_NUM;

	/* CKEDLY */
	clrsetbits32(&ch[channel].ddrphy_regs->cmddly[4],
		     0x1f << CMDDLY4_CS_SHIFT |
		     0x1f << CMDDLY4_CKE_SHIFT,
		     ca_shift_avg32 << CMDDLY4_CS_SHIFT |
		     ca_shift_avg32 << CMDDLY4_CKE_SHIFT);

	/* CKE1DLY */
	clrsetbits32(&ch[channel].ao_regs->dqscal1,
		     0x1f << DQSCAL1_CKE1_SHIFT,
		     ca_shift_avg32 << DQSCAL1_CKE1_SHIFT);

	/* CS1DLY */
	clrsetbits32(&ch[channel].ddrphy_regs->padctl1,
		     0xf << PADCTL1_CS1_SHIFT,
		     ca_shift_avg32 << PADCTL1_CS1_SHIFT);

	/* set max center into clk output delay */
	clrsetbits32(&ch[channel].ddrphy_regs->padctl1,
		     0xf << PADCTL1_CLK_SHIFT,
		     ca_max_center << PADCTL1_CLK_SHIFT);

	dramc_dbg("=========================================\n");
	dramc_dbg("   [Channel %d] CA training\n", channel);
	dramc_dbg("=========================================\n");

	for (i = 0; i < CATRAINING_NUM; i++)
		dramc_dbg("[CA] CA %d\tShift %d\n", i, ca_shift[i]);

	dramc_dbg("[CA] Reg CMDDLY4 = %xh\n",
			read32(&ch[channel].ddrphy_regs->cmddly[4]));
	dramc_dbg("[CA] Reg DQSCAL1 = %xh\n",
			read32(&ch[channel].ao_regs->dqscal1));
	dramc_dbg("[CA] Reg PADCTL1 = %xh\n",
			read32(&ch[channel].ddrphy_regs->padctl1));
}

void write_leveling(u32 channel, const struct mt8173_sdram_params *sdram_params)
{
	u8 i, byte_i;
	u32 value;

	for (i = 0; i < DQS_NUMBER; i++)
		wrlevel_dqs_dly[channel][i] =
			sdram_params->calib_params.wr_level[channel][i];
	/* DQS */
	value = 0;
	for (i = 0; i < DQS_NUMBER; i++) {
		value += ((u32)wrlevel_dqs_dly[channel][i]) << (4 * i);
	}
	write32(&ch[channel].ddrphy_regs->padctl3, value);

	/* DQM */
	clrsetbits32(&ch[channel].ddrphy_regs->padctl2, MASK_PADCTL2_32BIT,
		     (value << PADCTL2_SHIFT) & MASK_PADCTL2_32BIT);

	/* DQ */
	for (byte_i = 0; byte_i < DQS_NUMBER; byte_i++) {
		value = 0;
		for (i = 0; i < DQS_BIT_NUMBER; i++) {
			s8 val = wrlevel_dqs_dly[channel][byte_i];
			value += (((u32)val) << (4 * i));
		}
		write32(&ch[channel].ddrphy_regs->dqodly[byte_i], value);
	}

	dramc_dbg("========================================\n");
	dramc_dbg("[Channel %d] dramc_write_leveling_swcal\n", channel);
	dramc_dbg("========================================\n");

	dramc_dbg("[WL] DQS: %#x",
			read32(&ch[channel].ddrphy_regs->padctl3));
	dramc_dbg("[WL] DQM: %#x\n",
			read32(&ch[channel].ddrphy_regs->padctl2));

	for (byte_i = 0; byte_i < DQS_NUMBER; byte_i++)
		dramc_dbg("[WL] DQ byte%d: %#x\n", byte_i,
			     read32(&ch[channel].ddrphy_regs->dqodly[byte_i]));
}

static void set_gw_coarse_factor(u32 channel, u8 curr_val)
{
	u8 curr_val_p1, selph2_dqsgate, selph2_dqsgate_p1;

	u32 coarse_tune_start = curr_val >> 2;

	if (coarse_tune_start > 3) {
		coarse_tune_start -= 3;
	} else {
		if (coarse_tune_start) {
			coarse_tune_start = 1;
		}
	}

	if (coarse_tune_start > 15) {
		coarse_tune_start = 15;
	}

	curr_val_p1 = curr_val + 2;  /* diff is 0.5T */

	/* Rank 0 P0/P1 coarse tune settings */
	clrsetbits32(&ch[channel].ao_regs->dqsctl1,
		     0xf << DQSCTL1_DQSINCTL_SHIFT,
		     coarse_tune_start << DQSCTL1_DQSINCTL_SHIFT &
		     0xf << DQSCTL1_DQSINCTL_SHIFT);

	/* DQSINCTL does not have P1. */
	/* Need to use TXDLY_DQSGATE/TXDLY_DQSGATE_P1 to set */
	/* different 1 M_CK coarse tune values for P0 & P1. */
	selph2_dqsgate = (curr_val >> 2) - coarse_tune_start;
	selph2_dqsgate_p1 = (curr_val_p1 >> 2) - coarse_tune_start;

	clrsetbits32(&ch[channel].ao_regs->selph2,
		     0x7 << SELPH2_TXDLY_DQSGATE_SHIFT |
		     0x7 << SELPH2_TXDLY_DQSGATE_P1_SHIFT,
		     selph2_dqsgate << SELPH2_TXDLY_DQSGATE_SHIFT |
		     selph2_dqsgate_p1 << SELPH2_TXDLY_DQSGATE_P1_SHIFT);

	/* dly_DQSGATE and dly_DQSGATE_P1 */
	clrsetbits32(&ch[channel].ao_regs->selph5,
		     0x3 << SELPH5_DLY_DQSGATE_SHIFT |
		     0x3 << SELPH5_DLY_DQSGATE_P1_SHIFT,
		     (curr_val & 0x3) << SELPH5_DLY_DQSGATE_SHIFT |
		     (curr_val_p1 & 0x3) << SELPH5_DLY_DQSGATE_P1_SHIFT);
}

static void set_gw_fine_factor(u32 channel, u8 curr_val, u8 rank)
{
	u32 set = curr_val & (0x7f << DQSIEN_DQS0IEN_SHIFT);

	clrsetbits32(&ch[channel].ao_regs->dqsien[rank],
		     0x7f << DQSIEN_DQS0IEN_SHIFT |
		     0x7f << DQSIEN_DQS1IEN_SHIFT |
		     0x7f << DQSIEN_DQS2IEN_SHIFT |
		     0x7f << DQSIEN_DQS3IEN_SHIFT,
		     set << DQSIEN_DQS0IEN_SHIFT |
		     set << DQSIEN_DQS1IEN_SHIFT |
		     set << DQSIEN_DQS2IEN_SHIFT |
		     set << DQSIEN_DQS3IEN_SHIFT);
}

static void set_gw_coarse_factor_rank1(u32 channel, u8 curr_val, u8 dqsinctl)
{
	u8 curr_val_p1, r1dqsgate, r1dqsgate_p1;

	curr_val_p1 = curr_val + 2;  /* diff is 0.5T */

	clrsetbits32(&ch[channel].ao_regs->dqsctl2,
		     0xf << DQSCTL2_DQSINCTL_SHIFT,
		     dqsinctl << DQSCTL2_DQSINCTL_SHIFT);

	/* TXDLY_R1DQSGATE and TXDLY_R1DQSGATE_P1 */
	r1dqsgate = (curr_val >> 2) - dqsinctl;
	r1dqsgate_p1 = (curr_val_p1 >> 2) - dqsinctl;

	clrsetbits32(&ch[channel].ao_regs->selph6_1,
		     0x7 << SELPH6_1_TXDLY_R1DQSGATE_SHIFT |
		     0x7 << SELPH6_1_TXDLY_R1DQSGATE_P1_SHIFT,
		     r1dqsgate << SELPH6_1_TXDLY_R1DQSGATE_SHIFT |
		     r1dqsgate_p1 << SELPH6_1_TXDLY_R1DQSGATE_P1_SHIFT);

	/* dly_R1DQSGATE and dly_R1DQSGATE_P1 */
	clrsetbits32(&ch[channel].ao_regs->selph6_1,
		     0x3 << SELPH6_1_DLY_R1DQSGATE_SHIFT |
		     0x3 << SELPH6_1_DLY_R1DQSGATE_P1_SHIFT,
		     (curr_val & 0x3) << SELPH6_1_DLY_R1DQSGATE_SHIFT |
		     (curr_val_p1 & 0x3) << SELPH6_1_DLY_R1DQSGATE_P1_SHIFT);
}

static void dqs_gw_counter_reset(u32 channel)
{
	/* reset dqs counter (1 to 0) */
	setbits32(&ch[channel].ao_regs->spcmd, 1 << SPCMD_DQSGCNTRST_SHIFT);
	clrbits32(&ch[channel].ao_regs->spcmd, 1 << SPCMD_DQSGCNTRST_SHIFT);
	dramc_phy_reset(channel);
}

static int dqs_gw_test(u32 channel)
{
	u32 coarse_result01, coarse_result23;

	/* read data counter reset in PHY layer */
	dqs_gw_counter_reset(channel);

	/* use audio pattern to run the test */
	dramc_engine2(channel, TE_OP_READ_CHECK, DQS_GW_PATTERN2,
		      DQS_GW_PATTERN1 | DQS_GW_TE_OFFSET, 1, 0);

	/* get coarse result of DQS0, 1, 2, 3 */
	coarse_result01 = read32(&ch[channel].nao_regs->dqsgnwcnt[0]);
	coarse_result23 = read32(&ch[channel].nao_regs->dqsgnwcnt[1]);

	if (coarse_result01 == DQS_GW_GOLD_COUNTER_32BIT &&
	    coarse_result23 == DQS_GW_GOLD_COUNTER_32BIT)
		return 1;

	return 0;
}

static u8 dqs_gw_fine_tune_calib(u32 channel, u8 fine_val)
{
	u8 i, opt_fine_val;
	s8 delta[7] = {-48, -32, -16, 0, 16, 32, 48};
	int matches = 0, sum = 0;

	/* fine tune range from 0 to 127 */
	fine_val = MIN(MAX(fine_val, 0 - delta[0]), 127 - delta[6]);

	/* test gw fine tune */
	for (i = 0; i < ARRAY_SIZE(delta); i++) {
		opt_fine_val = fine_val + delta[i];
		set_gw_fine_factor(channel, opt_fine_val, 0);
		if (dqs_gw_test(channel)) {
			matches++;
			sum += delta[i];
		}
	}

	if (matches == 0) {
		die("[GW] ERROR, Fine-Tuning failed.\n");
	}

	opt_fine_val = fine_val + (sum / matches);

	return opt_fine_val;
}

static u8 dqs_gw_coarse_tune_calib(u32 channel, u8 coarse_val)
{
	u8 i, opt_coarse_val[3];
	s8 gw_ret[3], delta[3] = {0, 1, -1};

	for (i = 0; i < 3; i++) {
		/* adjust gw coarse tune value */
		opt_coarse_val[i] = coarse_val + delta[i];
		set_gw_coarse_factor(channel, opt_coarse_val[i]);
		/* get gw test result */
		gw_ret[i] = dqs_gw_test(channel);
		/* judge test result */
		if (gw_ret[i] != 0)
			return opt_coarse_val[i];
	}

	/* abnormal test result, set to default coarse tune value */
	printk(BIOS_ERR, "[GW] ERROR, No found coarse tune!!!\n");

	return coarse_val;
}

void rx_dqs_gating_cal(u32 channel, u8 rank,
		       const struct mt8173_sdram_params *sdram_params)
{
	u8 gw_coarse_val, gw_fine_val;

	/* disable HW gating */
	clrbits32(&ch[channel].ao_regs->dqscal0,
				  1 << DQSCAL0_STBCALEN_SHIFT);
	/* enable DQS gating window counter */
	setbits32(&ch[channel].ao_regs->dqsctl1,
				  1 << DQSCTL1_DQSIENMODE_SHIFT);
	setbits32(&ch[channel].ao_regs->spcmd,
				  1 << SPCMD_DQSGCNTEN_SHIFT);
	/* dual-phase DQS clock gating control enabling */
	setbits32(&ch[channel].ddrphy_regs->dqsgctl,
				  1 << DQSGCTL_DQSGDUALP_SHIFT);

	/* gating calibration value */
	gw_coarse_val = sdram_params->calib_params.gating_win[channel][rank][0];
	gw_fine_val = sdram_params->calib_params.gating_win[channel][rank][1];

	dramc_dbg("****************************************************\n");
	dramc_dbg("Channel %d Rank %d DQS GW Calibration\n", channel, rank);
	dramc_dbg("Default (coarse, fine) tune value %d, %d.\n",
		       gw_coarse_val, gw_fine_val);
	dramc_dbg("****************************************************\n");

	/* set default coarse and fine value */
	set_gw_coarse_factor(channel, gw_coarse_val);
	set_gw_fine_factor(channel, gw_fine_val, 0);

	/* adjust gw coarse tune */
	opt_gw_coarse_value[channel][rank] =
		dqs_gw_coarse_tune_calib(channel, gw_coarse_val);

	/* set adjusted gw coarse tune */
	set_gw_coarse_factor(channel, opt_gw_coarse_value[channel][rank]);

	/* adjust gw fine tune */
	opt_gw_fine_value[channel][rank] =
		dqs_gw_fine_tune_calib(channel, gw_fine_val);

	/* set adjusted gw fine tune */
	set_gw_fine_factor(channel, opt_gw_fine_value[channel][rank], 0);

	/* read data counter reset in PHY layer */
	dqs_gw_counter_reset(channel);

	/* gating window training result */
	printk(BIOS_INFO, "[GW] [Channel %d] [Rank %d] adjusted (coarse, fine) tune value: %d, %d.\n",
	       channel, rank, opt_gw_coarse_value[channel][rank],
	       opt_gw_fine_value[channel][rank]);
}

void dual_rank_rx_dqs_gating_cal(u32 channel,
				 const struct mt8173_sdram_params *sdram_params)
{
	u32 dqsinctl;

	/* rank 0 gw calibration */
	rx_dqs_gating_cal(channel, 0, sdram_params);

	/* get dqsinctl after rank 0 calibration */
	dqsinctl = read32(&ch[channel].ao_regs->dqsctl1);
	dqsinctl = (dqsinctl >> DQSCTL1_DQSINCTL_SHIFT) & (0xf << 0);

	/* swap cs0 and cs1 */
	setbits32(&ch[channel].ao_regs->rkcfg, MASK_RKCFG_RKSWAP_EN);

	/* rank 1 gw calibration */
	rx_dqs_gating_cal(channel, 1, sdram_params);

	/* set rank 1 coarse tune and fine tune */
	set_gw_coarse_factor_rank1(channel, opt_gw_coarse_value[channel][1],
				   dqsinctl);
	set_gw_fine_factor(channel, opt_gw_fine_value[channel][1], 1);

	/* swap cs back */
	clrbits32(&ch[channel].ao_regs->rkcfg, MASK_RKCFG_RKSWAP_EN);

	/* set rank 0 coarse tune and fine tune back */
	set_gw_coarse_factor(channel, opt_gw_coarse_value[channel][0]);
	set_gw_fine_factor(channel, opt_gw_fine_value[channel][0], 0);
}

void dramc_rankinctl_config(u32 channel,
			    const struct mt8173_sdram_params *sdram_params)
{
	u32 value;

	if (is_dual_rank(channel, sdram_params)) {
		/* RANKINCTL_ROOT1 = DQSINCTL + reg_TX_DLY_DQSGATE */
		value = MIN(opt_gw_coarse_value[channel][0],
			    opt_gw_coarse_value[channel][1]) >> 2;

		clrsetbits32(&ch[channel].ao_regs->dummy, 0xf, value);

		/* RANKINCTL = RANKINCTL_ROOT1 */
		clrsetbits32(&ch[channel].ao_regs->dqscal1,
			     0xf << 16, value << 16);
	}
	/* disable per-bank refresh when refresh rate >= 5 */
	setbits32(&ch[channel].ao_regs->rkcfg,
		  1 << RKCFG_PBREF_DISBYRATE_SHIFT);
}

u32 dram_k_perbit(u32 channel)
{
	u32 err_value = 0x0;

	/* use XTALK pattern to run the test */
	err_value = dramc_engine2(channel, TE_OP_WRITE_READ_CHECK,
				  DEFAULT_TEST2_1_CAL, DEFAULT_TEST2_2_CAL,
				  2, 0);
	return err_value;
}

void dramk_check_dqs_win(struct dqs_perbit_dly *p, u8 dly_step, u8 last_step,
			 u32 fail_bit)
{
	s8 dqsdly_pass_win, best_pass_win;

	if (fail_bit == 0) {
		if (p->first_dqsdly_pass == -1) {
			/* first DQS pass delay tap */
			p->first_dqsdly_pass = dly_step;
		}
		if ((p->last_dqsdly_pass == -2) && (dly_step == last_step)) {
			/* pass to the last tap */
			p->last_dqsdly_pass = dly_step;
			dqsdly_pass_win = p->last_dqsdly_pass -
					  p->first_dqsdly_pass;
			best_pass_win = p->best_last_dqsdly_pass -
					p->best_first_dqsdly_pass;
			if (dqsdly_pass_win > best_pass_win) {
				p->best_last_dqsdly_pass =  p->last_dqsdly_pass;
				p->best_first_dqsdly_pass = p->first_dqsdly_pass;
			}
			/* clear to find the next pass range if it has */
			p->first_dqsdly_pass = -1;
			p->last_dqsdly_pass = -2;
		}
	} else {
		if ((p->first_dqsdly_pass != -1) && (p->last_dqsdly_pass == -2)) {
			p->last_dqsdly_pass = dly_step - 1;
			dqsdly_pass_win = p->last_dqsdly_pass -
					  p->first_dqsdly_pass;
			best_pass_win = p->best_last_dqsdly_pass -
					p->best_first_dqsdly_pass;
			if (dqsdly_pass_win > best_pass_win) {
				p->best_last_dqsdly_pass =  p->last_dqsdly_pass;
				p->best_first_dqsdly_pass = p->first_dqsdly_pass;
			}
			/* clear to find the next pass range if it has */
			p->first_dqsdly_pass = -1;
			p->last_dqsdly_pass = -2;
		}
	}
}

void dramk_check_dq_win(struct dqs_perbit_dly *p, u8 dly_step, u8 last_step,
			u32 fail_bit)
{
	s8 dqdly_pass_win, best_pass_win;

	if (fail_bit == 0) {
		if (p->first_dqdly_pass == -1) {
			/* first DQ pass delay tap */
			p->first_dqdly_pass = dly_step;
		}

		if ((p->last_dqdly_pass == -2) && (dly_step == last_step)) {
			/* pass to the last tap */
			p->last_dqdly_pass = dly_step;
			dqdly_pass_win = p->last_dqdly_pass -
					 p->first_dqdly_pass;
			best_pass_win = p->best_last_dqdly_pass -
					p->best_first_dqdly_pass;
			if (dqdly_pass_win > best_pass_win) {
				p->best_last_dqdly_pass =  p->last_dqdly_pass;
				p->best_first_dqdly_pass = p->first_dqdly_pass;
			}
			/* clear to find the next pass range if it has */
			p->first_dqdly_pass = -1;
			p->last_dqdly_pass = -2;
		}
	} else {
		if ((p->first_dqdly_pass != -1) && (p->last_dqdly_pass == -2)) {
			p->last_dqdly_pass = dly_step - 1;
			dqdly_pass_win = p->last_dqdly_pass -
					 p->first_dqdly_pass;
			best_pass_win = p->best_last_dqdly_pass -
					p->best_first_dqdly_pass;
			if (dqdly_pass_win > best_pass_win) {
				p->best_last_dqdly_pass =  p->last_dqdly_pass;
				p->best_first_dqdly_pass = p->first_dqdly_pass;
			}
			/* clear to find the next pass range if it has */
			p->first_dqdly_pass = -1;
			p->last_dqdly_pass = -2;
		}
	}
}

u8 dramk_calcu_best_dly(u8 bit, struct dqs_perbit_dly *p, u8 *p_max_byte)
{
	u8 fail = 0;
	u8 hold, setup;

	/* hold time = DQS pass taps */
	hold = p->best_last_dqsdly_pass - p->best_first_dqsdly_pass + 1;
	/* setup time = DQ pass taps */
	setup = p->best_last_dqdly_pass - p->best_first_dqdly_pass + 1;

	/* The relationship of setup and hold time of dqs and dq signals
	 * is represented with delay tap in the following format:
	 *
	 *   setup time(dq delay)  hold time(dqs delay)
	 *   xxxxxxxxxxxxxoooooooo|ooooooooooooooooooooxxxxx
	 *   15		         0 1		      15 tap
	 */

	if (hold > setup) {
		/* like this: (setup time != 0) */
		/* xxxxxxxxxxxxxoooooooo|ooooooooooooooooooooxxxxx */
		/* like this: (setup time == 0) */
		/* xxxxxxxxxxxxxxxxxxxxx|xxxooooooooooxxxxxxxxxxxx */

		p->best_dqdly = 0;
		p->best_dqsdly = (setup != 0)? (hold - setup) / 2:
				 (hold - setup) / 2 + p->best_first_dqsdly_pass;

		if (p->best_dqsdly > *p_max_byte) {
			*p_max_byte = p->best_dqsdly;
		}

	} else if (hold < setup) {
		/* like this: (hold time != 0)*/
		/* xxxoooooooooooooooooo|ooooooooxxxxxxxxxxxxxxxxx */
		/* like this: (hold time == 0) */
		/* xxxoooooooooooooooxxx|xxxxxxxxxxxxxxxxxxxxxxxxx */

		p->best_dqsdly = 0;
		p->best_dqdly = (hold != 0)? (setup - hold) / 2:
				(setup - hold) / 2 + p->best_first_dqdly_pass;

	} else { /* hold time == setup time */
		p->best_dqsdly = 0;
		p->best_dqdly = 0;

		if (hold == 0) {
			/* like this: (mean this bit is error) */
			/* xxxxxxxxxxxxxxxxxxxxx|xxxxxxxxxxxxxxxxxxxxxxxxx */
			printk(BIOS_ERR, "ERROR, error bit %d, "
					 "setup_time = hold_time = 0!!\n", bit);
			fail = 1;
		}
	}

	dramc_dbg("bit#%d : dq =%d dqs=%d win=%d (%d, %d)\n",
		      bit, setup, hold, setup + hold,
		      p->best_dqdly, p->best_dqsdly);

	return fail;
}

void clk_duty_cal(u32 channel)
{
	u8  max_duty_sel, max_duty;
	u32 max_win_size = 0;

	max_duty_sel = max_duty = 1;

	clrsetbits32(&ch[channel].ddrphy_regs->phyclkduty,
		     0x3 << PHYCLKDUTY_CMDCLKP0DUTYN_SHIFT |
		     1 << PHYCLKDUTY_CMDCLKP0DUTYP_SHIFT,
		     1 << PHYCLKDUTY_CMDCLKP0DUTYSEL_SHIFT |
		     max_duty << PHYCLKDUTY_CMDCLKP0DUTYN_SHIFT);

	max_win_size = read32(&ch[channel].ddrphy_regs->phyclkduty);

	dramc_dbg("[Channel %d CLK DUTY CALIB] ", channel);
	dramc_dbg("Final DUTY_SEL=%d, DUTY=%d, rx window size=%d\n",
		      max_duty_sel, max_duty, max_win_size);
}

static void set_dle_factor(u32 channel, u8 curr_val)
{
	clrsetbits32(&ch[channel].ao_regs->ddr2ctl,
		     0x7 << DDR2CTL_DATLAT_SHIFT,
		     (curr_val & 0x7) << DDR2CTL_DATLAT_SHIFT);

	clrsetbits32(&ch[channel].ao_regs->padctl4,
		     0x1 << PADCTL4_DATLAT3_SHIFT,
		     ((curr_val >> 3) & 0x1) << PADCTL4_DATLAT3_SHIFT);

	clrsetbits32(&ch[channel].ao_regs->phyctl1,
		     0x1 << PHYCTL1_DATLAT4_SHIFT,
		     ((curr_val >> 4) & 0x1) << PHYCTL1_DATLAT4_SHIFT);

	clrsetbits32(&ch[channel].ao_regs->misc,
		     0x1f << MISC_DATLAT_DSEL_SHIFT,
		     (curr_val - 8) << MISC_DATLAT_DSEL_SHIFT);

	/* optimize bandwidth for HW run time test engine use */
	clrsetbits32(&ch[channel].ao_regs->misc,
		     0x1f << MISC_LATNORMP_SHIFT,
		     (curr_val - 3) << MISC_LATNORMP_SHIFT);
}

void dual_rank_rx_datlat_cal(u32 channel,
			     const struct mt8173_sdram_params *sdram_params)
{
	u8 r0_dle_setting, r1_dle_setting;

	/* rank 0 dle calibration */
	r0_dle_setting = rx_datlat_cal(channel, 0, sdram_params);

	/* swap cs0 and cs1 */
	setbits32(&ch[channel].ao_regs->rkcfg, MASK_RKCFG_RKSWAP_EN);

	/* set rank 1 coarse tune and fine tune back */
	set_gw_coarse_factor(channel, opt_gw_coarse_value[channel][1]);
	set_gw_fine_factor(channel, opt_gw_fine_value[channel][1], 0);

	/* rank 1 dle calibration */
	r1_dle_setting = rx_datlat_cal(channel, 1, sdram_params);

	/* set rank 0 coarse tune and fine tune back */
	set_gw_coarse_factor(channel, opt_gw_coarse_value[channel][0]);
	set_gw_fine_factor(channel, opt_gw_fine_value[channel][0], 0);

	/* swap cs back */
	clrbits32(&ch[channel].ao_regs->rkcfg, MASK_RKCFG_RKSWAP_EN);

	/* output dle setting of rank 0 and 1 */
	dramc_dbg("[DLE] Rank 0 DLE calibrated setting = %xh.\n"
		      "[DLE] Rank 1 DLE calibrated setting = %xh.\n",
		      r0_dle_setting, r1_dle_setting);

	if (r1_dle_setting < r0_dle_setting) {
		/* compare dle setting of two ranks */
		dramc_dbg("[DLE] rank 0 > rank 1. set to rank 0.\n");
		/* case 1: set rank 0 dle setting */
		set_dle_factor(channel, r0_dle_setting);
	} else {
		/* compare dle setting of two ranks */
		dramc_dbg("[DLE] rank 0 < rank 1. use rank 1.\n");
		/* case 2: set rank 1 dle setting */
		set_dle_factor(channel, r1_dle_setting);
	}
}

u8 rx_datlat_cal(u32 channel, u8 rank,
		 const struct mt8173_sdram_params *sdram_params)
{
	u8 i, best_step;
	u32 err[DLE_TEST_NUM];

	dramc_dbg("=========================================\n");
	dramc_dbg("[Channel %d] [Rank %d] DATLAT calibration\n",
		       channel, rank);
	dramc_dbg("=========================================\n");

	clrbits32(&ch[channel].ao_regs->mckdly,
		     0x11 << MCKDLY_DQIENQKEND_SHIFT |
		     0x1  << MCKDLY_DQIENLAT_SHIFT);

	/* set dle calibration initial value */
	best_step = sdram_params->calib_params.datlat_ucfirst + 1;

	/* do dle calibration test */
	for (i = 0; i < DLE_TEST_NUM; i++) {
		set_dle_factor(channel, best_step - i);
		err[i] = dramc_engine2(channel, TE_OP_WRITE_READ_CHECK,
				       DEFAULT_TEST2_1_CAL,
				       DEFAULT_TEST2_2_CAL, 2, 0);
	}

	if (err[0]) {
		/* dle test error */
		printk(BIOS_ERR, "[DLE] CH:%d calibration ERROR CMP_ERR =%xh,\n",
			channel, err[0]);
	} else {
		/* judge dle test result */
		for (i = 0; i < DLE_TEST_NUM; i++) {
			if (!err[i] && (i + 1 == DLE_TEST_NUM || err[i + 1])) {
				/* dle test ok */
				best_step -= (i - 1);
				break;
			}
		}
	}

	/* Default dle value is set when test error (error recovery).
	 * Others, adjusted dle calibration value is set normally.
	 */
	set_dle_factor(channel, best_step);

	dramc_dbg("[DLE] adjusted value = %#x\n", best_step);

	return best_step;
}

void tx_delay_for_wrleveling(u32 channel,
			     struct dqs_perbit_dly *dqdqs_perbit_dly,
			     u8 *max_dqsdly_byte, u8 *ave_dqdly_byte)
{
	s8 i, delta, index, max_taps;

	max_taps = MAX_DQDLY_TAPS - 1;

	for (i = 0; i < DATA_WIDTH_32BIT; i++) {

		index = i / DQS_BIT_NUMBER;

		if (i % DQS_BIT_NUMBER == 0)
			dramc_dbg("DQS%d: %d\n", index,
				       wrlevel_dqs_dly[channel][index]);

		if (max_dqsdly_byte[index] <= wrlevel_dqs_dly[channel][index]) {
			/* set diff value (delta) */
			delta = wrlevel_dqs_dly[channel][index] -
				max_dqsdly_byte[index];

			dqdqs_perbit_dly[i].best_dqdly += delta;

			/* max limit to 15 */
			if (dqdqs_perbit_dly[i].best_dqdly > max_taps)
				dqdqs_perbit_dly[i].best_dqdly = max_taps;

			if ((i + 1) % DQS_BIT_NUMBER == 0) {
				/* DQS */
				max_dqsdly_byte[index] =
					wrlevel_dqs_dly[channel][index];
				/* DQM */
				ave_dqdly_byte[index] += delta;
				/* max limit to 15 */
				if (ave_dqdly_byte[index] > max_taps)
					ave_dqdly_byte[index] = max_taps;
			}

		} else if (i % DQS_BIT_NUMBER == 0) {
			/* max_dqsdly_byte[j] > wrlevel_dqs_dly[channel][j]
			 * Originally, we should move clk and CA delay.
			 * Then, do GW calibration again. However, DQ/DQS
			 * skew should not be large in MT8173, so we sacrifice
			 * the Clk/DQS margin by keeping the clk out delay.
			 */
			printk(BIOS_ERR, "[Warning] DQSO %d in TX "
					 "per-bit = %d > DQSO %d in WL = %d  ",
					 index, max_dqsdly_byte[index], index,
					 wrlevel_dqs_dly[channel][index]);
		}
	}
}

static void set_rx_dly_factor(u32 channel, u32 curr_val, u8 type)
{
	u32 i, value = 0;

	for (i = 0; i < DQS_NUMBER; i++)
		value += (curr_val << (8 * i));

	switch (type) {
	case RX_DQS:
		write32(&ch[channel].ao_regs->r0deldly, value);
		break;
	case RX_DQ:
		for (i = 0; i < DATA_WIDTH_32BIT; i += 4)
			write32(&ch[channel].ao_regs->dqidly[i/4], value);
		break;
	}
}

static void set_tx_dly_factor(u32 channel, u32 curr_val, u8 type)
{
	u32 i, bit_num, value = 0;

	bit_num = (type == TX_DQ)? DQS_BIT_NUMBER: DQS_NUMBER;

	for (i = 0; i < bit_num; i++)
		value += (curr_val << (4 * i));

	switch (type) {

	case TX_DQS:
		write32(&ch[channel].ddrphy_regs->padctl3, value);
		break;
	case TX_DQM:
		write32(&ch[channel].ddrphy_regs->padctl2, value);
		break;
	case TX_DQ:
		for (i = 0; i < DQS_NUMBER; i++)
			write32(&ch[channel].ddrphy_regs->dqodly[i], value);
		break;
	}
}

static void set_dly_factor(u32 channel, u8 stage, u8 type, u8 dly)
{
	switch (stage | type << 1) {
	/* set delay for DQ/DQM/DQS by setup/hold stage and window type */
	case STAGE_SETUP_TX_WIN:
		/* set DQ/DQM delay for tx window */
		set_tx_dly_factor(channel, dly, TX_DQ);
		set_tx_dly_factor(channel, dly, TX_DQM);
		break;
	case STAGE_SETUP_RX_WIN:
		/* set DQ delay for rx window */
		set_rx_dly_factor(channel, dly, RX_DQ);
		break;
	case STAGE_HOLD_TX_WIN:
		/* set DQS delay for tx window */
		set_tx_dly_factor(channel, dly, TX_DQS);
		break;
	case STAGE_HOLD_RX_WIN:
		/* set DQS delay for rx window */
		set_rx_dly_factor(channel, dly, RX_DQS);
		break;
	}
}

static void set_rx_best_dly_factor(u32 channel,
				   struct dqs_perbit_dly *dqdqs_perbit_dly,
				   u8 *max_dqsdly_byte)
{
	u32 i, value = 0;

	for (i = 0; i < DQS_NUMBER; i++)
		value += (((u32)max_dqsdly_byte[i]) << (8 * i));

	write32(&ch[channel].ao_regs->r0deldly, value);
	write32(&ch[channel].ao_regs->r1deldly, value);

	dramc_dbg("[RX] DQS Reg R0DELDLY=%xh\n",
			read32(&ch[channel].ao_regs->r0deldly));
	dramc_dbg("[RX] DQS Reg R1DELDLY=%xh\n",
			read32(&ch[channel].ao_regs->r1deldly));

	for (i = 0; i < DATA_WIDTH_32BIT; i += 4) {
		/* every 4bit dq have the same delay register address */
		value = ((u32)dqdqs_perbit_dly[i].best_dqdly) +
			(((u32)dqdqs_perbit_dly[i + 1].best_dqdly) << 8) +
			(((u32)dqdqs_perbit_dly[i + 2].best_dqdly) << 16) +
			(((u32)dqdqs_perbit_dly[i + 3].best_dqdly) << 24);

		write32(&ch[channel].ao_regs->dqidly[i / 4], value);
		dramc_dbg("[RX] DQ DQIDLY%d = %xh\n", (i + 4) / 4, value);
	}
}

static void set_tx_best_dly_factor(u32 channel,
				   struct dqs_perbit_dly *dqdqs_perbit_dly,
				   u8 *max_dqsdly_byte, u8 *ave_dqdly_byte)
{
	u32 bit, value, shift, dqs_index = 0;

	value = 0;
	for (bit = 0; bit < DQS_NUMBER; bit++) {
		value += (((u32)max_dqsdly_byte[bit]) << (4 * bit));
	}

	write32(&ch[channel].ddrphy_regs->padctl3, value);
	dramc_dbg("[TX] DQS PADCTL3 Reg = %#x\n", value);

	/* DQ delay */
	for (bit = 0; bit < DATA_WIDTH_32BIT; bit++) {
		/* every 8 DQ reset */
		if (bit % DQS_BIT_NUMBER == 0) {
			value = 0;
			dqs_index = bit / DQS_BIT_NUMBER;
		}
		/* 4 bits field for each DQ */
		shift = 4 * (bit % DQS_BIT_NUMBER);
		value += (((u32)(dqdqs_perbit_dly[bit].best_dqdly)) << shift);
		/* each register is with 8 DQ */
		if ((bit + 1) % DQS_BIT_NUMBER == 0) {
			write32(&ch[channel].ddrphy_regs->dqodly[dqs_index], value);
			dramc_dbg("[TX] DQ DQ0DLY%d = %xh\n",
					dqs_index + 1, value);
		}
	}

	/* DQM delay */
	value = read32(&ch[channel].ddrphy_regs->padctl2);
	value &= MASK_PADCTL2;

	for (bit = 0; bit < DQS_NUMBER; bit++) {
		value += (((u32)ave_dqdly_byte[bit]) << (4 * bit));
	}
	write32(&ch[channel].ddrphy_regs->padctl2, value);
	dramc_dbg("[TX] DQM PADCTL2 Reg = %#x\n", value);
}

void perbit_window_cal(u32 channel, u8 type)
{
	u8 i, dly, bit, max_dqs_taps, fail = 0;
	u8 max_dqsdly_byte[DQS_NUMBER], ave_dqdly_byte[DQS_NUMBER];
	u32 err_value, fail_bit, max_limit, index;

	struct dqs_perbit_dly dqdqs_perbit_dly[DQ_DATA_WIDTH];

	dramc_dbg("\n[Channel %d] %s DQ/DQS per bit :\n",
			channel, (type == TX_WIN)? "TX": "RX");

	if (type == TX_WIN)
		dramc_phy_reset(channel);

	for (i = 0; i < DATA_WIDTH_32BIT; i++) {
		dqdqs_perbit_dly[i].first_dqdly_pass = -1;
		dqdqs_perbit_dly[i].last_dqdly_pass = -2;
		dqdqs_perbit_dly[i].first_dqsdly_pass = -1;
		dqdqs_perbit_dly[i].last_dqsdly_pass = -2;
		dqdqs_perbit_dly[i].best_first_dqdly_pass = -1;
		dqdqs_perbit_dly[i].best_last_dqdly_pass = -2;
		dqdqs_perbit_dly[i].best_first_dqsdly_pass = -1;
		dqdqs_perbit_dly[i].best_last_dqsdly_pass = -2;
	}

	/* 1. delay DQ,find the pass window (left boundary)
	 * 2. delay DQS find the pass window (right boundary)
	 * 3. find the best DQ / DQS to satisfy the middle value
	 *    of the overall pass window per bit
	 * 4. set DQS delay to the max per byte, delay DQ to de-skew
	 */

	/* 1. set DQS delay to 0 first */
	set_dly_factor(channel, STAGE_HOLD, type, FIRST_DQS_DELAY);

	dramc_dbg("----------------------------------"
			"--------------------\n");
	dramc_dbg("Start DQ delay to find pass range,"
			"DQS delay fixed to %#x...\n", FIRST_DQS_DELAY);
	dramc_dbg("----------------------------------"
			"-------------------\n");
	dramc_dbg("x-axis is bit #; y-axis is DQ delay (%d~%d)\n",
			FIRST_DQ_DELAY, MAX_DQDLY_TAPS - 1);

	/* delay DQ from 0 to 15 to get the setup time */
	for (dly = FIRST_DQ_DELAY; dly < MAX_DQDLY_TAPS; dly++) {

		set_dly_factor(channel, STAGE_SETUP, type, dly);
		err_value = dram_k_perbit(channel);

		/* check fail bit, 0 ok, others fail */
		for (bit = 0; bit < DATA_WIDTH_32BIT; bit++) {
			fail_bit = err_value & ((u32)1 << bit);
			dramk_check_dq_win(&(dqdqs_perbit_dly[bit]), dly,
					   MAX_DQDLY_TAPS - 1, fail_bit);
			if (fail_bit == 0) {
				dramc_dbg("o");
			} else {
				dramc_dbg("x");
			}
		}
		dramc_dbg("\n");
	}

	/* 2. set DQ delay to 0 */
	set_dly_factor(channel, STAGE_SETUP, type, FIRST_DQ_DELAY);

	/* DQS delay taps: tx and rx are 16 and 64 taps */
	max_dqs_taps = (type == TX_WIN)? MAX_TX_DQSDLY_TAPS: MAX_RX_DQSDLY_TAPS;

	dramc_dbg("-----------------------------------"
			"-------------------\n");
	dramc_dbg("Start DQS delay to find pass range,"
			"DQ delay fixed to %#x...\n", FIRST_DQ_DELAY);
	dramc_dbg("------------------------------------"
			"------------------\n");
	dramc_dbg("x-axis is bit #; y-axis is DQS delay (%d~%d)\n",
		      FIRST_DQS_DELAY + 1, max_dqs_taps - 1);

	/* delay DQS to get the hold time, dq_dly = dqs_dly = 0 is counted */
	/* when we delay dq, so we set first dqs delay to 1 */
	for (dly = (FIRST_DQS_DELAY + 1); dly < max_dqs_taps; dly++) {

		set_dly_factor(channel, STAGE_HOLD, type, dly);
		err_value = dram_k_perbit(channel);

		/* check fail bit, 0 ok, others fail */
		for (bit = 0; bit < DATA_WIDTH_32BIT; bit++) {
			fail_bit = err_value & ((u32)1 << bit);
			dramk_check_dqs_win(&(dqdqs_perbit_dly[bit]), dly,
					    max_dqs_taps - 1, fail_bit);
			if (fail_bit == 0) {
				dramc_dbg("o");
			} else {
				dramc_dbg("x");
			}
		}
		dramc_dbg("\n");
	}

	/* 3 calculate dq and dqs time */
	dramc_dbg("-------------------------------"
			"-----------------------\n");
	dramc_dbg("Start calculate dq time and dqs "
			"time:\n");
	dramc_dbg("Find max DQS delay per byte / "
			"Adjust DQ delay to align DQS...\n");
	dramc_dbg("--------------------------------"
			"----------------------\n");

	/* As per byte, check max DQS delay in 8-bit.
	 * Except for the bit of max DQS delay, delay
	 * DQ to fulfill setup time = hold time
	 */
	for (i = 0; i < DQS_NUMBER; i++) {
		max_dqsdly_byte[i] = 0;
		ave_dqdly_byte[i] = 0;
	}

	for (i = 0; i < DATA_WIDTH_32BIT; i++) {
		/* we delay DQ or DQS to let DQS sample the middle */
		/* of tx/rx pass window for all the 8 bits */
		index = i / DQS_BIT_NUMBER;
		fail |= dramk_calcu_best_dly(i, &dqdqs_perbit_dly[i],
					     &max_dqsdly_byte[index]);

		if ((i + 1) % DQS_BIT_NUMBER == 0)
			dramc_dbg("----separate line----\n");
	}

	for (i = 0; i < DATA_WIDTH_32BIT; i++) {
		/* dqs index for every 8-bit */
		index = i / DQS_BIT_NUMBER;
		/* set DQS to max for 8-bit */
		if (dqdqs_perbit_dly[i].best_dqsdly < max_dqsdly_byte[index]) {
			/* delay DQ to compensate extra DQS delay */
			dly = max_dqsdly_byte[index] -
			      dqdqs_perbit_dly[i].best_dqsdly;
			dqdqs_perbit_dly[i].best_dqdly += dly;
			/* max limit to 15 */
			max_limit = MAX_DQDLY_TAPS - 1;
			if (dqdqs_perbit_dly[i].best_dqdly > max_limit) {
				dqdqs_perbit_dly[i].best_dqdly = max_limit;
			}
		}
		/* accumulation variable for TX DQM */
		ave_dqdly_byte[index] += dqdqs_perbit_dly[i].best_dqdly;
		/* take the average of DQ for TX DQM */
		if ((i + 1) % DQS_BIT_NUMBER == 0) {
			ave_dqdly_byte[index] /= DQS_BIT_NUMBER;
		}
	}

	if (fail == 1) /* error handling */
		die("fail on %s()\n", __func__);

	dramc_dbg("==================================================\n");
	dramc_dbg("        dramc_perbit_window_swcal:\n");
	dramc_dbg("           channel=%d(0:cha, 1:chb)\n", channel);
	dramc_dbg("           bus width=%d\n", DATA_WIDTH_32BIT);
	dramc_dbg("==================================================\n");
	dramc_dbg("DQS Delay :\n DQS0 = %d DQS1 = %d DQS2 = %d DQS3 = %d\n",
		       max_dqsdly_byte[0], max_dqsdly_byte[1],
		       max_dqsdly_byte[2], max_dqsdly_byte[3]);

	if (type == TX_WIN)
		dramc_dbg("DQM Delay :\n"
			      "DQM0 = %d DQM1 = %d DQM2 = %d DQM3 = %d\n",
			       ave_dqdly_byte[0], ave_dqdly_byte[1],
			       ave_dqdly_byte[2], ave_dqdly_byte[3]);

	dramc_dbg("DQ Delay :\n");
	for (i = 0; i < DATA_WIDTH_32BIT; i++) {
		dramc_dbg("DQ%d = %d ", i, dqdqs_perbit_dly[i].best_dqdly);
		if (((i + 1) % 4) == 0)
			dramc_dbg("\n");
	}

	dramc_dbg("____________________________________"
		      "____________________________________\n");

	if (type == TX_WIN) {
		/* Add CLK to DQS/DQ skew after write leveling */
		dramc_dbg("Add CLK to DQS/DQ skew based on write leveling.\n");
		/* this subroutine add clk delay to DQS/DQ after WL */
		tx_delay_for_wrleveling(channel, dqdqs_perbit_dly,
					max_dqsdly_byte, ave_dqdly_byte);
	}

	if (type == TX_WIN)
		set_tx_best_dly_factor(channel, dqdqs_perbit_dly,
				       max_dqsdly_byte, ave_dqdly_byte);
	else
		set_rx_best_dly_factor(channel, dqdqs_perbit_dly,
				       max_dqsdly_byte);

	dramc_phy_reset(channel);
}
