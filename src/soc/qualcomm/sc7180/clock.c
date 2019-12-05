/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Qualcomm Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <commonlib/helpers.h>
#include <device/mmio.h>
#include <soc/clock.h>
#include <types.h>

#define DIV(div) (2 * div - 1)

struct clock_config qup_cfg[] = {
	{
		.hz = 7372800,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = DIV(1),
		.m = 384,
		.n = 15625,
		.d_2 = 15625,
	},
	{
		.hz = SRC_XO_HZ,	/* 19.2KHz */
		.src = SRC_XO_19_2MHZ,
		.div = DIV(1),
	}
};

struct clock_config qspi_core_cfg[] = {
	{
		.hz = SRC_XO_HZ,	/* 19.2KHz */
		.src = SRC_XO_19_2MHZ,
		.div = DIV(1),
	},
	{
		.hz = 100 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = DIV(3),
	},
	{
		.hz = 150 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = DIV(2),
	},
	{
		.hz = GPLL0_EVEN_HZ,	/* 300MHz */
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = DIV(1),
	}
};

static int clock_configure_gpll0(void)
{
	setbits32(&gcc->gpll0.user_ctl_u, 1 << SCALE_FREQ_SHFT);

	/* Keep existing GPLL0 configuration, in RUN mode @600Mhz. */
	setbits32(&gcc->gpll0.user_ctl,
			1 << CLK_CTL_GPLL_PLLOUT_EVEN_SHFT |
			1 << CLK_CTL_GPLL_PLLOUT_MAIN_SHFT |
			1 << CLK_CTL_GPLL_PLLOUT_ODD_SHFT);

	return 0;
}

static int clock_configure_mnd(struct sc7180_clock *clk, uint32_t m, uint32_t n,
				uint32_t d_2)
{
	struct sc7180_mnd_clock *mnd = (struct sc7180_mnd_clock *)clk;
	setbits32(&clk->rcg_cfg,
			RCG_MODE_DUAL_EDGE << CLK_CTL_CFG_MODE_SHFT);

	write32(&mnd->m, m & CLK_CTL_RCG_MND_BMSK);
	write32(&mnd->n, ~(n-m) & CLK_CTL_RCG_MND_BMSK);
	write32(&mnd->d_2, ~(d_2) & CLK_CTL_RCG_MND_BMSK);

	return 0;
}

static int clock_configure(struct sc7180_clock *clk,
				struct clock_config *clk_cfg,
				uint32_t hz, uint32_t num_perfs)
{
	uint32_t reg_val;
	uint32_t idx;

	for (idx = 0; idx < num_perfs; idx++)
		if (hz <= clk_cfg[idx].hz)
			break;

	assert(hz == clk_cfg[idx].hz);

	reg_val = (clk_cfg[idx].src << CLK_CTL_CFG_SRC_SEL_SHFT) |
			(clk_cfg[idx].div << CLK_CTL_CFG_SRC_DIV_SHFT);

	/* Set clock config */
	write32(&clk->rcg_cfg, reg_val);

	if (clk_cfg[idx].m != 0)
		clock_configure_mnd(clk, clk_cfg[idx].m, clk_cfg[idx].n,
				clk_cfg[idx].d_2);

	/* Commit config to RCG*/
	setbits32(&clk->rcg_cmd, BIT(CLK_CTL_CMD_UPDATE_SHFT));

	return 0;
}

static bool clock_is_off(u32 *cbcr_addr)
{
	return (read32(cbcr_addr) & CLK_CTL_CBC_CLK_OFF_BMSK);
}

static int clock_enable_vote(void *cbcr_addr, void *vote_addr,
				uint32_t vote_bit)
{
	/* Set clock vote bit */
	setbits32(vote_addr, BIT(vote_bit));

	/* Ensure clock is enabled */
	while (clock_is_off(cbcr_addr))
		;

	return 0;
}

static int clock_enable(void *cbcr_addr)
{
	/* Set clock enable bit */
	setbits32(cbcr_addr, BIT(CLK_CTL_CBC_CLK_EN_SHFT));

	/* Ensure clock is enabled */
	while (clock_is_off(cbcr_addr))
		;

	return 0;
}

void clock_reset_aop(void)
{
	/* Bring AOP out of RESET */
	clrbits32(&aoss->aoss_cc_apcs_misc, BIT(AOP_RESET_SHFT));
}

void clock_configure_qspi(uint32_t hz)
{
	clock_configure(&gcc->qspi_core,
			qspi_core_cfg, hz,
			ARRAY_SIZE(qspi_core_cfg));
	clock_enable(&gcc->qspi_cnoc_ahb_cbcr);
	clock_enable(&gcc->qspi_core.cbcr);
}

int clock_reset_bcr(void *bcr_addr, bool reset)
{
	struct sc7180_bcr *bcr = bcr_addr;

	if (reset)
		setbits32(bcr, BIT(CLK_CTL_BCR_BLK_ARES_SHFT));
	else
		clrbits32(bcr, BIT(CLK_CTL_BCR_BLK_ARES_SHFT));

	return 0;
}

void clock_configure_qup(int qup, uint32_t hz)
{
	int s = qup % QUP_WRAP1_S0;
	struct sc7180_qupv3_clock *qup_clk = qup < QUP_WRAP1_S0 ?
				&gcc->qup_wrap0_s[s] : &gcc->qup_wrap1_s[s];

	clock_configure(&qup_clk->mnd_clk.clock, qup_cfg, hz,
							ARRAY_SIZE(qup_cfg));
}

void clock_enable_qup(int qup)
{
	int s = qup % QUP_WRAP1_S0;
	int clk_en_off = qup < QUP_WRAP1_S0 ?
			QUPV3_WRAP0_CLK_ENA_S(s) : QUPV3_WRAP1_CLK_ENA_S(s);
	struct sc7180_qupv3_clock *qup_clk = qup < QUP_WRAP1_S0 ?
				&gcc->qup_wrap0_s[s] : &gcc->qup_wrap1_s[s];

	clock_enable_vote(&qup_clk->mnd_clk, &gcc->apcs_clk_br_en1,
							clk_en_off);
}

void clock_init(void)
{
	clock_configure_gpll0();

	clock_enable_vote(&gcc->qup_wrap0_core_2x.cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP0_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap0_core_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP0_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap0_m_ahb_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP_0_M_AHB_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap0_s_ahb_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP_0_S_AHB_CLK_ENA);

	clock_enable_vote(&gcc->qup_wrap1_core_2x_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP1_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_core_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP1_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_m_ahb_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP_1_M_AHB_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_s_ahb_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP_1_S_AHB_CLK_ENA);
}
