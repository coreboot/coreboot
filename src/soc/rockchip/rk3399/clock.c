/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
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
#include <console/console.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/grf.h>
#include <soc/soc.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct pll_div {
	u32 refdiv;
	u32 fbdiv;
	u32 postdiv1;
	u32 postdiv2;
	u32 frac;
};

#define PLL_DIVISORS(hz, _refdiv, _postdiv1, _postdiv2) {\
	.refdiv = _refdiv,\
	.fbdiv = (u32)((u64)hz * _refdiv * _postdiv1 * _postdiv2 / OSC_HZ),\
	.postdiv1 = _postdiv1, .postdiv2 = _postdiv2};\
	_Static_assert(((u64)hz * _refdiv * _postdiv1 * _postdiv2 / OSC_HZ) *\
			 OSC_HZ / (_refdiv * _postdiv1 * _postdiv2) == hz,\
			 #hz "Hz cannot be hit with PLL "\
			 "divisors on line " STRINGIFY(__LINE__))

static const struct pll_div gpll_init_cfg = PLL_DIVISORS(GPLL_HZ, 2, 2, 1);
static const struct pll_div cpll_init_cfg = PLL_DIVISORS(CPLL_HZ, 1, 2, 2);
static const struct pll_div ppll_init_cfg = PLL_DIVISORS(PPLL_HZ, 2, 2, 1);

static const struct pll_div apll_l_1600_cfg = PLL_DIVISORS(1600*MHz, 3, 1, 1);
static const struct pll_div apll_l_600_cfg = PLL_DIVISORS(600*MHz, 1, 2, 1);

static const struct pll_div *apll_l_cfgs[] = {
	[APLL_L_1600_MHZ] = &apll_l_1600_cfg,
	[APLL_L_600_MHZ] = &apll_l_600_cfg,
};

enum {
	/* PLL_CON0 */
	PLL_FBDIV_MASK			= 0xfff,
	PLL_FBDIV_SHIFT			= 0,

	/* PLL_CON1 */
	PLL_POSTDIV2_MASK		= 0x7,
	PLL_POSTDIV2_SHIFT		= 12,
	PLL_POSTDIV1_MASK		= 0x7,
	PLL_POSTDIV1_SHIFT		= 8,
	PLL_REFDIV_MASK			= 0x3f,
	PLL_REFDIV_SHIFT		= 0,

	/* PLL_CON2 */
	PLL_LOCK_STATUS_MASK		= 1,
	PLL_LOCK_STATUS_SHIFT		= 31,
	PLL_FRACDIV_MASK		= 0xffffff,
	PLL_FRACDIV_SHIFT		= 0,

	/* PLL_CON3 */
	PLL_MODE_MASK			= 3,
	PLL_MODE_SHIFT			= 8,
	PLL_MODE_SLOW			= 0,
	PLL_MODE_NORM,
	PLL_MODE_DEEP,
	PLL_DSMPD_MASK			= 1,
	PLL_DSMPD_SHIFT			= 3,
	PLL_INTEGER_MODE		= 1,

	/* PMUCRU_CLKSEL_CON0 */
	PMU_PCLK_DIV_CON_MASK		= 0x1f,
	PMU_PCLK_DIV_CON_SHIFT		= 0,

	/* CLKSEL_CON0 */
	ACLKM_CORE_L_DIV_CON_MASK	= 0x1f,
	ACLKM_CORE_L_DIV_CON_SHIFT	= 8,
	CLK_CORE_L_PLL_SEL_MASK		= 3,
	CLK_CORE_L_PLL_SEL_SHIFT	= 6,
	CLK_CORE_L_PLL_SEL_ALPLL	= 0x0,
	CLK_CORE_L_PLL_SEL_ABPLL	= 0x1,
	CLK_CORE_L_PLL_SEL_DPLL		= 0x10,
	CLK_CORE_L_PLL_SEL_GPLL		= 0x11,
	CLK_CORE_L_DIV_MASK		= 0x1f,
	CLK_CORE_L_DIV_SHIFT		= 0,

	/* CLKSEL_CON1 */
	PCLK_DBG_L_DIV_MASK		= 0x1f,
	PCLK_DBG_L_DIV_SHIFT		= 0x8,
	ATCLK_CORE_L_DIV_MASK		= 0x1f,
	ATCLK_CORE_L_DIV_SHIFT		= 0,

	/* CLKSEL_CON14 */
	PCLK_PERIHP_DIV_CON_MASK	= 0x7,
	PCLK_PERIHP_DIV_CON_SHIFT	= 12,
	HCLK_PERIHP_DIV_CON_MASK	= 3,
	HCLK_PERIHP_DIV_CON_SHIFT	= 8,
	ACLK_PERIHP_PLL_SEL_MASK	= 1,
	ACLK_PERIHP_PLL_SEL_SHIFT	= 7,
	ACLK_PERIHP_PLL_SEL_CPLL	= 0,
	ACLK_PERIHP_PLL_SEL_GPLL	= 1,
	ACLK_PERIHP_DIV_CON_MASK	= 0x1f,
	ACLK_PERIHP_DIV_CON_SHIFT	= 0,

