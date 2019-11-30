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
 */

#include <device/mmio.h>
#include <assert.h>
#include <console/console.h>
#include <soc/clk.h>
#include <soc/periph.h>
#include <timer.h>

/* input clock of PLL: SMDK5420 has 24MHz input clock */
#define CONF_SYS_CLK_FREQ            24000000

/* Epll Clock division values to achieve different frequency output */
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
	unsigned long r, m, p, s, k = 0, mask, fout;
	unsigned int freq;

	switch (pllreg) {
	case APLL:
		r = read32(&exynos_clock->apll_con0);
		break;
	case MPLL:
		r = read32(&exynos_clock->mpll_con0);
		break;
	case EPLL:
		r = read32(&exynos_clock->epll_con0);
		k = read32(&exynos_clock->epll_con1);
		break;
	case VPLL:
		r = read32(&exynos_clock->vpll_con0);
		k = read32(&exynos_clock->vpll_con1);
		break;
	case BPLL:
		r = read32(&exynos_clock->bpll_con0);
		break;
	case RPLL:
		r = read32(&exynos_clock->rpll_con0);
		k = read32(&exynos_clock->rpll_con1);
		break;
	case SPLL:
		r = read32(&exynos_clock->spll_con0);
		break;
	case CPLL:
		r = read32(&exynos_clock->cpll_con0);
		break;
	case DPLL:
		r = read32(&exynos_clock->dpll_con0);
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

	freq = CONF_SYS_CLK_FREQ;

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

enum peripheral_clock_select {
	PERIPH_SRC_CPLL = 1,
	PERIPH_SRC_DPLL = 2,
	PERIPH_SRC_MPLL = 3,
	PERIPH_SRC_SPLL = 4,
	PERIPH_SRC_IPLL = 5,
	PERIPH_SRC_EPLL = 6,
	PERIPH_SRC_RPLL = 7,
};

static int clock_select_to_pll(enum peripheral_clock_select sel)
{
	int pll;

	switch (sel) {
	case PERIPH_SRC_CPLL:
		pll = CPLL;
		break;
	case PERIPH_SRC_DPLL:
		pll = DPLL;
		break;
	case PERIPH_SRC_MPLL:
		pll = MPLL;
		break;
	case PERIPH_SRC_SPLL:
		pll = SPLL;
		break;
	case PERIPH_SRC_IPLL:
		pll = IPLL;
		break;
	case PERIPH_SRC_EPLL:
		pll = EPLL;
		break;
	case PERIPH_SRC_RPLL:
		pll = RPLL;
		break;
	default:
		pll = -1;
		break;
	}

	return pll;
}

unsigned long clock_get_periph_rate(enum periph_id peripheral)
{
	unsigned long sclk;
	unsigned int div;
	int src;

	switch (peripheral) {
	case PERIPH_ID_UART0:
		src = (read32(&exynos_clock->clk_src_peric0) >> 4) & 0x7;
		div = (read32(&exynos_clock->clk_div_peric0) >> 8) & 0xf;
		break;
	case PERIPH_ID_UART1:
		src = (read32(&exynos_clock->clk_src_peric0) >> 8) & 0x7;
		div = (read32(&exynos_clock->clk_div_peric0) >> 12) & 0xf;
		break;
	case PERIPH_ID_UART2:
		src = (read32(&exynos_clock->clk_src_peric0) >> 12) & 0x7;
		div = (read32(&exynos_clock->clk_div_peric0) >> 16) & 0xf;
		break;
	case PERIPH_ID_UART3:
		src = (read32(&exynos_clock->clk_src_peric0) >> 16) & 0x7;
		div = (read32(&exynos_clock->clk_div_peric0) >> 20) & 0xf;
		break;
	case PERIPH_ID_PWM0:
	case PERIPH_ID_PWM1:
	case PERIPH_ID_PWM2:
	case PERIPH_ID_PWM3:
	case PERIPH_ID_PWM4:
		src = (read32(&exynos_clock->clk_src_peric0) >> 24) & 0x7;
		div = (read32(&exynos_clock->clk_div_peric0) >> 28) & 0x7;
		break;
	case PERIPH_ID_SPI0:
		src = (read32(&exynos_clock->clk_src_peric1) >> 20) & 0x7;
		div = (read32(&exynos_clock->clk_div_peric1) >> 20) & 0xf;
		break;
	case PERIPH_ID_SPI1:
		src = (read32(&exynos_clock->clk_src_peric1) >> 24) & 0x7;
		div = (read32(&exynos_clock->clk_div_peric1) >> 24) & 0xf;
		break;
	case PERIPH_ID_SPI2:
		src = (read32(&exynos_clock->clk_src_peric1) >> 28) & 0x7;
		div = (read32(&exynos_clock->clk_div_peric1) >> 28) & 0xf;
		break;
	case PERIPH_ID_SPI3:	/* aka SPI0_ISP */
		src = (read32(&exynos_clock->clk_src_isp) >> 16) & 0x7;
		div = (read32(&exynos_clock->clk_div_isp0) >> 0) & 0x7;
		break;
	case PERIPH_ID_SPI4:	/* aka SPI1_ISP */
		src = (read32(&exynos_clock->clk_src_isp) >> 12) & 0x7;
		div = (read32(&exynos_clock->clk_div_isp1) >> 4) & 0x7;
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
		/*
		 * I2C block parent clock selection is different from other
		 * peripherals, so we handle it all here.
		 * TODO: Add a helper function like with the peripheral clock
		 * select fields?
		 */
		src = (read32(&exynos_clock->clk_src_top1) >> 8) & 0x3;
		if (src == 0x0)
			src = CPLL;
		else if (src == 0x1)
			src = DPLL;
		else if (src == 0x2)
			src = MPLL;
		else
			return -1;

		sclk = get_pll_clk(src);
		div = ((read32(&exynos_clock->clk_div_top1) >> 8) & 0x3f) + 1;
		return sclk / div;
	default:
		printk(BIOS_DEBUG, "%s: invalid peripheral %d",
				__func__, peripheral);
		return -1;
	};

	src = clock_select_to_pll(src);
	if (src < 0) {
		printk(BIOS_DEBUG, "%s: cannot determine source PLL", __func__);
		return -1;
	}

	sclk = get_pll_clk(src);

	return sclk / (div + 1);
}

/* exynos5: return ARM clock frequency */
unsigned long get_arm_clk(void)
{
	unsigned long div;
	unsigned long armclk;
	unsigned int arm_ratio;
	unsigned int arm2_ratio;

	div = read32(&exynos_clock->clk_div_cpu0);

	/* ARM_RATIO: [2:0], ARM2_RATIO: [30:28] */
	arm_ratio = (div >> 0) & 0x7;
	arm2_ratio = (div >> 28) & 0x7;

	armclk = get_pll_clk(APLL) / (arm_ratio + 1);
	armclk /= (arm2_ratio + 1);

	return armclk;
}

/* exynos5: get the mmc clock */
static unsigned long get_mmc_clk(int dev_index)
{
	unsigned long uclk, sclk;
	unsigned int sel, ratio;
	int shift = 0;

	sel = read32(&exynos_clock->clk_src_fsys);
	sel = (sel >> ((dev_index * 4) + 8)) & 0x7;

	if (sel == 0x3)
		sclk = get_pll_clk(MPLL);
	else if (sel == 0x6)
		sclk = get_pll_clk(EPLL);
	else
		return 0;

	ratio = read32(&exynos_clock->clk_div_fsys1);

	shift = dev_index * 10;

	ratio = (ratio >> shift) & 0x3ff;
	uclk = (sclk / (ratio + 1));
	printk(BIOS_DEBUG, "%s(%d): %lu\n", __func__, dev_index, uclk);

	return uclk;
}

/* exynos5: set the mmc clock */
void set_mmc_clk(int dev_index, unsigned int div)
{
	void *addr;
	unsigned int val, shift;

	addr = &exynos_clock->clk_div_fsys1;
	shift = dev_index * 10;

	val = read32(addr);
	val &= ~(0x3ff << shift);
	val |= (div & 0x3ff) << shift;
	write32(addr, val);
}

/* Set DW MMC Controller clock */
int clock_set_dwmci(enum periph_id peripheral)
{
	/* Request MMC clock value to 52MHz. */
	const unsigned long freq = 52000000;
	unsigned long sdclkin, cclkin;
	int device_index = (int)peripheral - (int)PERIPH_ID_SDMMC0;

	ASSERT(device_index >= 0 && device_index < 4);
	sdclkin = get_mmc_clk(device_index);
	if (!sdclkin) {
		return -1;
	}

	/* The SDCLKIN is divided inside the controller by the DIVRATIO field in
	 * CLKSEL register, so we must calculate clock value as
	 *   cclk_in = SDCLKIN / (DIVRATIO + 1)
	 * Currently the RIVRATIO must be 3 for MMC0 and MMC2 on Exynos5420
	 * (and must be configured in payload).
	 */
	if (device_index == 0 || device_index == 2){
		int divratio = 3;
		sdclkin /= (divratio + 1);
	}
	printk(BIOS_DEBUG, "%s(%d): sdclkin: %ld\n", __func__, device_index, sdclkin);

	cclkin = DIV_ROUND_UP(sdclkin, freq);
	set_mmc_clk(device_index, cclkin);
	return 0;
}

void clock_ll_set_pre_ratio(enum periph_id periph_id, unsigned int divisor)
{
	unsigned int shift;
	unsigned int mask = 0xff;
	u32 *reg;

	/*
	 * For now we only handle a very small subset of peripherals here.
	 * Others will need to (and do) mangle the clock registers
	 * themselves, At some point it is hoped that this function can work
	 * from a table or calculated register offset / mask. For now this
	 * is at least better than spreading clock control code around
	 * U-Boot.
	 */
	switch (periph_id) {
	case PERIPH_ID_SPI0:
		reg = &exynos_clock->clk_div_peric4;
		shift = 8;
		break;
	case PERIPH_ID_SPI1:
		reg = &exynos_clock->clk_div_peric4;
		shift = 16;
		break;
	case PERIPH_ID_SPI2:
		reg = &exynos_clock->clk_div_peric4;
		shift = 24;
		break;
	case PERIPH_ID_SPI3:
		reg = &exynos_clock->clk_div_isp1;
		shift = 0;
		break;
	case PERIPH_ID_SPI4:
		reg = &exynos_clock->clk_div_isp1;
		shift = 8;
		break;
	default:
		printk(BIOS_DEBUG, "%s: Unsupported peripheral ID %d\n", __func__,
		      periph_id);
		return;
	}
	clrsetbits32(reg, mask << shift, (divisor & mask) << shift);
}

void clock_ll_set_ratio(enum periph_id periph_id, unsigned int divisor)
{
	unsigned int shift;
	unsigned int mask = 0xf;
	u32 *reg;

	switch (periph_id) {
	case PERIPH_ID_SPI0:
		reg = &exynos_clock->clk_div_peric1;
		shift = 20;
		break;
	case PERIPH_ID_SPI1:
		reg = &exynos_clock->clk_div_peric1;
		shift = 24;
		break;
	case PERIPH_ID_SPI2:
		reg = &exynos_clock->clk_div_peric1;
		shift = 28;
		break;
	case PERIPH_ID_SPI3:
		reg = &exynos_clock->clk_div_isp1;
		shift = 16;
		break;
	case PERIPH_ID_SPI4:
		reg = &exynos_clock->clk_div_isp1;
		shift = 20;
		break;
	default:
		printk(BIOS_DEBUG, "%s: Unsupported peripheral ID %d\n", __func__,
		      periph_id);
		return;
	}
	clrsetbits32(reg, mask << shift, (divisor & mask) << shift);
}

/**
 * Linearly searches for the most accurate main and fine stage clock scalars
 * (divisors) for a specified target frequency and scalar bit sizes by checking
 * all multiples of main_scalar_bits values. Will always return scalars up to or
 * slower than target.
 *
 * @param main_scalar_bits	Number of main scalar bits, must be > 0 and < 32
 * @param fine_scalar_bits	Number of fine scalar bits, must be > 0 and < 32
 * @param input_rate		Clock frequency to be scaled in Hz
 * @param target_rate		Desired clock frequency in Hz
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
	int main_scalar;
	unsigned int fine;

	switch (periph_id) {
	case PERIPH_ID_SPI0:
	case PERIPH_ID_SPI1:
	case PERIPH_ID_SPI2:
	case PERIPH_ID_SPI3:
	case PERIPH_ID_SPI4:
		main_scalar = clock_calc_best_scalar(4, 8, 400000000, rate, &fine);
		if (main_scalar < 0) {
			printk(BIOS_DEBUG, "%s: Cannot set clock rate for periph %d",
					__func__, periph_id);
			return -1;
		}
		clock_ll_set_ratio(periph_id, main_scalar - 1);
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
		addr = &exynos_clock->clk_div_fsys1;
		break;
	case PERIPH_ID_SDMMC2:
		addr = &exynos_clock->clk_div_fsys2;
		break;
	default:
		printk(BIOS_DEBUG, "invalid peripheral\n");
		return -1;
	}
	tmp = read32(addr) & ~0xff0f;
	for (i = 0; i <= 0xf; i++) {
		if ((clock / (i + 1)) <= 400) {
			write32(addr, tmp | i << 0);
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
	struct stopwatch sw;

	epll_con = read32(&exynos_clock->epll_con0);
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
	 * Required period (in cycles) to generate a stable clock output.
	 * The maximum clock time can be up to 3000 * PDIV cycles of PLLs
	 * frequency input (as per spec)
	 */
	lockcnt = 3000 * epll_div[i].p_div;

	write32(&exynos_clock->epll_lock, lockcnt);
	write32(&exynos_clock->epll_con0, epll_con);
	write32(&exynos_clock->epll_con1, epll_con_k);

	stopwatch_init_msecs_expire(&sw, TIMEOUT_EPLL_LOCK);

	while (!(read32(&exynos_clock->epll_con0) &
			(0x1 << EXYNOS5_EPLLCON0_LOCKED_SHIFT))) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_DEBUG, "%s: Timeout waiting for EPLL lock\n", __func__);
			return -1;
		}
	}

	return 0;
}

void clock_select_i2s_clk_source(void)
{
	clrsetbits32(&exynos_clock->clk_src_peric1, AUDIO1_SEL_MASK,
			(CLK_SRC_SCLK_EPLL));
}

int clock_set_i2s_clk_prescaler(unsigned int src_frq, unsigned int dst_frq)
{
	unsigned int div ;

	if ((dst_frq == 0) || (src_frq == 0)) {
		printk(BIOS_DEBUG, "%s: Invalid frequency input for prescaler\n", __func__);
		printk(BIOS_DEBUG, "src frq = %d des frq = %d ", src_frq, dst_frq);
		return -1;
	}

	div = (src_frq / dst_frq);
	if (div > AUDIO_1_RATIO_MASK) {
		printk(BIOS_DEBUG, "%s: Frequency ratio is out of range\n", __func__);
		printk(BIOS_DEBUG, "src frq = %d des frq = %d ", src_frq, dst_frq);
		return -1;
	}
	clrsetbits32(&exynos_clock->clk_div_peric4, AUDIO_1_RATIO_MASK,
				(div & AUDIO_1_RATIO_MASK));
	return 0;
}
