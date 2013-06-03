/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Samsung Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Clock setup for SMDK5420 board based on EXYNOS5 */

#include <console/console.h>
#include <delay.h>
#include "clk.h"
#include "cpu.h"
#include "dp.h"
#include "dmc.h"
#include "setup.h"

void system_clock_init(void)
{
	struct exynos5420_clock *clk =
		(struct exynos5420_clock *)EXYNOS5_CLOCK_BASE;
	struct exynos5_mct_regs *mct_regs =
		(struct exynos5_mct_regs *)EXYNOS5_MULTI_CORE_TIMER_BASE;
	u32 val;

	/* Turn on the MCT as early as possible. */
	mct_regs->g_tcon |= (1 << 8);

	/* PLL locktime */
	writel(APLL_LOCK_VAL, &clk->apll_lock);
	writel(MPLL_LOCK_VAL, &clk->mpll_lock);
	writel(BPLL_LOCK_VAL, &clk->bpll_lock);
	writel(CPLL_LOCK_VAL, &clk->cpll_lock);
	writel(DPLL_LOCK_VAL, &clk->dpll_lock);
	writel(EPLL_LOCK_VAL, &clk->epll_lock);
	writel(VPLL_LOCK_VAL, &clk->vpll_lock);
	writel(IPLL_LOCK_VAL, &clk->ipll_lock);
	writel(SPLL_LOCK_VAL, &clk->spll_lock);
	writel(KPLL_LOCK_VAL, &clk->kpll_lock);

	setbits_le32(&clk->clk_src_cpu, MUX_HPM_SEL_MASK);

	writel(0, &clk->clk_src_top6);

	writel(0, &clk->clk_src_cdrex);
	writel(SRC_KFC_HPM_SEL, &clk->clk_src_kfc);
	writel(HPM_RATIO,  &clk->clk_div_cpu1);
	writel(CLK_DIV_CPU0_VAL, &clk->clk_div_cpu0);

	/* Set APLL */
	writel(APLL_CON1_VAL, &clk->apll_con1);
	val = set_pll(0xc8, 0x3, 0x1);
	writel(val, &clk->apll_con0);
	while ((readl(&clk->apll_con0) & PLL_LOCKED) == 0)
		;

	writel(SRC_KFC_HPM_SEL, &clk->clk_src_kfc);
	writel(CLK_DIV_KFC_VAL, &clk->clk_div_kfc0);

	/* Set KPLL*/
	writel(KPLL_CON1_VAL, &clk->kpll_con1);
	val = set_pll(0xc8, 0x2, 0x2);
	writel(val, &clk->kpll_con0);
	while ((readl(&clk->kpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set MPLL */
	writel(MPLL_CON1_VAL, &clk->mpll_con1);
	val = set_pll(0xc8, 0x3, 0x1);
	writel(val, &clk->mpll_con0);
	while ((readl(&clk->mpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set DPLL */
	writel(DPLL_CON1_VAL, &clk->dpll_con1);
	val = set_pll(0xc8, 0x2, 0x2);
	writel(val, &clk->dpll_con0);
	while ((readl(&clk->dpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set EPLL */
	writel(EPLL_CON2_VAL, &clk->epll_con2);
	writel(EPLL_CON1_VAL, &clk->epll_con1);
	val = set_pll(0x64, 0x2, 0x1);
	writel(val, &clk->epll_con0);
	while ((readl(&clk->epll_con0) & PLL_LOCKED) == 0)
		;

	/* Set CPLL */
	writel(CPLL_CON1_VAL, &clk->cpll_con1);
	val = set_pll(0x6f, 0x2, 0x1);
	writel(val, &clk->cpll_con0);
	while ((readl(&clk->cpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set IPLL */
	writel(IPLL_CON1_VAL, &clk->ipll_con1);
	val = set_pll(0xB9, 0x3, 0x2);
	writel(val, &clk->ipll_con0);
	while ((readl(&clk->ipll_con0) & PLL_LOCKED) == 0)
		;

	/* Set VPLL */
	writel(VPLL_CON1_VAL, &clk->vpll_con1);
	val = set_pll(0xd7, 0x3, 0x2);
	writel(val, &clk->vpll_con0);
	while ((readl(&clk->vpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set BPLL */
	writel(BPLL_CON1_VAL, &clk->bpll_con1);
	val = set_pll(0xc8, 0x3, 0x1);
	writel(val, &clk->bpll_con0);
	while ((readl(&clk->bpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set SPLL */
	writel(SPLL_CON1_VAL, &clk->spll_con1);
	val = set_pll(0xc8, 0x2, 0x3);
	writel(val, &clk->spll_con0);
	while ((readl(&clk->spll_con0) & PLL_LOCKED) == 0)
		;

	writel(CLK_DIV_CDREX0_VAL, &clk->clk_div_cdrex0);
	writel(CLK_DIV_CDREX1_VAL, &clk->clk_div_cdrex1);

	writel(CLK_SRC_TOP0_VAL, &clk->clk_src_top0);
	writel(CLK_SRC_TOP1_VAL, &clk->clk_src_top1);
	writel(CLK_SRC_TOP2_VAL, &clk->clk_src_top2);
	writel(CLK_SRC_TOP7_VAL, &clk->clk_src_top7);

	writel(CLK_DIV_TOP0_VAL, &clk->clk_div_top0);
	writel(CLK_DIV_TOP1_VAL, &clk->clk_div_top1);
	writel(CLK_DIV_TOP2_VAL, &clk->clk_div_top2);

	writel(0, &clk->clk_src_top10);
	writel(0, &clk->clk_src_top11);
	writel(0, &clk->clk_src_top12);

	writel(CLK_SRC_TOP3_VAL, &clk->clk_src_top3);
	writel(CLK_SRC_TOP4_VAL, &clk->clk_src_top4);
	writel(CLK_SRC_TOP5_VAL, &clk->clk_src_top5);

	/* DISP1 BLK CLK SELECTION */
	writel(CLK_SRC_DISP1_0_VAL, &clk->clk_src_disp10);
	writel(CLK_DIV_DISP1_0_VAL, &clk->clk_div_disp10);

	/* AUDIO BLK */
	writel(AUDIO0_SEL_EPLL, &clk->clk_src_mau);
	writel(DIV_MAU_VAL, &clk->clk_div_mau);

	/* FSYS */
	writel(CLK_SRC_FSYS0_VAL, &clk->clk_src_fsys);
	writel(CLK_DIV_FSYS0_VAL, &clk->clk_div_fsys0);
	writel(CLK_DIV_FSYS1_VAL, &clk->clk_div_fsys1);
	writel(CLK_DIV_FSYS2_VAL, &clk->clk_div_fsys2);

	writel(CLK_SRC_ISP_VAL, &clk->clk_src_isp);
	writel(CLK_DIV_ISP0_VAL, &clk->clk_div_isp0);
	writel(CLK_DIV_ISP1_VAL, &clk->clk_div_isp1);

	writel(CLK_SRC_PERIC0_VAL, &clk->clk_src_peric0);
	writel(CLK_SRC_PERIC1_VAL, &clk->clk_src_peric1);

	writel(CLK_DIV_PERIC0_VAL, &clk->clk_div_peric0);
	writel(CLK_DIV_PERIC1_VAL, &clk->clk_div_peric1);
	writel(CLK_DIV_PERIC2_VAL, &clk->clk_div_peric2);
	writel(CLK_DIV_PERIC3_VAL, &clk->clk_div_peric3);
	writel(CLK_DIV_PERIC4_VAL, &clk->clk_div_peric4);

	writel(CLK_DIV2_RATIO, &clk->clkdiv2_ratio);
	writel(CLK_DIV4_RATIO, &clk->clkdiv4_ratio);
	writel(CLK_DIV_G2D, &clk->clk_div_g2d);

	writel(CLK_SRC_CPU_VAL, &clk->clk_src_cpu);
	writel(CLK_SRC_TOP3_VAL, &clk->clk_src_top6);
	writel(CLK_SRC_CDREX_VAL, &clk->clk_src_cdrex);
	writel(CLK_SRC_KFC_VAL, &clk->clk_src_kfc);
}

void clock_gate(void)
{
	/* Not implemented for now. */
}

void clock_init_dp_clock(void)
{
	/* Not implemented for now. */
}
