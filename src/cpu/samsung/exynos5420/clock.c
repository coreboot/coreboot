/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Samsung Electronics
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

#include <console/console.h>
#include <stdlib.h>
#include <assert.h>
#include <arch/io.h>
#include "timer.h"
#include "clk.h"
#include "cpu.h"
#include "periph.h"

/* input clock of PLL: SMDK5420 has 24MHz input clock */
#define CONFIG_SYS_CLK_FREQ            24000000

/* src_bit div_bit prediv_bit */
static struct clk_bit_info clk_bit_info[PERIPH_ID_COUNT] = {
	{0,	0,	-1},
	{4,	4,	-1},
	{8,	8,	-1},
	{12,	12,	-1},
	{0,	0,	8},
	{4,	16,	24},
	{8,	0,	8},
	{12,	16,	24},
	{-1,	-1,	-1},
	{16,	0,	8}, /* PERIPH_ID_SROMC */
	{20,	16,	24},
	{24,	0,	8},
	{0,	0,	4},
	{4,	12,	16},
	{-1,	-1,	-1},
	{-1,	-1,	-1},
	[PERIPH_ID_DPHPD]
	{24,	0,	-1},
	{24,	0,	-1},
	{24,	0,	-1},
	{24,	0,	-1},
	{24,	0,	-1},
};

/* Epll Clock division values to achive different frequency output */
static struct st_epll_con_val epll_div[] = {
	{ 192000000, 0, 48, 3, 1, 0 },
	{ 180000000, 0, 45, 3, 1, 0 },
	{  73728000, 1, 73, 3, 3, 47710 },
	{  67737600, 1, 90, 4, 3, 20762 },
	{  49152000, 0, 49, 3, 3, 9961 },
	{  45158400, 0, 45, 3, 3, 10381 },
	{ 180633600, 0, 45, 3, 1, 10381 }
};

/* exynos5: return pll clock frequency */
unsigned long get_pll_clk(int pllreg)
{
	struct exynos5420_clock *clk = samsung_get_base_clock();
	unsigned long r, m, p, s, k = 0, mask, fout;
	unsigned int freq;

	switch (pllreg) {
	case APLL:
		r = readl(&clk->apll_con0);
		break;
	case MPLL:
		r = readl(&clk->mpll_con0);
		break;
	case EPLL:
		r = readl(&clk->epll_con0);
		k = readl(&clk->epll_con1);
		break;
	case VPLL:
		r = readl(&clk->vpll_con0);
		k = readl(&clk->vpll_con1);
		break;
	case BPLL:
		r = readl(&clk->bpll_con0);
		break;
	case RPLL:
		r = readl(&clk->rpll_con0);
		k = readl(&clk->rpll_con1);
		break;
	case SPLL:
		r = readl(&clk->spll_con0);
		break;
	default:
		printk(BIOS_DEBUG, "Unsupported PLL (%d)\n", pllreg);
		return 0;
	}

	/*
	 * APLL_CON: MIDV [25:16]
	 * MPLL_CON: MIDV [25:16]
	 * EPLL_CON: MIDV [24:16]
	 * VPLL_CON: MIDV [24:16]
	 */
	if (pllreg == APLL || pllreg == BPLL || pllreg == MPLL ||
			pllreg == SPLL)
		mask = 0x3ff;
	else
		mask = 0x1ff;

	m = (r >> 16) & mask;

	/* PDIV [13:8] */
	p = (r >> 8) & 0x3f;
	/* SDIV [2:0] */
	s = r & 0x7;

	freq = CONFIG_SYS_CLK_FREQ;

	if (pllreg == EPLL || pllreg == RPLL) {
		k = k & 0xffff;
		/* FOUT = (MDIV + K / 65536) * FIN / (PDIV * 2^SDIV) */
		fout = (m + k / 65536) * (freq / (p * (1 << s)));
	} else if (pllreg == VPLL) {
		k = k & 0xfff;
		/* FOUT = (MDIV + K / 1024) * FIN / (PDIV * 2^SDIV) */
		fout = (m + k / 1024) * (freq / (p * (1 << s)));
	} else {
		/* FOUT = MDIV * FIN / (PDIV * 2^SDIV) */
		fout = m * (freq / (p * (1 << s)));
	}

	return fout;
}

