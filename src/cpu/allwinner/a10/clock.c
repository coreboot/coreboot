/*
 * Helpers for clock control and gating on Allwinner CPUs
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "clock.h"

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <lib.h>
#include <stdlib.h>

static struct a10_ccm *const ccm = (void *)A1X_CCM_BASE;

/**
 * \brief Enable the clock source for the peripheral
 *
 * @param[in] periph peripheral and clock type to enable @see a1x_clken
 */
void a1x_periph_clock_enable(enum a1x_clken periph)
{
	void *addr;
	u32 reg32;

	addr = (void *)A1X_CCM_BASE + (periph >> 5);
	reg32 = read32(addr);
	reg32 |= 1 << (periph & 0x1f);
	write32(addr, reg32);
}

/**
 * \brief Disable the clock source for the peripheral
 *
 * @param[in] periph peripheral and clock type to disable @see a1x_clken
 */
void a1x_periph_clock_disable(enum a1x_clken periph)
{
	void *addr;
	u32 reg32;

	addr = (void *)A1X_CCM_BASE + (periph >> 5);
	reg32 = read32(addr);
	reg32 &= ~(1 << (periph & 0x1f));
	write32(addr, reg32);
}

/**
 * \brief Configure PLL5 factors
 *
 * This is a low-level accessor to configure the divisors and multipliers of
 * PLL5. PLL5 uses two factors to multiply the 24MHz oscillator clock to
 * generate a pre-clock. The pre-divided clock is then divided by one of two
 * independent divisors, one for DRAM, and another for peripherals clocked from
 * this PLL. If the PLL was previously disabled, this function will enable it.
 * Other than that, this function only modifies these factors, and leaves the
 * other settings unchanged.
 *
 * The output clocks are given by the following formulas:
 *
 * Pre-clock    = (24 MHz * N * K) <- Must be between 240MHz and 2GHz
 * DRAM clock   = pre / M
 * Other module = pre / P
 *
 * It is the caller's responsibility to make sure the pre-divided clock falls
 * within the operational range of the PLL, and that the divisors and
 * multipliers are within their ranges.
 *
 * @param[in] mul_n Multiplier N, between 0 and 32
 * @param[in] mul_k Multiplier K, between 1 and 4
 * @param[in] div_m DRAM clock divisor, between 1 and 4
 * @param[in] exp_div_p Peripheral clock divisor exponent, between 0 and 3
 *			(P = 1/2/4/8, respectively)
 */
void a1x_pll5_configure(u8 mul_n, u8 mul_k, u8 div_m, u8 exp_div_p)
{
	u32 reg32;

	reg32 = read32(&ccm->pll5_cfg);
	reg32 &= ~(PLL5_FACTOR_M_MASK | PLL5_FACTOR_N_MASK |
		   PLL5_FACTOR_K_MASK | PLL5_DIV_EXP_P_MASK);
	/* The M1 factor is not documented in the datasheet, and the reference
	 * raminit code does not use it. Whether this is a fractional divisor,
	 * or an additional divisor is unknown, so don't use it for now */
	reg32 &= ~PLL5_FACTOR_M1_MASK;
	reg32 |= (PLL5_FACTOR_M(div_m) | PLL5_FACTOR_N(mul_n) |
		  PLL5_FACTOR_K(mul_k) | PLL5_DIV_EXP_P(exp_div_p));
	reg32 |= PLL5_PLL_ENABLE;
	write32(&ccm->pll5_cfg, reg32);
}

/**
 * \brief Enable the clock output to DRAM chips
 *
 * This enables the DRAM clock to be sent to DRAM chips. This should normally be
 * done after PLL5 is configured and locked. Note that the clock may be gated,
 * and also needs to be ungated in order to reach the DDR chips.
 * Also see @ref clock_ungate_dram_clk_output
 */
void a1x_pll5_enable_dram_clock_output(void)
{
	setbits_le32(&ccm->pll5_cfg, PLL5_DDR_CLK_OUT_EN);
}

/**
 * \brief Ungate the clock to DRAM chips
 *
 * Although the DRAM clock output may be enabled, it is by default gated. It
 * needs to be ungated before reaching DRAM.
 */
void a1x_ungate_dram_clock_output(void)
{
	setbits_le32(&ccm->dram_clk_cfg, DRAM_CTRL_DCLK_OUT);
}

/**
 * \brief Gate the clock to DRAM chips
 *
 * Disable the clock to DRAM without altering PLL configuration, by closing the
 * DRAM clock gate.
 */
void a1x_gate_dram_clock_output(void)
{
	clrbits_le32(&ccm->dram_clk_cfg, DRAM_CTRL_DCLK_OUT);
}

/*
 * Linker doesn't garbage collect and the function below adds about half
 * kilobyte to the bootblock, and log2_ceil is not available in the bootblock.
 */
#ifndef __BOOTBLOCK__

#define PLL1_CFG(N, K, M, P_EXP)	\
	((1 << 31 | 0 << 30 | 8 << 26 | 0 << 25 | 16 << 20 | 2 << 13) | \
	(P_EXP) << 16 |  (N) << 8 | \
	(K - 1) << 4 | 0 << 3 | 0 << 2 | (M -1) << 0)

