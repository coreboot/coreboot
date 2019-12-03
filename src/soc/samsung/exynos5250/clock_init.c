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
 */

/* Clock setup for SMDK5250 board based on EXYNOS5 */

#include <device/mmio.h>
#include <soc/clk.h>
#include <soc/dp.h>
#include <soc/setup.h>

void system_clock_init(struct mem_timings *mem,
		struct arm_clk_ratios *arm_clk_ratio)
{
	u32 val, tmp;

	/* Turn on the MCT as early as possible. */
	exynos_mct->g_tcon |= (1 << 8);

	clrbits32(&exynos_clock->src_cpu, MUX_APLL_SEL_MASK);
	do {
		val = read32(&exynos_clock->mux_stat_cpu);
	} while ((val | MUX_APLL_SEL_MASK) != val);

	clrbits32(&exynos_clock->src_core1, MUX_MPLL_SEL_MASK);
	do {
		val = read32(&exynos_clock->mux_stat_core1);
	} while ((val | MUX_MPLL_SEL_MASK) != val);

	clrbits32(&exynos_clock->src_top2, MUX_CPLL_SEL_MASK);
	clrbits32(&exynos_clock->src_top2, MUX_EPLL_SEL_MASK);
	clrbits32(&exynos_clock->src_top2, MUX_VPLL_SEL_MASK);
	clrbits32(&exynos_clock->src_top2, MUX_GPLL_SEL_MASK);
	tmp = MUX_CPLL_SEL_MASK | MUX_EPLL_SEL_MASK | MUX_VPLL_SEL_MASK
		| MUX_GPLL_SEL_MASK;
	do {
		val = read32(&exynos_clock->mux_stat_top2);
	} while ((val | tmp) != val);

	clrbits32(&exynos_clock->src_cdrex, MUX_BPLL_SEL_MASK);
	do {
		val = read32(&exynos_clock->mux_stat_cdrex);
	} while ((val | MUX_BPLL_SEL_MASK) != val);

	/* PLL locktime */
	write32(&exynos_clock->apll_lock, APLL_LOCK_VAL);

	write32(&exynos_clock->mpll_lock, MPLL_LOCK_VAL);

	write32(&exynos_clock->bpll_lock, BPLL_LOCK_VAL);

	write32(&exynos_clock->cpll_lock, CPLL_LOCK_VAL);

	write32(&exynos_clock->gpll_lock, GPLL_LOCK_VAL);

	write32(&exynos_clock->epll_lock, EPLL_LOCK_VAL);

	write32(&exynos_clock->vpll_lock, VPLL_LOCK_VAL);

	write32(&exynos_clock->pll_div2_sel, CLK_REG_DISABLE);

	write32(&exynos_clock->src_cpu, MUX_HPM_SEL_MASK);
	do {
		val = read32(&exynos_clock->mux_stat_cpu);
	} while ((val | HPM_SEL_SCLK_MPLL) != val);

	val = arm_clk_ratio->arm2_ratio << 28
		| arm_clk_ratio->apll_ratio << 24
		| arm_clk_ratio->pclk_dbg_ratio << 20
		| arm_clk_ratio->atb_ratio << 16
		| arm_clk_ratio->periph_ratio << 12
		| arm_clk_ratio->acp_ratio << 8
		| arm_clk_ratio->cpud_ratio << 4
		| arm_clk_ratio->arm_ratio;
	write32(&exynos_clock->div_cpu0, val);
	do {
		val = read32(&exynos_clock->div_stat_cpu0);
	} while (0 != val);

	write32(&exynos_clock->div_cpu1, CLK_DIV_CPU1_VAL);
	do {
		val = read32(&exynos_clock->div_stat_cpu1);
	} while (0 != val);

	/* switch A15 clock source to OSC clock before changing APLL */
	clrbits32(&exynos_clock->src_cpu, APLL_FOUT);

	/* Set APLL */
	write32(&exynos_clock->apll_con1, APLL_CON1_VAL);
	val = set_pll(arm_clk_ratio->apll_mdiv, arm_clk_ratio->apll_pdiv,
			arm_clk_ratio->apll_sdiv);
	write32(&exynos_clock->apll_con0, val);
	while ((read32(&exynos_clock->apll_con0) & APLL_CON0_LOCKED) == 0)
		;

