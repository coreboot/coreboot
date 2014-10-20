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
#include <soc/clk.h>
#include <soc/cpu.h>
#include <soc/dp.h>
#include <soc/dmc.h>
#include <soc/setup.h>

void system_clock_init(void)
{
	u32 val;

	/* Turn on the MCT as early as possible. */
	exynos_mct->g_tcon |= (1 << 8);

	/* PLL locktime */
	writel(APLL_LOCK_VAL, &exynos_clock->apll_lock);
	writel(MPLL_LOCK_VAL, &exynos_clock->mpll_lock);
	writel(BPLL_LOCK_VAL, &exynos_clock->bpll_lock);
	writel(CPLL_LOCK_VAL, &exynos_clock->cpll_lock);
	writel(DPLL_LOCK_VAL, &exynos_clock->dpll_lock);
	writel(EPLL_LOCK_VAL, &exynos_clock->epll_lock);
	writel(VPLL_LOCK_VAL, &exynos_clock->vpll_lock);
	writel(IPLL_LOCK_VAL, &exynos_clock->ipll_lock);
	writel(SPLL_LOCK_VAL, &exynos_clock->spll_lock);
	writel(KPLL_LOCK_VAL, &exynos_clock->kpll_lock);
        writel(RPLL_LOCK_VAL, &exynos_clock->rpll_lock);

	setbits_le32(&exynos_clock->clk_src_cpu, MUX_HPM_SEL_MASK);

	writel(0, &exynos_clock->clk_src_top6);

	writel(0, &exynos_clock->clk_src_cdrex);
	writel(SRC_KFC_HPM_SEL, &exynos_clock->clk_src_kfc);
	writel(HPM_RATIO,  &exynos_clock->clk_div_cpu1);
	writel(CLK_DIV_CPU0_VAL, &exynos_clock->clk_div_cpu0);

	/* switch A15 clock source to OSC clock before changing APLL */
	clrbits_le32(&exynos_clock->clk_src_cpu, APLL_FOUT);

	/* Set APLL */
	writel(APLL_CON1_VAL, &exynos_clock->apll_con1);
	val = set_pll(225, 3, 0);	/* FOUT=1800MHz */
	writel(val, &exynos_clock->apll_con0);
	while ((readl(&exynos_clock->apll_con0) & PLL_LOCKED) == 0)
		;

	/* now it is safe to switch to APLL */
	setbits_le32(&exynos_clock->clk_src_cpu, APLL_FOUT);

	writel(SRC_KFC_HPM_SEL, &exynos_clock->clk_src_kfc);
	writel(CLK_DIV_KFC_VAL, &exynos_clock->clk_div_kfc0);

	/* switch A7 clock source to OSC clock before changing KPLL */
	clrbits_le32(&exynos_clock->clk_src_kfc, KPLL_FOUT);

	/* Set KPLL*/
	writel(KPLL_CON1_VAL, &exynos_clock->kpll_con1);
	val = set_pll(0x190, 0x4, 0x2);
	writel(val, &exynos_clock->kpll_con0);
	while ((readl(&exynos_clock->kpll_con0) & PLL_LOCKED) == 0)
		;

	/* now it is safe to switch to KPLL */
	setbits_le32(&exynos_clock->clk_src_kfc, KPLL_FOUT);

	/* Set MPLL */
	writel(MPLL_CON1_VAL, &exynos_clock->mpll_con1);
	val = set_pll(0xc8, 0x3, 0x1);
	writel(val, &exynos_clock->mpll_con0);
	while ((readl(&exynos_clock->mpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set DPLL */
	writel(DPLL_CON1_VAL, &exynos_clock->dpll_con1);
	val = set_pll(0x190, 0x4, 0x2);
	writel(val, &exynos_clock->dpll_con0);
	while ((readl(&exynos_clock->dpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set EPLL */
	writel(EPLL_CON2_VAL, &exynos_clock->epll_con2);
	writel(EPLL_CON1_VAL, &exynos_clock->epll_con1);
	val = set_pll(0x64, 0x2, 0x1);
	writel(val, &exynos_clock->epll_con0);
	while ((readl(&exynos_clock->epll_con0) & PLL_LOCKED) == 0)
		;

	/* Set CPLL */
	writel(CPLL_CON1_VAL, &exynos_clock->cpll_con1);
	val = set_pll(0xde, 0x4, 0x1);
	writel(val, &exynos_clock->cpll_con0);
	while ((readl(&exynos_clock->cpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set IPLL */
	writel(IPLL_CON1_VAL, &exynos_clock->ipll_con1);
	val = set_pll(0xB9, 0x3, 0x2);
	writel(val, &exynos_clock->ipll_con0);
	while ((readl(&exynos_clock->ipll_con0) & PLL_LOCKED) == 0)
		;

	/* Set VPLL */
	writel(VPLL_CON1_VAL, &exynos_clock->vpll_con1);
	val = set_pll(0xd7, 0x3, 0x2);
	writel(val, &exynos_clock->vpll_con0);
	while ((readl(&exynos_clock->vpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set BPLL */
	writel(BPLL_CON1_VAL, &exynos_clock->bpll_con1);
	val = set_pll(0xc8, 0x3, 0x1);
	writel(val, &exynos_clock->bpll_con0);
	while ((readl(&exynos_clock->bpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set SPLL */
	writel(SPLL_CON1_VAL, &exynos_clock->spll_con1);
	val = set_pll(200, 0x3, 0x2);	/* 400MHz */
	writel(val, &exynos_clock->spll_con0);
	while ((readl(&exynos_clock->spll_con0) & PLL_LOCKED) == 0)
		;

        /* We use RPLL as the source for FIMD video stream clock */
	writel(RPLL_CON1_VAL, &exynos_clock->rpll_con1);
	writel(RPLL_CON2_VAL, &exynos_clock->rpll_con2);
	/* computed by gabe from first principles; u-boot is probably
	 * wrong again
	 */
	val = set_pll(0xa0, 0x3, 0x2);
	writel(val, &exynos_clock->rpll_con0);
	/* note: this is a meaningless exercise. The hardware lock
	 * detection does not work. So this just spins for some
	 * time and is done. NO indication of success should attach
	 * to this or any other spin on a con0 value.
	 */
	while ((readl(&exynos_clock->rpll_con0) & PLL_LOCKED) == 0)
		;

	writel(CLK_DIV_CDREX0_VAL, &exynos_clock->clk_div_cdrex0);
	writel(CLK_DIV_CDREX1_VAL, &exynos_clock->clk_div_cdrex1);

	writel(CLK_SRC_TOP0_VAL, &exynos_clock->clk_src_top0);
	writel(CLK_SRC_TOP1_VAL, &exynos_clock->clk_src_top1);
	writel(CLK_SRC_TOP2_VAL, &exynos_clock->clk_src_top2);
	writel(CLK_SRC_TOP7_VAL, &exynos_clock->clk_src_top7);

	writel(CLK_DIV_TOP0_VAL, &exynos_clock->clk_div_top0);
	writel(CLK_DIV_TOP1_VAL, &exynos_clock->clk_div_top1);
	writel(CLK_DIV_TOP2_VAL, &exynos_clock->clk_div_top2);

	writel(0, &exynos_clock->clk_src_top10);
	writel(0, &exynos_clock->clk_src_top11);
	writel(0, &exynos_clock->clk_src_top12);

	writel(CLK_SRC_TOP3_VAL, &exynos_clock->clk_src_top3);
	writel(CLK_SRC_TOP4_VAL, &exynos_clock->clk_src_top4);
	writel(CLK_SRC_TOP5_VAL, &exynos_clock->clk_src_top5);

	/* DISP1 BLK CLK SELECTION */
	writel(CLK_SRC_DISP1_0_VAL, &exynos_clock->clk_src_disp10);
	writel(CLK_DIV_DISP1_0_VAL, &exynos_clock->clk_div_disp10);

	/* AUDIO BLK */
	writel(AUDIO0_SEL_EPLL, &exynos_clock->clk_src_mau);
	writel(DIV_MAU_VAL, &exynos_clock->clk_div_mau);

	/* FSYS */
	writel(CLK_SRC_FSYS0_VAL, &exynos_clock->clk_src_fsys);
	writel(CLK_DIV_FSYS0_VAL, &exynos_clock->clk_div_fsys0);
	writel(CLK_DIV_FSYS1_VAL, &exynos_clock->clk_div_fsys1);
	writel(CLK_DIV_FSYS2_VAL, &exynos_clock->clk_div_fsys2);

	writel(CLK_SRC_ISP_VAL, &exynos_clock->clk_src_isp);
	writel(CLK_DIV_ISP0_VAL, &exynos_clock->clk_div_isp0);
	writel(CLK_DIV_ISP1_VAL, &exynos_clock->clk_div_isp1);

	writel(CLK_SRC_PERIC0_VAL, &exynos_clock->clk_src_peric0);
	writel(CLK_SRC_PERIC1_VAL, &exynos_clock->clk_src_peric1);

	writel(CLK_DIV_PERIC0_VAL, &exynos_clock->clk_div_peric0);
	writel(CLK_DIV_PERIC1_VAL, &exynos_clock->clk_div_peric1);
	writel(CLK_DIV_PERIC2_VAL, &exynos_clock->clk_div_peric2);
	writel(CLK_DIV_PERIC3_VAL, &exynos_clock->clk_div_peric3);
	writel(CLK_DIV_PERIC4_VAL, &exynos_clock->clk_div_peric4);

	writel(CLK_DIV_CPERI1_VAL, &exynos_clock->clk_div_cperi1);

	writel(CLK_DIV2_RATIO, &exynos_clock->clkdiv2_ratio);
	writel(CLK_DIV4_RATIO, &exynos_clock->clkdiv4_ratio);
	writel(CLK_DIV_G2D, &exynos_clock->clk_div_g2d);

	writel(CLK_SRC_CPU_VAL, &exynos_clock->clk_src_cpu);
	writel(CLK_SRC_TOP6_VAL, &exynos_clock->clk_src_top6);
	writel(CLK_SRC_CDREX_VAL, &exynos_clock->clk_src_cdrex);
	writel(CLK_SRC_KFC_VAL, &exynos_clock->clk_src_kfc);
}

void clock_gate(void)
{
	/* Not implemented for now. */
}
