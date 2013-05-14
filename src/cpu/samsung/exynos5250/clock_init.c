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

/* Clock setup for SMDK5250 board based on EXYNOS5 */

#include <console/console.h>
#include <delay.h>
#include "clk.h"
#include "cpu.h"
#include "dp.h"
#include "setup.h"

void system_clock_init(struct mem_timings *mem,
		struct arm_clk_ratios *arm_clk_ratio)
{
	struct exynos5_clock *clk = (struct exynos5_clock *)EXYNOS5_CLOCK_BASE;
	struct exynos5_mct_regs *mct_regs =
		(struct exynos5_mct_regs *)EXYNOS5_MULTI_CORE_TIMER_BASE;
	u32 val, tmp;

	/* Turn on the MCT as early as possible. */
	mct_regs->g_tcon |= (1 << 8);

	clrbits_le32(&clk->src_cpu, MUX_APLL_SEL_MASK);
	do {
		val = readl(&clk->mux_stat_cpu);
	} while ((val | MUX_APLL_SEL_MASK) != val);

	clrbits_le32(&clk->src_core1, MUX_MPLL_SEL_MASK);
	do {
		val = readl(&clk->mux_stat_core1);
	} while ((val | MUX_MPLL_SEL_MASK) != val);

	clrbits_le32(&clk->src_top2, MUX_CPLL_SEL_MASK);
	clrbits_le32(&clk->src_top2, MUX_EPLL_SEL_MASK);
	clrbits_le32(&clk->src_top2, MUX_VPLL_SEL_MASK);
	clrbits_le32(&clk->src_top2, MUX_GPLL_SEL_MASK);
	tmp = MUX_CPLL_SEL_MASK | MUX_EPLL_SEL_MASK | MUX_VPLL_SEL_MASK
		| MUX_GPLL_SEL_MASK;
	do {
		val = readl(&clk->mux_stat_top2);
	} while ((val | tmp) != val);

	clrbits_le32(&clk->src_cdrex, MUX_BPLL_SEL_MASK);
	do {
		val = readl(&clk->mux_stat_cdrex);
	} while ((val | MUX_BPLL_SEL_MASK) != val);

	/* PLL locktime */
	writel(APLL_LOCK_VAL, &clk->apll_lock);

	writel(MPLL_LOCK_VAL, &clk->mpll_lock);

	writel(BPLL_LOCK_VAL, &clk->bpll_lock);

	writel(CPLL_LOCK_VAL, &clk->cpll_lock);

	writel(GPLL_LOCK_VAL, &clk->gpll_lock);

	writel(EPLL_LOCK_VAL, &clk->epll_lock);

	writel(VPLL_LOCK_VAL, &clk->vpll_lock);

	writel(CLK_REG_DISABLE, &clk->pll_div2_sel);

	writel(MUX_HPM_SEL_MASK, &clk->src_cpu);
	do {
		val = readl(&clk->mux_stat_cpu);
	} while ((val | HPM_SEL_SCLK_MPLL) != val);

	val = arm_clk_ratio->arm2_ratio << 28
		| arm_clk_ratio->apll_ratio << 24
		| arm_clk_ratio->pclk_dbg_ratio << 20
		| arm_clk_ratio->atb_ratio << 16
		| arm_clk_ratio->periph_ratio << 12
		| arm_clk_ratio->acp_ratio << 8
		| arm_clk_ratio->cpud_ratio << 4
		| arm_clk_ratio->arm_ratio;
	writel(val, &clk->div_cpu0);
	do {
		val = readl(&clk->div_stat_cpu0);
	} while (0 != val);

	writel(CLK_DIV_CPU1_VAL, &clk->div_cpu1);
	do {
		val = readl(&clk->div_stat_cpu1);
	} while (0 != val);