static const struct {
	u32 pll1_cfg;
	u16 freq_mhz;
} pll1_table[] = {
	/* PLL1 output = (24MHz * N * K) / (M * P) */
	{ PLL1_CFG(16, 1, 1, 0),  384 },
	{ PLL1_CFG(16, 2, 1, 0),  768 },
	{ PLL1_CFG(20, 2, 1, 0),  960 },
	{ PLL1_CFG(21, 2, 1, 0), 1008 },
	{ PLL1_CFG(22, 2, 1, 0), 1056 },
	{ PLL1_CFG(23, 2, 1, 0), 1104 },
	{ PLL1_CFG(24, 2, 1, 0), 1152 },
	{ PLL1_CFG(25, 2, 1, 0), 1200 },
	{ PLL1_CFG(26, 2, 1, 0), 1248 },
	{ PLL1_CFG(27, 2, 1, 0), 1296 },
	{ PLL1_CFG(28, 2, 1, 0), 1344 },
	{ PLL1_CFG(29, 2, 1, 0), 1392 },
	{ PLL1_CFG(30, 2, 1, 0), 1440 },
	{ PLL1_CFG(31, 2, 1, 0), 1488 },
	{ PLL1_CFG(20, 4, 1, 0), 1944 },
};

static void cpu_clk_src_switch(u32 clksel_bits)
{
	u32 reg32;

	reg32 = read32(&ccm->cpu_ahb_apb0_cfg);
	reg32 &= ~CPU_CLK_SRC_MASK;
	reg32 |= clksel_bits & CPU_CLK_SRC_MASK;
	write32(&ccm->cpu_ahb_apb0_cfg, reg32);
}

static void change_sys_divisors(u8 axi, u8 ahb_exp, u8 apb0_exp)
{
	u32 reg32;

	reg32 = read32(&ccm->cpu_ahb_apb0_cfg);
	/* Not a typo: We want to keep only the CLK_SRC bits */
	reg32 &= CPU_CLK_SRC_MASK;
	reg32 |= ((axi - 1) << 0) & AXI_DIV_MASK;
	reg32 |= (ahb_exp << 4) & AHB_DIV_MASK;
	reg32 |= (apb0_exp << 8) & APB0_DIV_MASK;
	write32(&ccm->cpu_ahb_apb0_cfg, reg32);
}

static void spin_delay(u32 loops)
{
	volatile u32 x = loops;
	while (x--);
}

/**
 * \brief Configure the CPU clock and PLL1
 *
 * To run at full speed, the CPU uses PLL1 as the clock source. AXI, AHB, and
 * APB0 are derived from the CPU clock, and need to be kept within certain
 * limits. This function configures PLL1 as close as possible to the desired
 * frequency, based on a set of known working configurations for PLL1. It then
 * calculates and applies the appropriate divisors for the AXI/AHB/APB0 clocks,
 * before finally switching the CPU to run from the new clock.
 * No further configuration of the CPU clock or divisors is needed. after
 * calling this function.
 *
 * @param[in] cpu_clk_mhz Desired CPU clock, in MHz
 */
void a1x_set_cpu_clock(u16 cpu_clk_mhz)
{
	int i = 0;
	u8 axi, ahb, ahb_exp, apb0, apb0_exp;
	u32 actual_mhz;

	/*
	 * Rated clock for PLL1 is 2000 MHz, but there is no combination of
	 * parameters that yields that exact frequency. 1944 MHz is the highest.
	 */
	if (cpu_clk_mhz > 1944) {
		printk(BIOS_CRIT, "BUG! maximum PLL1 clock is 1944 MHz,"
				  "but asked to clock CPU at %d MHz\n",
				  cpu_clk_mhz);
		cpu_clk_mhz = 1944;
	}
	/* Find target frequency */
	while (pll1_table[i].freq_mhz < cpu_clk_mhz)
		i++;

	actual_mhz = pll1_table[i].freq_mhz;

	if (cpu_clk_mhz != actual_mhz) {
		printk(BIOS_WARNING, "Parameters for %d MHz not available, "
				     "setting CPU clock at %d MHz\n",
				     cpu_clk_mhz, actual_mhz);
	}

	/*
	 * Calculate system clock divisors:
	 * The minimum clock divisor for APB0 is 2, which guarantees that AHB0
	 * will always be in spec, as long as AHB is in spec, although the max
	 * AHB0 clock we can get is 125 MHz
	 */
	axi = CEIL_DIV(actual_mhz, 450);	/* Max 450 MHz */
	ahb = CEIL_DIV(actual_mhz/axi, 250);	/* Max 250 MHz */
	apb0 = 2;				/* Max 150 MHz */

	ahb_exp = log2_ceil(ahb);
	ahb = 1 << ahb_exp;
	apb0_exp = 1;

	printk(BIOS_INFO, "CPU: %d MHz, AXI %d Mhz, AHB: %d MHz APB0: %d MHz\n",
			  actual_mhz,
			  actual_mhz / axi,
			  actual_mhz / (axi * ahb),
			  actual_mhz / (axi * ahb * apb0));

	/* Keep the CPU off PLL1 while we change PLL parameters */
	cpu_clk_src_switch(CPU_CLK_SRC_OSC24M);
	/*
	 * We can't use udelay() here. udelay() relies on timer 0, but timers
	 * have the habit of not ticking when the CPU is clocked from the main
	 * oscillator.
	 */
	spin_delay(8);

	change_sys_divisors(axi, ahb_exp, apb0_exp);

	/* Configure PLL1 at the desired frequency */
	write32(&ccm->pll1_cfg, pll1_table[i].pll1_cfg);
	spin_delay(8);

	cpu_clk_src_switch(CPU_CLK_SRC_PLL1);
	/* Here, we're running from PLL, so timers will tick */
	udelay(1);
}

#endif  /* __BOOTBLOCK__ */