	/* now it is safe to switch to APLL */
	setbits32(&exynos_clock->src_cpu, APLL_FOUT);

	/* Set MPLL */
	write32(&exynos_clock->mpll_con1, MPLL_CON1_VAL);
	val = set_pll(mem->mpll_mdiv, mem->mpll_pdiv, mem->mpll_sdiv);
	write32(&exynos_clock->mpll_con0, val);
	while ((read32(&exynos_clock->mpll_con0) & MPLL_CON0_LOCKED) == 0)
		;

	/*
	 * Configure MUX_MPLL_FOUT to choose the direct clock source
	 * path and avoid the fixed DIV/2 block to save power
	 */
	setbits32(&exynos_clock->pll_div2_sel, MUX_MPLL_FOUT_SEL);

	/* Set BPLL */
	if (mem->use_bpll) {
		write32(&exynos_clock->bpll_con1, BPLL_CON1_VAL);
		val = set_pll(mem->bpll_mdiv, mem->bpll_pdiv, mem->bpll_sdiv);
		write32(&exynos_clock->bpll_con0, val);
		while ((read32(&exynos_clock->bpll_con0) & BPLL_CON0_LOCKED) == 0)
			;

		setbits32(&exynos_clock->pll_div2_sel, MUX_BPLL_FOUT_SEL);
	}

	/* Set CPLL */
	write32(&exynos_clock->cpll_con1, CPLL_CON1_VAL);
	val = set_pll(mem->cpll_mdiv, mem->cpll_pdiv, mem->cpll_sdiv);
	write32(&exynos_clock->cpll_con0, val);
	while ((read32(&exynos_clock->cpll_con0) & CPLL_CON0_LOCKED) == 0)
		;

	/* Set GPLL */
	write32(&exynos_clock->gpll_con1, GPLL_CON1_VAL);
	val = set_pll(mem->gpll_mdiv, mem->gpll_pdiv, mem->gpll_sdiv);
	write32(&exynos_clock->gpll_con0, val);
	while ((read32(&exynos_clock->gpll_con0) & GPLL_CON0_LOCKED) == 0)
		;

	/* Set EPLL */
	write32(&exynos_clock->epll_con2, EPLL_CON2_VAL);
	write32(&exynos_clock->epll_con1, EPLL_CON1_VAL);
	val = set_pll(mem->epll_mdiv, mem->epll_pdiv, mem->epll_sdiv);
	write32(&exynos_clock->epll_con0, val);
	while ((read32(&exynos_clock->epll_con0) & EPLL_CON0_LOCKED) == 0)
		;

	/* Set VPLL */
	write32(&exynos_clock->vpll_con2, VPLL_CON2_VAL);
	write32(&exynos_clock->vpll_con1, VPLL_CON1_VAL);
	val = set_pll(mem->vpll_mdiv, mem->vpll_pdiv, mem->vpll_sdiv);
	write32(&exynos_clock->vpll_con0, val);
	while ((read32(&exynos_clock->vpll_con0) & VPLL_CON0_LOCKED) == 0)
		;

	write32(&exynos_clock->src_core0, CLK_SRC_CORE0_VAL);
	write32(&exynos_clock->div_core0, CLK_DIV_CORE0_VAL);
	while (read32(&exynos_clock->div_stat_core0) != 0)
		;

	write32(&exynos_clock->div_core1, CLK_DIV_CORE1_VAL);
	while (read32(&exynos_clock->div_stat_core1) != 0)
		;

	write32(&exynos_clock->div_sysrgt, CLK_DIV_SYSRGT_VAL);
	while (read32(&exynos_clock->div_stat_sysrgt) != 0)
		;

	write32(&exynos_clock->div_acp, CLK_DIV_ACP_VAL);
	while (read32(&exynos_clock->div_stat_acp) != 0)
		;

	write32(&exynos_clock->div_syslft, CLK_DIV_SYSLFT_VAL);
	while (read32(&exynos_clock->div_stat_syslft) != 0)
		;

	write32(&exynos_clock->src_top0, CLK_SRC_TOP0_VAL);
	write32(&exynos_clock->src_top1, CLK_SRC_TOP1_VAL);
	write32(&exynos_clock->src_top2, TOP2_VAL);
	write32(&exynos_clock->src_top3, CLK_SRC_TOP3_VAL);