unsigned long clock_get_periph_rate(enum periph_id peripheral)
{
	struct clk_bit_info *bit_info = &clk_bit_info[peripheral];
	unsigned long sclk, sub_clk;
	unsigned int src, div, sub_div;
	struct exynos5420_clock *clk = samsung_get_base_clock();

	switch (peripheral) {
	case PERIPH_ID_UART0:
	case PERIPH_ID_UART1:
	case PERIPH_ID_UART2:
	case PERIPH_ID_UART3:
		src = readl(&clk->clk_src_peric0);
		div = readl(&clk->clk_div_peric0);
		break;
	case PERIPH_ID_PWM0:
	case PERIPH_ID_PWM1:
	case PERIPH_ID_PWM2:
	case PERIPH_ID_PWM3:
	case PERIPH_ID_PWM4:
		src = readl(&clk->clk_src_peric0);
		div = readl(&clk->clk_div_peric3);
		break;
	case PERIPH_ID_SPI0:
	case PERIPH_ID_SPI1:
		src = readl(&clk->clk_src_peric1);
		div = readl(&clk->clk_div_peric1);
		break;
	case PERIPH_ID_SPI2:
		src = readl(&clk->clk_src_peric1);
		div = readl(&clk->clk_div_peric2);
		break;
	case PERIPH_ID_SPI3:
	case PERIPH_ID_SPI4:
		src = readl(&clk->clk_src_isp);
		div = readl(&clk->clk_div_isp1);
		break;
	case PERIPH_ID_SDMMC0:
	case PERIPH_ID_SDMMC1:
	case PERIPH_ID_SDMMC2:
	case PERIPH_ID_SDMMC3:
		src = readl(&clk->clk_src_fsys);
		div = readl(&clk->clk_div_fsys1);
		break;
	case PERIPH_ID_I2C0:
	case PERIPH_ID_I2C1:
	case PERIPH_ID_I2C2:
	case PERIPH_ID_I2C3:
	case PERIPH_ID_I2C4:
	case PERIPH_ID_I2C5:
	case PERIPH_ID_I2C6:
	case PERIPH_ID_I2C7:
	case PERIPH_ID_I2C8:
	case PERIPH_ID_I2C9:
	case PERIPH_ID_I2C10:
		sclk = get_pll_clk(MPLL);
		div = ((readl(&clk->clk_div_top1) >> 8) & 0x3f) + 1;
		return sclk / div;
	default:
		printk(BIOS_DEBUG, "%s: invalid peripheral %d", __func__, peripheral);
		return -1;
	};

	src = (src >> bit_info->src_bit) & 0xf;

	switch (src) {
	case EXYNOS_SRC_MPLL:
		sclk = get_pll_clk(MPLL);
		break;
	case EXYNOS_SRC_EPLL:
		sclk = get_pll_clk(EPLL);
		break;
	default:
		return 0;
	}

	/* Ratio clock division for this peripheral */
	sub_div = (div >> bit_info->div_bit) & 0xf;
	sub_clk = sclk / (sub_div + 1);

	/* Pre-ratio clock division for SDMMC0 and 2 */
	if (peripheral == PERIPH_ID_SDMMC0 || peripheral == PERIPH_ID_SDMMC2) {
		div = (div >> bit_info->prediv_bit) & 0xff;
		return sub_clk / (div + 1);
	}

	return sub_clk;
}

/* exynos5: return ARM clock frequency */
unsigned long get_arm_clk(void)
{
	struct exynos5420_clock *clk = samsung_get_base_clock();
	unsigned long div;
	unsigned long armclk;
	unsigned int arm_ratio;
	unsigned int arm2_ratio;

	div = readl(&clk->clk_div_cpu0);

	/* ARM_RATIO: [2:0], ARM2_RATIO: [30:28] */
	arm_ratio = (div >> 0) & 0x7;
	arm2_ratio = (div >> 28) & 0x7;

	armclk = get_pll_clk(APLL) / (arm_ratio + 1);
	armclk /= (arm2_ratio + 1);

	return armclk;
}

/* exynos5: set the mmc clock */
void set_mmc_clk(int dev_index, unsigned int div)
{
	struct exynos5420_clock *clk = samsung_get_base_clock();
	void *addr;
	unsigned int val, shift;

	addr = &clk->clk_div_fsys1;
	shift = dev_index * 10;

	val = readl(addr);
	val &= ~(0x3ff << shift);
	val |= (div & 0x3ff) << shift;
	writel(val, addr);
}

void clock_ll_set_pre_ratio(enum periph_id periph_id, unsigned divisor)
{
	struct exynos5420_clock *clk = samsung_get_base_clock();
	unsigned shift;
	unsigned mask = 0xff;
	u32 *reg;

	/*
	 * For now we only handle a very small subset of peipherals here.
	 * Others will need to (and do) mangle the clock registers
	 * themselves, At some point it is hoped that this function can work
	 * from a table or calculated register offset / mask. For now this
	 * is at least better than spreading clock control code around
	 * U-Boot.
	 */
	switch (periph_id) {
	case PERIPH_ID_SPI0:
		reg = &clk->clk_div_peric4;
		shift = 8;
		break;
	case PERIPH_ID_SPI1:
		reg = &clk->clk_div_peric4;
		shift = 16;
		break;
	case PERIPH_ID_SPI2:
		reg = &clk->clk_div_peric4;
		shift = 24;
		break;
	case PERIPH_ID_SPI3:
		reg = &clk->clk_div_isp1;
		shift = 0;
		break;
	case PERIPH_ID_SPI4:
		reg = &clk->clk_div_isp1;
		shift = 8;
		break;
	default:
		printk(BIOS_DEBUG, "%s: Unsupported peripheral ID %d\n", __func__,
		      periph_id);
		return;
	}
	clrsetbits_le32(reg, mask << shift, (divisor & mask) << shift);
}