	/* Set APLL */
	writel(APLL_CON1_VAL, &clk->apll_con1);
	val = set_pll(arm_clk_ratio->apll_mdiv, arm_clk_ratio->apll_pdiv,
			arm_clk_ratio->apll_sdiv);
	writel(val, &clk->apll_con0);
	while ((readl(&clk->apll_con0) & APLL_CON0_LOCKED) == 0)
		;

	/* Set MPLL */
	writel(MPLL_CON1_VAL, &clk->mpll_con1);
	val = set_pll(mem->mpll_mdiv, mem->mpll_pdiv, mem->mpll_sdiv);
	writel(val, &clk->mpll_con0);
	while ((readl(&clk->mpll_con0) & MPLL_CON0_LOCKED) == 0)
		;

	/*
	 * Configure MUX_MPLL_FOUT to choose the direct clock source
	 * path and avoid the fixed DIV/2 block to save power
	 */
	setbits_le32(&clk->pll_div2_sel, MUX_MPLL_FOUT_SEL);

	/* Set BPLL */
	if (mem->use_bpll) {
		writel(BPLL_CON1_VAL, &clk->bpll_con1);
		val = set_pll(mem->bpll_mdiv, mem->bpll_pdiv, mem->bpll_sdiv);
		writel(val, &clk->bpll_con0);
		while ((readl(&clk->bpll_con0) & BPLL_CON0_LOCKED) == 0)
			;

		setbits_le32(&clk->pll_div2_sel, MUX_BPLL_FOUT_SEL);
	}

	/* Set CPLL */
	writel(CPLL_CON1_VAL, &clk->cpll_con1);
	val = set_pll(mem->cpll_mdiv, mem->cpll_pdiv, mem->cpll_sdiv);
	writel(val, &clk->cpll_con0);
	while ((readl(&clk->cpll_con0) & CPLL_CON0_LOCKED) == 0)
		;

	/* Set GPLL */
	writel(GPLL_CON1_VAL, &clk->gpll_con1);
	val = set_pll(mem->gpll_mdiv, mem->gpll_pdiv, mem->gpll_sdiv);
	writel(val, &clk->gpll_con0);
	while ((readl(&clk->gpll_con0) & GPLL_CON0_LOCKED) == 0)
		;

	/* Set EPLL */
	writel(EPLL_CON2_VAL, &clk->epll_con2);
	writel(EPLL_CON1_VAL, &clk->epll_con1);
	val = set_pll(mem->epll_mdiv, mem->epll_pdiv, mem->epll_sdiv);
	writel(val, &clk->epll_con0);
	while ((readl(&clk->epll_con0) & EPLL_CON0_LOCKED) == 0)
		;

	/* Set VPLL */
	writel(VPLL_CON2_VAL, &clk->vpll_con2);
	writel(VPLL_CON1_VAL, &clk->vpll_con1);
	val = set_pll(mem->vpll_mdiv, mem->vpll_pdiv, mem->vpll_sdiv);
	writel(val, &clk->vpll_con0);
	while ((readl(&clk->vpll_con0) & VPLL_CON0_LOCKED) == 0)
		;

	writel(CLK_SRC_CORE0_VAL, &clk->src_core0);
	writel(CLK_DIV_CORE0_VAL, &clk->div_core0);
	while (readl(&clk->div_stat_core0) != 0)
		;

	writel(CLK_DIV_CORE1_VAL, &clk->div_core1);
	while (readl(&clk->div_stat_core1) != 0)
		;

	writel(CLK_DIV_SYSRGT_VAL, &clk->div_sysrgt);
	while (readl(&clk->div_stat_sysrgt) != 0)
		;

	writel(CLK_DIV_ACP_VAL, &clk->div_acp);
	while (readl(&clk->div_stat_acp) != 0)
		;

	writel(CLK_DIV_SYSLFT_VAL, &clk->div_syslft);
	while (readl(&clk->div_stat_syslft) != 0)
		;

