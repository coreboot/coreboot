/*
 * Helpers for clock control and gating on Allwinner CPUs
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "clock.h"

#include <arch/io.h>

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

	addr = (void *)A1X_CCM_BASE  + (periph >> 5);
	reg32 = read32(addr);
	reg32 |= 1 << (periph & 0x1f);
	write32(reg32, addr);
}

/**
 * \brief Disable the clock source for the peripheral
 *
 * @param[in] periph peripheral and clock type to enable @see a1x_clken
 */
void a1x_periph_clock_disable(enum a1x_clken periph)
{
	void *addr;
	u32 reg32;

	addr = (void *)A1X_CCM_BASE  + (periph >> 5);
	reg32 = read32(addr);
	reg32 &= ~(1 << (periph & 0x1f));
	write32(reg32, addr);
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
 * The output clocks are give by the following formulas:
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
	write32(reg32, &ccm->pll5_cfg);
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