void clock_ll_set_ratio(enum periph_id periph_id, unsigned divisor)
{
	struct exynos5420_clock *clk = samsung_get_base_clock();
	unsigned shift;
	unsigned mask = 0xff;
	u32 *reg;

	switch (periph_id) {
	case PERIPH_ID_SPI0:
		reg = &clk->clk_div_peric1;
		shift = 20;
		break;
	case PERIPH_ID_SPI1:
		reg = &clk->clk_div_peric1;
		shift = 24;
		break;
	case PERIPH_ID_SPI2:
		reg = &clk->clk_div_peric1;
		shift = 28;
		break;
	case PERIPH_ID_SPI3:
		reg = &clk->clk_div_isp1;
		shift = 16;
		break;
	case PERIPH_ID_SPI4:
		reg = &clk->clk_div_isp1;
		shift = 20;
		break;
	default:
		printk(BIOS_DEBUG, "%s: Unsupported peripheral ID %d\n", __func__,
		      periph_id);
		return;
	}
	clrsetbits_le32(reg, mask << shift, (divisor & mask) << shift);
}

/**
 * Linearly searches for the most accurate main and fine stage clock scalars
 * (divisors) for a specified target frequency and scalar bit sizes by checking
 * all multiples of main_scalar_bits values. Will always return scalars up to or
 * slower than target.
 *
 * @param main_scalar_bits	Number of main scalar bits, must be > 0 and < 32
 * @param fine_scalar_bits	Number of fine scalar bits, must be > 0 and < 32
 * @param input_freq		Clock frequency to be scaled in Hz
 * @param target_freq		Desired clock frequency in Hz
 * @param best_fine_scalar	Pointer to store the fine stage divisor
 *
 * @return best_main_scalar	Main scalar for desired frequency or -1 if none
 * found
 */
static int clock_calc_best_scalar(unsigned int main_scaler_bits,
	unsigned int fine_scalar_bits, unsigned int input_rate,
	unsigned int target_rate, unsigned int *best_fine_scalar)
{
	int i;
	int best_main_scalar = -1;
	unsigned int best_error = target_rate;
	const unsigned int cap = (1 << fine_scalar_bits) - 1;
	const unsigned int loops = 1 << main_scaler_bits;

	printk(BIOS_DEBUG, "Input Rate is %u, Target is %u, Cap is %u\n", input_rate,
			target_rate, cap);

	ASSERT(best_fine_scalar != NULL);
	ASSERT(main_scaler_bits <= fine_scalar_bits);

	*best_fine_scalar = 1;

	if (input_rate == 0 || target_rate == 0)
		return -1;

	if (target_rate >= input_rate)
		return 1;

	for (i = 1; i <= loops; i++) {
		const unsigned int effective_div = MAX(MIN(input_rate / i /
							target_rate, cap), 1);
		const unsigned int effective_rate = input_rate / i /
							effective_div;
		const int error = target_rate - effective_rate;

		printk(BIOS_DEBUG, "%d|effdiv:%u, effrate:%u, error:%d\n", i, effective_div,
				effective_rate, error);

		if (error >= 0 && error <= best_error) {
			best_error = error;
			best_main_scalar = i;
			*best_fine_scalar = effective_div;
		}
	}

	return best_main_scalar;
}

int clock_set_rate(enum periph_id periph_id, unsigned int rate)
{
	int main;
	unsigned int fine;

	switch (periph_id) {
	case PERIPH_ID_SPI0:
	case PERIPH_ID_SPI1:
	case PERIPH_ID_SPI2:
	case PERIPH_ID_SPI3:
	case PERIPH_ID_SPI4:
		main = clock_calc_best_scalar(4, 8, 400000000, rate, &fine);
		if (main < 0) {
			printk(BIOS_DEBUG, "%s: Cannot set clock rate for periph %d",
					__func__, periph_id);
			return -1;
		}
		clock_ll_set_ratio(periph_id, main - 1);
		clock_ll_set_pre_ratio(periph_id, fine - 1);
		break;
	default:
		printk(BIOS_DEBUG, "%s: Unsupported peripheral ID %d\n", __func__,
		      periph_id);
		return -1;
	}

	return 0;
}