	writel(CLK_SRC_TOP0_VAL, &clk->src_top0);
	writel(CLK_SRC_TOP1_VAL, &clk->src_top1);
	writel(TOP2_VAL, &clk->src_top2);
	writel(CLK_SRC_TOP3_VAL, &clk->src_top3);

	writel(CLK_DIV_TOP0_VAL, &clk->div_top0);
	while (readl(&clk->div_stat_top0))
		;

	writel(CLK_DIV_TOP1_VAL, &clk->div_top1);
	while (readl(&clk->div_stat_top1))
		;

	writel(CLK_SRC_LEX_VAL, &clk->src_lex);
	while (1) {
		val = readl(&clk->mux_stat_lex);
		if (val == (val | 1))
			break;
	}

	writel(CLK_DIV_LEX_VAL, &clk->div_lex);
	while (readl(&clk->div_stat_lex))
		;

	writel(CLK_DIV_R0X_VAL, &clk->div_r0x);
	while (readl(&clk->div_stat_r0x))
		;

	writel(CLK_DIV_R0X_VAL, &clk->div_r0x);
	while (readl(&clk->div_stat_r0x))
		;

	writel(CLK_DIV_R1X_VAL, &clk->div_r1x);
	while (readl(&clk->div_stat_r1x))
		;

	if (mem->use_bpll) {
		writel(MUX_BPLL_SEL_MASK | MUX_MCLK_CDREX_SEL |
			MUX_MCLK_DPHY_SEL, &clk->src_cdrex);
	} else {
		writel(CLK_REG_DISABLE, &clk->src_cdrex);
	}

	writel(CLK_DIV_CDREX_VAL, &clk->div_cdrex);
	while (readl(&clk->div_stat_cdrex))
		;

	val = readl(&clk->src_cpu);
	val |= CLK_SRC_CPU_VAL;
	writel(val, &clk->src_cpu);

	val = readl(&clk->src_top2);
	val |= CLK_SRC_TOP2_VAL;
	writel(val, &clk->src_top2);

	val = readl(&clk->src_core1);
	val |= CLK_SRC_CORE1_VAL;
	writel(val, &clk->src_core1);

	writel(CLK_SRC_FSYS0_VAL, &clk->src_fsys);
	writel(CLK_DIV_FSYS0_VAL, &clk->div_fsys0);
	while (readl(&clk->div_stat_fsys0))
		;

	writel(CLK_REG_DISABLE, &clk->clkout_cmu_cpu);
	writel(CLK_REG_DISABLE, &clk->clkout_cmu_core);
	writel(CLK_REG_DISABLE, &clk->clkout_cmu_acp);
	writel(CLK_REG_DISABLE, &clk->clkout_cmu_top);
	writel(CLK_REG_DISABLE, &clk->clkout_cmu_lex);
	writel(CLK_REG_DISABLE, &clk->clkout_cmu_r0x);
	writel(CLK_REG_DISABLE, &clk->clkout_cmu_r1x);
	writel(CLK_REG_DISABLE, &clk->clkout_cmu_cdrex);

	writel(CLK_SRC_PERIC0_VAL, &clk->src_peric0);
	writel(CLK_DIV_PERIC0_VAL, &clk->div_peric0);

	writel(CLK_SRC_PERIC1_VAL, &clk->src_peric1);
	writel(CLK_DIV_PERIC1_VAL, &clk->div_peric1);
	writel(CLK_DIV_PERIC2_VAL, &clk->div_peric2);
	writel(SCLK_SRC_ISP_VAL, &clk->sclk_src_isp);
	writel(SCLK_DIV_ISP_VAL, &clk->sclk_div_isp);
	writel(CLK_DIV_ISP0_VAL, &clk->div_isp0);
	writel(CLK_DIV_ISP1_VAL, &clk->div_isp1);
	writel(CLK_DIV_ISP2_VAL, &clk->div_isp2);

	/* FIMD1 SRC CLK SELECTION */
	writel(CLK_SRC_DISP1_0_VAL, &clk->src_disp1_0);