	write32(&exynos_clock->div_top0, CLK_DIV_TOP0_VAL);
	while (read32(&exynos_clock->div_stat_top0))
		;

	write32(&exynos_clock->div_top1, CLK_DIV_TOP1_VAL);
	while (read32(&exynos_clock->div_stat_top1))
		;

	write32(&exynos_clock->src_lex, CLK_SRC_LEX_VAL);
	while (1) {
		val = read32(&exynos_clock->mux_stat_lex);
		if (val == (val | 1))
			break;
	}

	write32(&exynos_clock->div_lex, CLK_DIV_LEX_VAL);
	while (read32(&exynos_clock->div_stat_lex))
		;

	write32(&exynos_clock->div_r0x, CLK_DIV_R0X_VAL);
	while (read32(&exynos_clock->div_stat_r0x))
		;

	write32(&exynos_clock->div_r0x, CLK_DIV_R0X_VAL);
	while (read32(&exynos_clock->div_stat_r0x))
		;

	write32(&exynos_clock->div_r1x, CLK_DIV_R1X_VAL);
	while (read32(&exynos_clock->div_stat_r1x))
		;

	if (mem->use_bpll) {
		write32(&exynos_clock->src_cdrex, MUX_BPLL_SEL_MASK |
			MUX_MCLK_CDREX_SEL | MUX_MCLK_DPHY_SEL);
	} else {
		write32(&exynos_clock->src_cdrex, CLK_REG_DISABLE);
	}

	write32(&exynos_clock->div_cdrex, CLK_DIV_CDREX_VAL);
	while (read32(&exynos_clock->div_stat_cdrex))
		;

	val = read32(&exynos_clock->src_cpu);
	val |= CLK_SRC_CPU_VAL;
	write32(&exynos_clock->src_cpu, val);

	val = read32(&exynos_clock->src_top2);
	val |= CLK_SRC_TOP2_VAL;
	write32(&exynos_clock->src_top2, val);

	val = read32(&exynos_clock->src_core1);
	val |= CLK_SRC_CORE1_VAL;
	write32(&exynos_clock->src_core1, val);

	write32(&exynos_clock->src_fsys, CLK_SRC_FSYS0_VAL);
	write32(&exynos_clock->div_fsys0, CLK_DIV_FSYS0_VAL);
	while (read32(&exynos_clock->div_stat_fsys0))
		;

	write32(&exynos_clock->clkout_cmu_cpu, CLK_REG_DISABLE);
	write32(&exynos_clock->clkout_cmu_core, CLK_REG_DISABLE);
	write32(&exynos_clock->clkout_cmu_acp, CLK_REG_DISABLE);
	write32(&exynos_clock->clkout_cmu_top, CLK_REG_DISABLE);
	write32(&exynos_clock->clkout_cmu_lex, CLK_REG_DISABLE);
	write32(&exynos_clock->clkout_cmu_r0x, CLK_REG_DISABLE);
	write32(&exynos_clock->clkout_cmu_r1x, CLK_REG_DISABLE);
	write32(&exynos_clock->clkout_cmu_cdrex, CLK_REG_DISABLE);

	write32(&exynos_clock->src_peric0, CLK_SRC_PERIC0_VAL);
	write32(&exynos_clock->div_peric0, CLK_DIV_PERIC0_VAL);

	write32(&exynos_clock->src_peric1, CLK_SRC_PERIC1_VAL);
	write32(&exynos_clock->div_peric1, CLK_DIV_PERIC1_VAL);
	write32(&exynos_clock->div_peric2, CLK_DIV_PERIC2_VAL);
	write32(&exynos_clock->sclk_src_isp, SCLK_SRC_ISP_VAL);
	write32(&exynos_clock->sclk_div_isp, SCLK_DIV_ISP_VAL);
	write32(&exynos_clock->div_isp0, CLK_DIV_ISP0_VAL);
	write32(&exynos_clock->div_isp1, CLK_DIV_ISP1_VAL);
	write32(&exynos_clock->div_isp2, CLK_DIV_ISP2_VAL);