int clock_set_mshci(enum periph_id peripheral)
{
	struct exynos5420_clock *clk = samsung_get_base_clock();
	u32 *addr;
	unsigned int clock;
	unsigned int tmp;
	unsigned int i;

	/* get mpll clock */
	clock = get_pll_clk(MPLL) / 1000000;

	/*
	 * CLK_DIV_FSYS1
	 * MMC0_PRE_RATIO [15:8], MMC0_RATIO [3:0]
	 * CLK_DIV_FSYS2
	 * MMC2_PRE_RATIO [15:8], MMC2_RATIO [3:0]
	 */
	switch (peripheral) {
	case PERIPH_ID_SDMMC0:
		addr = &clk->clk_div_fsys1;
		break;
	case PERIPH_ID_SDMMC2:
		addr = &clk->clk_div_fsys2;
		break;
	default:
		printk(BIOS_DEBUG, "invalid peripheral\n");
		return -1;
	}
	tmp = readl(addr) & ~0xff0f;
	for (i = 0; i <= 0xf; i++) {
		if ((clock / (i + 1)) <= 400) {
			writel(tmp | i << 0, addr);
			break;
		}
	}
	return 0;
}

int clock_epll_set_rate(unsigned long rate)
{
	unsigned int epll_con, epll_con_k;
	unsigned int i;
	unsigned int lockcnt;
	unsigned int start;
	struct exynos5420_clock *clk = samsung_get_base_clock();

	epll_con = readl(&clk->epll_con0);
	epll_con &= ~((EPLL_CON0_LOCK_DET_EN_MASK <<
			EPLL_CON0_LOCK_DET_EN_SHIFT) |
		EPLL_CON0_MDIV_MASK << EPLL_CON0_MDIV_SHIFT |
		EPLL_CON0_PDIV_MASK << EPLL_CON0_PDIV_SHIFT |
		EPLL_CON0_SDIV_MASK << EPLL_CON0_SDIV_SHIFT);

	for (i = 0; i < ARRAY_SIZE(epll_div); i++) {
		if (epll_div[i].freq_out == rate)
			break;
	}

	if (i == ARRAY_SIZE(epll_div))
		return -1;

	epll_con_k = epll_div[i].k_dsm << 0;
	epll_con |= epll_div[i].en_lock_det << EPLL_CON0_LOCK_DET_EN_SHIFT;
	epll_con |= epll_div[i].m_div << EPLL_CON0_MDIV_SHIFT;
	epll_con |= epll_div[i].p_div << EPLL_CON0_PDIV_SHIFT;
	epll_con |= epll_div[i].s_div << EPLL_CON0_SDIV_SHIFT;

	/*
	 * Required period ( in cycles) to genarate a stable clock output.
	 * The maximum clock time can be up to 3000 * PDIV cycles of PLLs
	 * frequency input (as per spec)
	 */
	lockcnt = 3000 * epll_div[i].p_div;

	writel(lockcnt, &clk->epll_lock);
	writel(epll_con, &clk->epll_con0);
	writel(epll_con_k, &clk->epll_con1);

	start = get_timer(0);

	 while (!(readl(&clk->epll_con0) &
			(0x1 << EXYNOS5_EPLLCON0_LOCKED_SHIFT))) {
		if (get_timer(start) > TIMEOUT_EPLL_LOCK) {
			printk(BIOS_DEBUG, "%s: Timeout waiting for EPLL lock\n", __func__);
			return -1;
		}
	}

	return 0;
}

void clock_select_i2s_clk_source(void)
{
	struct exynos5420_clock *clk = samsung_get_base_clock();

	clrsetbits_le32(&clk->clk_src_peric1, AUDIO1_SEL_MASK,
			(CLK_SRC_SCLK_EPLL));
}

int clock_set_i2s_clk_prescaler(unsigned int src_frq, unsigned int dst_frq)
{
	struct exynos5420_clock *clk = samsung_get_base_clock();
	unsigned int div ;

	if ((dst_frq == 0) || (src_frq == 0)) {
		printk(BIOS_DEBUG, "%s: Invalid requency input for prescaler\n", __func__);
		printk(BIOS_DEBUG, "src frq = %d des frq = %d ", src_frq, dst_frq);
		return -1;
	}

	div = (src_frq / dst_frq);
	if (div > AUDIO_1_RATIO_MASK) {
		printk(BIOS_DEBUG, "%s: Frequency ratio is out of range\n", __func__);
		printk(BIOS_DEBUG, "src frq = %d des frq = %d ", src_frq, dst_frq);
		return -1;
	}
	clrsetbits_le32(&clk->clk_div_peric4, AUDIO_1_RATIO_MASK,
				(div & AUDIO_1_RATIO_MASK));
	return 0;
}