	/* CLKSEL_CON23 */
	PCLK_PERILP0_DIV_CON_MASK	= 0x7,
	PCLK_PERILP0_DIV_CON_SHIFT	= 12,
	HCLK_PERILP0_DIV_CON_MASK	= 3,
	HCLK_PERILP0_DIV_CON_SHIFT	= 8,
	ACLK_PERILP0_PLL_SEL_MASK	= 1,
	ACLK_PERILP0_PLL_SEL_SHIFT	= 7,
	ACLK_PERILP0_PLL_SEL_CPLL	= 0,
	ACLK_PERILP0_PLL_SEL_GPLL	= 1,
	ACLK_PERILP0_DIV_CON_MASK	= 0x1f,
	ACLK_PERILP0_DIV_CON_SHIFT	= 0,

	/* CLKSEL_CON25 */
	PCLK_PERILP1_DIV_CON_MASK	= 0x7,
	PCLK_PERILP1_DIV_CON_SHIFT	= 8,
	HCLK_PERILP1_PLL_SEL_MASK	= 1,
	HCLK_PERILP1_PLL_SEL_SHIFT	= 7,
	HCLK_PERILP1_PLL_SEL_CPLL	= 0,
	HCLK_PERILP1_PLL_SEL_GPLL	= 1,
	HCLK_PERILP1_DIV_CON_MASK	= 0x1f,
	HCLK_PERILP1_DIV_CON_SHIFT	= 0,
};

#define VCO_MAX_KHZ	(3200 * (MHz / KHz))
#define VCO_MIN_KHZ	(800 * (MHz / KHz))
#define OUTPUT_MAX_KHZ	(3200 * (MHz / KHz))
#define OUTPUT_MIN_KHZ	(16 * (MHz / KHz))

/* the div restrictions of pll in integer mode,
 * these are defined in * CRU_*PLL_CON0 or PMUCRU_*PLL_CON0
 */
#define PLL_DIV_MIN	16
#define PLL_DIV_MAX	3200

/* How to calculate the PLL(from TRM V0.3 Part 1 Page 63):
 * Formulas also embedded within the Fractional PLL Verilog model:
 * If DSMPD = 1 (DSM is disabled, "integer mode")
 * FOUTVCO = FREF / REFDIV * FBDIV
 * FOUTPOSTDIV = FOUTVCO / POSTDIV1 / POSTDIV2
 * Where:
 * FOUTVCO = Fractional PLL non-divided output frequency
 * FOUTPOSTDIV = Fractional PLL divided output frequency
 *               (output of second post divider)
 * FREF = Fractional PLL input reference frequency, (the OSC_HZ 24MHz input)
 * REFDIV = Fractional PLL input reference clock divider
 * FBDIV = Integer value programmed into feedback divide
 *
 */
static void rkclk_set_pll(u32 *pll_con, const struct pll_div *div)
{
	/* All 8 PLLs have same VCO and output frequency range restrictions. */
	u32 vco_khz = OSC_HZ / 1000 * div->fbdiv / div->refdiv;
	u32 output_khz = vco_khz / div->postdiv1 / div->postdiv2;

	printk(BIOS_DEBUG, "PLL at %p: fbdiv=%d, refdiv=%d, postdiv1=%d, "
			   "postdiv2=%d, vco=%u khz, output=%u khz\n",
			   pll_con, div->fbdiv, div->refdiv, div->postdiv1,
			   div->postdiv2, vco_khz, output_khz);
	assert(vco_khz >= VCO_MIN_KHZ && vco_khz <= VCO_MAX_KHZ &&
	       output_khz >= OUTPUT_MIN_KHZ && output_khz <= OUTPUT_MAX_KHZ &&
	       div->fbdiv >= PLL_DIV_MIN && div->fbdiv <= PLL_DIV_MAX);

	/* When power on or changing PLL setting,
	 * we must force PLL into slow mode to ensure output stable clock.
	 */
	write32(&pll_con[3], RK_CLRSETBITS(PLL_MODE_MASK << PLL_MODE_SHIFT,
					   PLL_MODE_SLOW << PLL_MODE_SHIFT));

	/* use integer mode */
	write32(&pll_con[3],
		RK_CLRSETBITS(PLL_DSMPD_MASK << PLL_DSMPD_SHIFT,
			      PLL_INTEGER_MODE << PLL_DSMPD_SHIFT));

	write32(&pll_con[0], RK_CLRSETBITS(PLL_FBDIV_MASK << PLL_FBDIV_SHIFT,
					   div->fbdiv << PLL_FBDIV_SHIFT));
	write32(&pll_con[1],
		RK_CLRSETBITS(PLL_POSTDIV2_MASK << PLL_POSTDIV2_SHIFT |
			      PLL_POSTDIV1_MASK << PLL_POSTDIV1_SHIFT |
			      PLL_REFDIV_MASK | PLL_REFDIV_SHIFT,
			      (div->postdiv2 << PLL_POSTDIV2_SHIFT) |
			      (div->postdiv1 << PLL_POSTDIV1_SHIFT) |
			      (div->refdiv << PLL_REFDIV_SHIFT)));

	/* waiting for pll lock */
	while (!(read32(&pll_con[2]) & (1 << PLL_LOCK_STATUS_SHIFT)))
		udelay(1);

	/* pll enter normal mode */
	write32(&pll_con[3], RK_CLRSETBITS(PLL_MODE_MASK << PLL_MODE_SHIFT,
					   PLL_MODE_NORM << PLL_MODE_SHIFT));
}