	val = MMC2_PRE_RATIO_VAL << MMC2_PRE_RATIO_OFFSET
		| MMC2_RATIO_VAL << MMC2_RATIO_OFFSET
		| MMC3_PRE_RATIO_VAL << MMC3_PRE_RATIO_OFFSET
		| MMC3_RATIO_VAL << MMC3_RATIO_OFFSET;
	writel(val, &clk->div_fsys2);
}

void clock_gate(void)
{
	struct exynos5_clock *clk = (struct exynos5_clock *)EXYNOS5_CLOCK_BASE;

	/* CLK_GATE_IP_SYSRGT */
	clrbits_le32(&clk->gate_ip_sysrgt, CLK_C2C_MASK);

	/* CLK_GATE_IP_ACP */
	clrbits_le32(&clk->gate_ip_acp, CLK_SMMUG2D_MASK |
					CLK_SMMUSSS_MASK |
					CLK_SMMUMDMA_MASK |
					CLK_ID_REMAPPER_MASK |
					CLK_G2D_MASK |
					CLK_SSS_MASK |
					CLK_MDMA_MASK |
					CLK_SECJTAG_MASK);

	/* CLK_GATE_BUS_SYSLFT */
	clrbits_le32(&clk->gate_bus_syslft, CLK_EFCLK_MASK);

	/* CLK_GATE_IP_ISP0 */
	clrbits_le32(&clk->gate_ip_isp0, CLK_UART_ISP_MASK |
					 CLK_WDT_ISP_MASK |
					 CLK_PWM_ISP_MASK |
					 CLK_MTCADC_ISP_MASK |
					 CLK_I2C1_ISP_MASK |
					 CLK_I2C0_ISP_MASK |
					 CLK_MPWM_ISP_MASK |
					 CLK_MCUCTL_ISP_MASK |
					 CLK_INT_COMB_ISP_MASK |
					 CLK_SMMU_MCUISP_MASK |
					 CLK_SMMU_SCALERP_MASK |
					 CLK_SMMU_SCALERC_MASK |
					 CLK_SMMU_FD_MASK |
					 CLK_SMMU_DRC_MASK |
					 CLK_SMMU_ISP_MASK |
					 CLK_GICISP_MASK |
					 CLK_ARM9S_MASK |
					 CLK_MCUISP_MASK |
					 CLK_SCALERP_MASK |
					 CLK_SCALERC_MASK |
					 CLK_FD_MASK |
					 CLK_DRC_MASK |
					 CLK_ISP_MASK);

	/* CLK_GATE_IP_ISP1 */
	clrbits_le32(&clk->gate_ip_isp1, CLK_SPI1_ISP_MASK |
					 CLK_SPI0_ISP_MASK |
					 CLK_SMMU3DNR_MASK |
					 CLK_SMMUDIS1_MASK |
					 CLK_SMMUDIS0_MASK |
					 CLK_SMMUODC_MASK |
					 CLK_3DNR_MASK |
					 CLK_DIS_MASK |
					 CLK_ODC_MASK);

	/* CLK_GATE_SCLK_ISP */
	clrbits_le32(&clk->gate_sclk_isp, SCLK_MPWM_ISP_MASK);

	/* CLK_GATE_IP_GSCL */
	clrbits_le32(&clk->gate_ip_gscl, CLK_SMMUFIMC_LITE2_MASK |
					 CLK_SMMUFIMC_LITE1_MASK |
					 CLK_SMMUFIMC_LITE0_MASK |
					 CLK_SMMUGSCL3_MASK |
					 CLK_SMMUGSCL2_MASK |
					 CLK_SMMUGSCL1_MASK |
					 CLK_SMMUGSCL0_MASK |
					 CLK_GSCL_WRAP_B_MASK |
					 CLK_GSCL_WRAP_A_MASK |
					 CLK_CAMIF_TOP_MASK |
					 CLK_GSCL3_MASK |
					 CLK_GSCL2_MASK |
					 CLK_GSCL1_MASK |
					 CLK_GSCL0_MASK);

	/* CLK_GATE_IP_DISP1 */
	clrbits_le32(&clk->gate_ip_disp1, CLK_SMMUTVX_MASK |
					  CLK_ASYNCTVX_MASK |
					  CLK_HDMI_MASK |
					  CLK_MIXER_MASK |
					  CLK_DSIM1_MASK);

	/* CLK_GATE_IP_MFC */
	clrbits_le32(&clk->gate_ip_mfc, CLK_SMMUMFCR_MASK |
					CLK_SMMUMFCL_MASK |
					CLK_MFC_MASK);

	/* CLK_GATE_IP_GEN */
	clrbits_le32(&clk->gate_ip_gen, CLK_SMMUMDMA1_MASK |
					CLK_SMMUJPEG_MASK |
					CLK_SMMUROTATOR_MASK |
					CLK_MDMA1_MASK |
					CLK_JPEG_MASK |
					CLK_ROTATOR_MASK);

	/* CLK_GATE_IP_FSYS */
	clrbits_le32(&clk->gate_ip_fsys, CLK_WDT_IOP_MASK |
					 CLK_SMMUMCU_IOP_MASK |
					 CLK_SATA_PHY_I2C_MASK |
					 CLK_SATA_PHY_CTRL_MASK |
					 CLK_MCUCTL_MASK |
					 CLK_NFCON_MASK |
					 CLK_SMMURTIC_MASK |
					 CLK_RTIC_MASK |
					 CLK_MIPI_HSI_MASK |
					 CLK_USBOTG_MASK |
					 CLK_SATA_MASK |
					 CLK_PDMA1_MASK |
					 CLK_PDMA0_MASK |
					 CLK_MCU_IOP_MASK);

	/* CLK_GATE_IP_PERIC */
	clrbits_le32(&clk->gate_ip_peric, CLK_HS_I2C3_MASK |
					  CLK_HS_I2C2_MASK |
					  CLK_HS_I2C1_MASK |
					  CLK_HS_I2C0_MASK |
					  CLK_AC97_MASK |
					  CLK_SPDIF_MASK |
					  CLK_PCM2_MASK |
					  CLK_PCM1_MASK |
					  CLK_I2S2_MASK |
					  CLK_SPI2_MASK |
					  CLK_SPI0_MASK);

	/*
	 * CLK_GATE_IP_PERIS
	 * Note: Keep CHIPID_APBIF ungated to ensure reading the product ID
	 * register (PRO_ID) works correctly when the OS kernel determines
	 * which chip it is running on.
	 */
	clrbits_le32(&clk->gate_ip_peris, CLK_RTC_MASK |
					  CLK_TZPC9_MASK |
					  CLK_TZPC8_MASK |
					  CLK_TZPC7_MASK |
					  CLK_TZPC6_MASK |
					  CLK_TZPC5_MASK |
					  CLK_TZPC4_MASK |
					  CLK_TZPC3_MASK |
					  CLK_TZPC2_MASK |
					  CLK_TZPC1_MASK |
					  CLK_TZPC0_MASK);

	/* CLK_GATE_BLOCK */
	clrbits_le32(&clk->gate_block, CLK_ACP_MASK);

	/* CLK_GATE_IP_CDREX */
	clrbits_le32(&clk->gate_ip_cdrex, CLK_DPHY0_MASK |
					  CLK_DPHY1_MASK |
					  CLK_TZASC_DRBXR_MASK);

}

void clock_init_dp_clock(void)
{
	struct exynos5_clock *clk = (struct exynos5_clock *)EXYNOS5_CLOCK_BASE;

	/* DP clock enable */
	setbits_le32(&clk->gate_ip_disp1, CLK_GATE_DP1_ALLOW);

	/* We run DP at 267 Mhz */
	setbits_le32(&clk->div_disp1_0, CLK_DIV_DISP1_0_FIMD1);
}