	/* FIMD1 SRC CLK SELECTION */
	write32(&exynos_clock->src_disp1_0, CLK_SRC_DISP1_0_VAL);

	val = MMC2_PRE_RATIO_VAL << MMC2_PRE_RATIO_OFFSET
		| MMC2_RATIO_VAL << MMC2_RATIO_OFFSET
		| MMC3_PRE_RATIO_VAL << MMC3_PRE_RATIO_OFFSET
		| MMC3_RATIO_VAL << MMC3_RATIO_OFFSET;
	write32(&exynos_clock->div_fsys2, val);
}

void clock_gate(void)
{
	/* CLK_GATE_IP_SYSRGT */
	clrbits32(&exynos_clock->gate_ip_sysrgt, CLK_C2C_MASK);

	/* CLK_GATE_IP_ACP */
	clrbits32(&exynos_clock->gate_ip_acp, CLK_SMMUG2D_MASK |
					      CLK_SMMUSSS_MASK |
					      CLK_SMMUMDMA_MASK |
					      CLK_ID_REMAPPER_MASK |
					      CLK_G2D_MASK |
					      CLK_SSS_MASK |
					      CLK_MDMA_MASK |
					      CLK_SECJTAG_MASK);

	/* CLK_GATE_BUS_SYSLFT */
	clrbits32(&exynos_clock->gate_bus_syslft, CLK_EFCLK_MASK);

	/* CLK_GATE_IP_ISP0 */
	clrbits32(&exynos_clock->gate_ip_isp0, CLK_UART_ISP_MASK |
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
	clrbits32(&exynos_clock->gate_ip_isp1, CLK_SPI1_ISP_MASK |
					       CLK_SPI0_ISP_MASK |
					       CLK_SMMU3DNR_MASK |
					       CLK_SMMUDIS1_MASK |
					       CLK_SMMUDIS0_MASK |
					       CLK_SMMUODC_MASK |
					       CLK_3DNR_MASK |
					       CLK_DIS_MASK |
					       CLK_ODC_MASK);

	/* CLK_GATE_SCLK_ISP */
	clrbits32(&exynos_clock->gate_sclk_isp, SCLK_MPWM_ISP_MASK);

	/* CLK_GATE_IP_GSCL */
	clrbits32(&exynos_clock->gate_ip_gscl, CLK_SMMUFIMC_LITE2_MASK |
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
	clrbits32(&exynos_clock->gate_ip_disp1, CLK_SMMUTVX_MASK |
						CLK_ASYNCTVX_MASK |
						CLK_HDMI_MASK |
						CLK_MIXER_MASK |
						CLK_DSIM1_MASK);

	/* CLK_GATE_IP_MFC */
	clrbits32(&exynos_clock->gate_ip_mfc, CLK_SMMUMFCR_MASK |
					      CLK_SMMUMFCL_MASK |
					      CLK_MFC_MASK);

	/* CLK_GATE_IP_GEN */
	clrbits32(&exynos_clock->gate_ip_gen, CLK_SMMUMDMA1_MASK |
					      CLK_SMMUJPEG_MASK |
					      CLK_SMMUROTATOR_MASK |
					      CLK_MDMA1_MASK |
					      CLK_JPEG_MASK |
					      CLK_ROTATOR_MASK);

	/* CLK_GATE_IP_FSYS */
	clrbits32(&exynos_clock->gate_ip_fsys, CLK_WDT_IOP_MASK |
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
	clrbits32(&exynos_clock->gate_ip_peric, CLK_HS_I2C3_MASK |
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
	clrbits32(&exynos_clock->gate_ip_peris, CLK_RTC_MASK |
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
	clrbits32(&exynos_clock->gate_block, CLK_ACP_MASK);

	/* CLK_GATE_IP_CDREX */
	clrbits32(&exynos_clock->gate_ip_cdrex, CLK_DPHY0_MASK |
						CLK_DPHY1_MASK |
						CLK_TZASC_DRBXR_MASK);

}

void clock_init_dp_clock(void)
{
	/* DP clock enable */
	setbits32(&exynos_clock->gate_ip_disp1, CLK_GATE_DP1_ALLOW);

	/* We run DP at 267 Mhz */
	setbits32(&exynos_clock->div_disp1_0, CLK_DIV_DISP1_0_FIMD1);
}