void rkclk_init(void)
{
	u32 aclk_div;
	u32 hclk_div;
	u32 pclk_div;

	/* some cru registers changed by bootrom, we'd better reset them to
	 * reset/default values described in TRM to avoid confusion in kernel.
	 * Please consider these threee lines as a fix of bootrom bug.
	 */
	write32(&cru_ptr->clksel_con[12], 0xffff4101);
	write32(&cru_ptr->clksel_con[19], 0xffff033f);
	write32(&cru_ptr->clksel_con[56], 0x00030003);

	/* configure pmu pll(ppll) */
	rkclk_set_pll(&pmucru_ptr->ppll_con[0], &ppll_init_cfg);

	/* configure pmu pclk */
	pclk_div = PPLL_HZ / PMU_PCLK_HZ - 1;
	assert((pclk_div + 1) * PMU_PCLK_HZ == PPLL_HZ && pclk_div < 0x1f);
	write32(&pmucru_ptr->pmucru_clksel[0],
		RK_CLRSETBITS(PMU_PCLK_DIV_CON_MASK << PMU_PCLK_DIV_CON_SHIFT,
			      pclk_div << PMU_PCLK_DIV_CON_SHIFT));

	/* configure gpll cpll */
	rkclk_set_pll(&cru_ptr->gpll_con[0], &gpll_init_cfg);
	rkclk_set_pll(&cru_ptr->cpll_con[0], &cpll_init_cfg);

	/* configure perihp aclk, hclk, pclk */
	aclk_div = GPLL_HZ / PERIHP_ACLK_HZ - 1;
	assert((aclk_div + 1) * PERIHP_ACLK_HZ == GPLL_HZ && aclk_div < 0x1f);

	hclk_div = PERIHP_ACLK_HZ / PERIHP_HCLK_HZ - 1;
	assert((hclk_div + 1) * PERIHP_HCLK_HZ ==
	       PERIHP_ACLK_HZ && (hclk_div < 0x4));

	pclk_div = PERIHP_ACLK_HZ / PERIHP_PCLK_HZ - 1;
	assert((pclk_div + 1) * PERIHP_PCLK_HZ ==
	       PERIHP_ACLK_HZ && (pclk_div < 0x7));

	write32(&cru_ptr->clksel_con[14],
		RK_CLRSETBITS(PCLK_PERIHP_DIV_CON_MASK <<
						PCLK_PERIHP_DIV_CON_SHIFT |
			      HCLK_PERIHP_DIV_CON_MASK <<
						HCLK_PERIHP_DIV_CON_SHIFT |
			      ACLK_PERIHP_PLL_SEL_MASK <<
						ACLK_PERIHP_PLL_SEL_SHIFT |
			      ACLK_PERIHP_DIV_CON_MASK <<
						ACLK_PERIHP_DIV_CON_SHIFT,
			      pclk_div << PCLK_PERIHP_DIV_CON_SHIFT |
			      hclk_div << HCLK_PERIHP_DIV_CON_SHIFT |
			      ACLK_PERIHP_PLL_SEL_GPLL <<
						ACLK_PERIHP_PLL_SEL_SHIFT |
			      aclk_div << ACLK_PERIHP_DIV_CON_SHIFT));

	/* configure perilp0 aclk, hclk, pclk */
	aclk_div = GPLL_HZ / PERILP0_ACLK_HZ - 1;
	assert((aclk_div + 1) * PERILP0_ACLK_HZ == GPLL_HZ && aclk_div < 0x1f);

	hclk_div = PERILP0_ACLK_HZ / PERILP0_HCLK_HZ - 1;
	assert((hclk_div + 1) * PERILP0_HCLK_HZ ==
	       PERILP0_ACLK_HZ && (hclk_div < 0x4));

	pclk_div = PERILP0_ACLK_HZ / PERILP0_PCLK_HZ - 1;
	assert((pclk_div + 1) * PERILP0_PCLK_HZ ==
	       PERILP0_ACLK_HZ && (pclk_div < 0x7));

	write32(&cru_ptr->clksel_con[23],
		RK_CLRSETBITS(PCLK_PERILP0_DIV_CON_MASK <<
						PCLK_PERILP0_DIV_CON_SHIFT |
			      HCLK_PERILP0_DIV_CON_MASK <<
						HCLK_PERILP0_DIV_CON_SHIFT |
			      ACLK_PERILP0_PLL_SEL_MASK <<
						ACLK_PERILP0_PLL_SEL_SHIFT |
			      ACLK_PERILP0_DIV_CON_MASK <<
						ACLK_PERILP0_DIV_CON_SHIFT,
			      pclk_div << PCLK_PERILP0_DIV_CON_SHIFT |
			      hclk_div << HCLK_PERILP0_DIV_CON_SHIFT |
			      ACLK_PERILP0_PLL_SEL_GPLL <<
						ACLK_PERILP0_PLL_SEL_SHIFT |
			      aclk_div << ACLK_PERILP0_DIV_CON_SHIFT));

	/* perilp1 hclk select gpll as source */
	hclk_div = GPLL_HZ / PERILP1_HCLK_HZ - 1;
	assert((hclk_div + 1) * PERILP1_HCLK_HZ ==
	       GPLL_HZ && (hclk_div < 0x1f));

	pclk_div = PERILP1_HCLK_HZ / PERILP1_HCLK_HZ - 1;
	assert((pclk_div + 1) * PERILP1_HCLK_HZ ==
	       PERILP1_HCLK_HZ && (hclk_div < 0x7));

	write32(&cru_ptr->clksel_con[25],
		RK_CLRSETBITS(PCLK_PERILP1_DIV_CON_MASK <<
						PCLK_PERILP1_DIV_CON_SHIFT |
			      HCLK_PERILP1_DIV_CON_MASK <<
						HCLK_PERILP1_DIV_CON_SHIFT |
			      HCLK_PERILP1_PLL_SEL_MASK <<
						HCLK_PERILP1_PLL_SEL_SHIFT,
			      pclk_div << PCLK_PERILP1_DIV_CON_SHIFT |
			      hclk_div << HCLK_PERILP1_DIV_CON_SHIFT |
			      HCLK_PERILP1_PLL_SEL_GPLL <<
						HCLK_PERILP1_PLL_SEL_SHIFT));
}

