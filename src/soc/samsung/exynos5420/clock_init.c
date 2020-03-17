/*
 * This file is part of the coreboot project.
 *
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

/* Clock setup for SMDK5420 board based on EXYNOS5 */

#include <device/mmio.h>
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
	write32(&exynos_clock->apll_lock, APLL_LOCK_VAL);
	write32(&exynos_clock->mpll_lock, MPLL_LOCK_VAL);
	write32(&exynos_clock->bpll_lock, BPLL_LOCK_VAL);
	write32(&exynos_clock->cpll_lock, CPLL_LOCK_VAL);
	write32(&exynos_clock->dpll_lock, DPLL_LOCK_VAL);
	write32(&exynos_clock->epll_lock, EPLL_LOCK_VAL);
	write32(&exynos_clock->vpll_lock, VPLL_LOCK_VAL);
	write32(&exynos_clock->ipll_lock, IPLL_LOCK_VAL);
	write32(&exynos_clock->spll_lock, SPLL_LOCK_VAL);
	write32(&exynos_clock->kpll_lock, KPLL_LOCK_VAL);
	write32(&exynos_clock->rpll_lock, RPLL_LOCK_VAL);

	setbits32(&exynos_clock->clk_src_cpu, MUX_HPM_SEL_MASK);

	write32(&exynos_clock->clk_src_top6, 0);

	write32(&exynos_clock->clk_src_cdrex, 0);
	write32(&exynos_clock->clk_src_kfc, SRC_KFC_HPM_SEL);
	write32(&exynos_clock->clk_div_cpu1, HPM_RATIO);
	write32(&exynos_clock->clk_div_cpu0, CLK_DIV_CPU0_VAL);

	/* switch A15 clock source to OSC clock before changing APLL */
	clrbits32(&exynos_clock->clk_src_cpu, APLL_FOUT);

	/* Set APLL */
	write32(&exynos_clock->apll_con1, APLL_CON1_VAL);
	val = set_pll(225, 3, 0);	/* FOUT=1800MHz */
	write32(&exynos_clock->apll_con0, val);
	while ((read32(&exynos_clock->apll_con0) & PLL_LOCKED) == 0)
		;

	/* now it is safe to switch to APLL */
	setbits32(&exynos_clock->clk_src_cpu, APLL_FOUT);

	write32(&exynos_clock->clk_src_kfc, SRC_KFC_HPM_SEL);
	write32(&exynos_clock->clk_div_kfc0, CLK_DIV_KFC_VAL);

	/* switch A7 clock source to OSC clock before changing KPLL */
	clrbits32(&exynos_clock->clk_src_kfc, KPLL_FOUT);

	/* Set KPLL*/
	write32(&exynos_clock->kpll_con1, KPLL_CON1_VAL);
	val = set_pll(0x190, 0x4, 0x2);
	write32(&exynos_clock->kpll_con0, val);
	while ((read32(&exynos_clock->kpll_con0) & PLL_LOCKED) == 0)
		;

	/* now it is safe to switch to KPLL */
	setbits32(&exynos_clock->clk_src_kfc, KPLL_FOUT);

	/* Set MPLL */
	write32(&exynos_clock->mpll_con1, MPLL_CON1_VAL);
	val = set_pll(0xc8, 0x3, 0x1);
	write32(&exynos_clock->mpll_con0, val);
	while ((read32(&exynos_clock->mpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set DPLL */
	write32(&exynos_clock->dpll_con1, DPLL_CON1_VAL);
	val = set_pll(0x190, 0x4, 0x2);
	write32(&exynos_clock->dpll_con0, val);
	while ((read32(&exynos_clock->dpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set EPLL */
	write32(&exynos_clock->epll_con2, EPLL_CON2_VAL);
	write32(&exynos_clock->epll_con1, EPLL_CON1_VAL);
	val = set_pll(0x64, 0x2, 0x1);
	write32(&exynos_clock->epll_con0, val);
	while ((read32(&exynos_clock->epll_con0) & PLL_LOCKED) == 0)
		;

	/* Set CPLL */
	write32(&exynos_clock->cpll_con1, CPLL_CON1_VAL);
	val = set_pll(0xde, 0x4, 0x1);
	write32(&exynos_clock->cpll_con0, val);
	while ((read32(&exynos_clock->cpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set IPLL */
	write32(&exynos_clock->ipll_con1, IPLL_CON1_VAL);
	val = set_pll(0xB9, 0x3, 0x2);
	write32(&exynos_clock->ipll_con0, val);
	while ((read32(&exynos_clock->ipll_con0) & PLL_LOCKED) == 0)
		;

	/* Set VPLL */
	write32(&exynos_clock->vpll_con1, VPLL_CON1_VAL);
	val = set_pll(0xd7, 0x3, 0x2);
	write32(&exynos_clock->vpll_con0, val);
	while ((read32(&exynos_clock->vpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set BPLL */
	write32(&exynos_clock->bpll_con1, BPLL_CON1_VAL);
	val = set_pll(0xc8, 0x3, 0x1);
	write32(&exynos_clock->bpll_con0, val);
	while ((read32(&exynos_clock->bpll_con0) & PLL_LOCKED) == 0)
		;

	/* Set SPLL */
	write32(&exynos_clock->spll_con1, SPLL_CON1_VAL);
	val = set_pll(200, 0x3, 0x2);	/* 400MHz */
	write32(&exynos_clock->spll_con0, val);
	while ((read32(&exynos_clock->spll_con0) & PLL_LOCKED) == 0)
		;

	/* We use RPLL as the source for FIMD video stream clock */
	write32(&exynos_clock->rpll_con1, RPLL_CON1_VAL);
	write32(&exynos_clock->rpll_con2, RPLL_CON2_VAL);
	/* computed by gabe from first principles; u-boot is probably
	 * wrong again
	 */
	val = set_pll(0xa0, 0x3, 0x2);
	write32(&exynos_clock->rpll_con0, val);
	/* note: this is a meaningless exercise. The hardware lock
	 * detection does not work. So this just spins for some
	 * time and is done. NO indication of success should attach
	 * to this or any other spin on a con0 value.
	 */
	while ((read32(&exynos_clock->rpll_con0) & PLL_LOCKED) == 0)
		;

	write32(&exynos_clock->clk_div_cdrex0, CLK_DIV_CDREX0_VAL);
	write32(&exynos_clock->clk_div_cdrex1, CLK_DIV_CDREX1_VAL);

	write32(&exynos_clock->clk_src_top0, CLK_SRC_TOP0_VAL);
	write32(&exynos_clock->clk_src_top1, CLK_SRC_TOP1_VAL);
	write32(&exynos_clock->clk_src_top2, CLK_SRC_TOP2_VAL);
	write32(&exynos_clock->clk_src_top7, CLK_SRC_TOP7_VAL);

	write32(&exynos_clock->clk_div_top0, CLK_DIV_TOP0_VAL);
	write32(&exynos_clock->clk_div_top1, CLK_DIV_TOP1_VAL);
	write32(&exynos_clock->clk_div_top2, CLK_DIV_TOP2_VAL);

	write32(&exynos_clock->clk_src_top10, 0);
	write32(&exynos_clock->clk_src_top11, 0);
	write32(&exynos_clock->clk_src_top12, 0);

	write32(&exynos_clock->clk_src_top3, CLK_SRC_TOP3_VAL);
	write32(&exynos_clock->clk_src_top4, CLK_SRC_TOP4_VAL);
	write32(&exynos_clock->clk_src_top5, CLK_SRC_TOP5_VAL);

	/* DISP1 BLK CLK SELECTION */
	write32(&exynos_clock->clk_src_disp10, CLK_SRC_DISP1_0_VAL);
	write32(&exynos_clock->clk_div_disp10, CLK_DIV_DISP1_0_VAL);

	/* AUDIO BLK */
	write32(&exynos_clock->clk_src_mau, AUDIO0_SEL_EPLL);
	write32(&exynos_clock->clk_div_mau, DIV_MAU_VAL);

	/* FSYS */
	write32(&exynos_clock->clk_src_fsys, CLK_SRC_FSYS0_VAL);
	write32(&exynos_clock->clk_div_fsys0, CLK_DIV_FSYS0_VAL);
	write32(&exynos_clock->clk_div_fsys1, CLK_DIV_FSYS1_VAL);
	write32(&exynos_clock->clk_div_fsys2, CLK_DIV_FSYS2_VAL);

	write32(&exynos_clock->clk_src_isp, CLK_SRC_ISP_VAL);
	write32(&exynos_clock->clk_div_isp0, CLK_DIV_ISP0_VAL);
	write32(&exynos_clock->clk_div_isp1, CLK_DIV_ISP1_VAL);

	write32(&exynos_clock->clk_src_peric0, CLK_SRC_PERIC0_VAL);
	write32(&exynos_clock->clk_src_peric1, CLK_SRC_PERIC1_VAL);

	write32(&exynos_clock->clk_div_peric0, CLK_DIV_PERIC0_VAL);
	write32(&exynos_clock->clk_div_peric1, CLK_DIV_PERIC1_VAL);
	write32(&exynos_clock->clk_div_peric2, CLK_DIV_PERIC2_VAL);
	write32(&exynos_clock->clk_div_peric3, CLK_DIV_PERIC3_VAL);
	write32(&exynos_clock->clk_div_peric4, CLK_DIV_PERIC4_VAL);

	write32(&exynos_clock->clk_div_cperi1, CLK_DIV_CPERI1_VAL);

	write32(&exynos_clock->clkdiv2_ratio, CLK_DIV2_RATIO);
	write32(&exynos_clock->clkdiv4_ratio, CLK_DIV4_RATIO);
	write32(&exynos_clock->clk_div_g2d, CLK_DIV_G2D);

	write32(&exynos_clock->clk_src_cpu, CLK_SRC_CPU_VAL);
	write32(&exynos_clock->clk_src_top6, CLK_SRC_TOP6_VAL);
	write32(&exynos_clock->clk_src_cdrex, CLK_SRC_CDREX_VAL);
	write32(&exynos_clock->clk_src_kfc, CLK_SRC_KFC_VAL);
}

void clock_gate(void)
{
	/* Not implemented for now. */
}