void rkclk_configure_cpu(enum apll_l_frequencies apll_l_freq)
{
	u32 aclkm_div;
	u32 pclk_dbg_div;
	u32 atclk_div;

	rkclk_set_pll(&cru_ptr->apll_l_con[0], apll_l_cfgs[apll_l_freq]);

	aclkm_div = APLL_HZ / ACLKM_CORE_HZ - 1;
	assert((aclkm_div + 1) * ACLKM_CORE_HZ == APLL_HZ &&
	       aclkm_div < 0x1f);

	pclk_dbg_div = APLL_HZ / PCLK_DBG_HZ - 1;
	assert((pclk_dbg_div + 1) * PCLK_DBG_HZ == APLL_HZ &&
	       pclk_dbg_div < 0x1f);

	atclk_div = APLL_HZ / ATCLK_CORE_HZ - 1;
	assert((atclk_div + 1) * ATCLK_CORE_HZ == APLL_HZ &&
	       atclk_div < 0x1f);

	write32(&cru_ptr->clksel_con[0],
		RK_CLRSETBITS(ACLKM_CORE_L_DIV_CON_MASK <<
						ACLKM_CORE_L_DIV_CON_SHIFT |
			      CLK_CORE_L_PLL_SEL_MASK <<
						CLK_CORE_L_PLL_SEL_SHIFT |
			      CLK_CORE_L_DIV_MASK << CLK_CORE_L_DIV_SHIFT,
			      aclkm_div << ACLKM_CORE_L_DIV_CON_SHIFT |
			      CLK_CORE_L_PLL_SEL_ALPLL <<
						CLK_CORE_L_PLL_SEL_SHIFT |
			      0 << CLK_CORE_L_DIV_SHIFT));

	write32(&cru_ptr->clksel_con[1],
		RK_CLRSETBITS(PCLK_DBG_L_DIV_MASK << PCLK_DBG_L_DIV_SHIFT |
			      ATCLK_CORE_L_DIV_MASK << ATCLK_CORE_L_DIV_SHIFT,
			      pclk_dbg_div << PCLK_DBG_L_DIV_SHIFT |
			      atclk_div << ATCLK_CORE_L_DIV_SHIFT));
}

void rkclk_configure_spi(unsigned int bus, unsigned int hz)
{
}
