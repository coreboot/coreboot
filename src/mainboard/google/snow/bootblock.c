/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 The ChromiumOS Authors.  All rights reserved.
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

#include <stdlib.h>
#include <types.h>
#include <arch/io.h>
#include "cpu/samsung/exynos5250/clk.h"
#include "cpu/samsung/exynos5250/cpu.h"
#include "cpu/samsung/exynos5250/gpio.h"
#include "cpu/samsung/s5p-common/gpio.h"
#include "cpu/samsung/exynos5-common/spi.h"

#define EXYNOS5_CLOCK_BASE		0x10010000

/* FIXME(dhendrix): Can we move this SPI stuff elsewhere? */
static void spi_rx_tx(struct exynos_spi *regs, int todo,
			void *dinp, void const *doutp, int i)
{
	unsigned int *rxp = (unsigned int *)(dinp + (i * (32 * 1024)));
	int rx_lvl, tx_lvl;
	unsigned int out_bytes, in_bytes;

	out_bytes = in_bytes = todo;
	setbits_le32(&regs->ch_cfg, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);
	writel(((todo * 8) / 32) | SPI_PACKET_CNT_EN, &regs->pkt_cnt);

	while (in_bytes) {
		uint32_t spi_sts;
		int temp;

		spi_sts = readl(&regs->spi_sts);
		rx_lvl = ((spi_sts >> 15) & 0x7f);
		tx_lvl = ((spi_sts >> 6) & 0x7f);
		while (tx_lvl < 32 && out_bytes) {
			temp = 0xffffffff;
			writel(temp, &regs->tx_data);
			out_bytes -= 4;
			tx_lvl += 4;
		}
		while (rx_lvl >= 4 && in_bytes) {
			temp = readl(&regs->rx_data);
			if (rxp)
				*rxp++ = temp;
			in_bytes -= 4;
			rx_lvl -= 4;
		}
	}
}

#if 0
void clock_ll_set_pre_ratio(enum periph_id periph_id, unsigned divisor)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
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
		reg = &clk->div_peric1;
		shift = 8;
		break;
	case PERIPH_ID_SPI1:
		reg = &clk->div_peric1;
		shift = 24;
		break;
	case PERIPH_ID_SPI2:
		reg = &clk->div_peric2;
		shift = 8;
		break;
	case PERIPH_ID_SPI3:
		reg = &clk->sclk_div_isp;
		shift = 4;
		break;
	case PERIPH_ID_SPI4:
		reg = &clk->sclk_div_isp;
		shift = 16;
		break;
	default:
		debug("%s: Unsupported peripheral ID %d\n", __func__,
		      periph_id);
		return;
	}
}
#endif
void clock_ll_set_pre_ratio(enum periph_id periph_id, unsigned divisor)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)EXYNOS5_CLOCK_BASE;
	unsigned shift;
	unsigned mask = 0xff;
	u32 *reg;

	reg = &clk->div_peric1;
	shift = 24;
	clrsetbits_le32(reg, mask << shift, (divisor & mask) << shift);
}

#if 0
void clock_ll_set_ratio(enum periph_id periph_id, unsigned divisor)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)samsung_get_base_clock();
	unsigned shift;
	unsigned mask = 0xff;
	u32 *reg;

	switch (periph_id) {
	case PERIPH_ID_SPI0:
		reg = &clk->div_peric1;
		shift = 0;
		break;
	case PERIPH_ID_SPI1:
		reg = &clk->div_peric1;
		shift = 16;
		break;
	case PERIPH_ID_SPI2:
		reg = &clk->div_peric2;
		shift = 0;
		break;
	case PERIPH_ID_SPI3:
		reg = &clk->sclk_div_isp;
		shift = 0;
		break;
	case PERIPH_ID_SPI4:
		reg = &clk->sclk_div_isp;
		shift = 12;
		break;
	default:
		debug("%s: Unsupported peripheral ID %d\n", __func__,
		      periph_id);
		return;
	}
}
#endif
void clock_ll_set_ratio(enum periph_id periph_id, unsigned divisor)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)EXYNOS5_CLOCK_BASE;
	unsigned shift;
	unsigned mask = 0xff;
	u32 *reg;

	reg = &clk->div_peric1;
	shift = 16;
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

#if 0
	debug("Input Rate is %u, Target is %u, Cap is %u\n", input_rate,
			target_rate, cap);

	assert(best_fine_scalar != NULL);
	assert(main_scaler_bits <= fine_scalar_bits);
#endif

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

#if 0
		debug("%d|effdiv:%u, effrate:%u, error:%d\n", i, effective_div,
				effective_rate, error);
#endif

		if (error >= 0 && error <= best_error) {
			best_error = error;
			best_main_scalar = i;
			*best_fine_scalar = effective_div;
		}
	}

	return best_main_scalar;
}

#if 0
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
			debug("%s: Cannot set clock rate for periph %d",
					__func__, periph_id);
			return -1;
		}
		clock_ll_set_ratio(periph_id, main - 1);
		clock_ll_set_pre_ratio(periph_id, fine - 1);
		break;
	default:
		debug("%s: Unsupported peripheral ID %d\n", __func__,
		      periph_id);
		return -1;
	}
	main = clock_calc_best_scalar(4, 8, 400000000, rate, &fine);
	if (main < 0) {
		debug("%s: Cannot set clock rate for periph %d",
				__func__, periph_id);
		return -1;
	}
	clock_ll_set_ratio(PERIPH_ID_SPI1, main - 1);
	clock_ll_set_pre_ratio(PERIPH_ID_SPI1, fine - 1);

	return 0;
}
#endif
int clock_set_rate(enum periph_id periph_id, unsigned int rate)
{
	int main;
	unsigned int fine;

	main = clock_calc_best_scalar(4, 8, 400000000, rate, &fine);
	if (main < 0) {
//		debug("%s: Cannot set clock rate for periph %d",
//				__func__, periph_id);
		return -1;
	}
	clock_ll_set_ratio(-1, main - 1);
	clock_ll_set_pre_ratio(-1, fine - 1);

	return 0;
}

struct gpio_info {
	unsigned int reg_addr;	/* Address of register for this part */
	unsigned int max_gpio;	/* Maximum GPIO in this part */
};

static const struct gpio_info gpio_data[EXYNOS_GPIO_NUM_PARTS] = {
	{ EXYNOS5_GPIO_PART1_BASE, GPIO_MAX_PORT_PART_1 },
	{ EXYNOS5_GPIO_PART2_BASE, GPIO_MAX_PORT_PART_2 },
	{ EXYNOS5_GPIO_PART3_BASE, GPIO_MAX_PORT_PART_3 },
	{ EXYNOS5_GPIO_PART4_BASE, GPIO_MAX_PORT_PART_4 },
	{ EXYNOS5_GPIO_PART5_BASE, GPIO_MAX_PORT_PART_5 },
	{ EXYNOS5_GPIO_PART6_BASE, GPIO_MAX_PORT },
};

static struct s5p_gpio_bank *gpio_get_bank(unsigned int gpio)
{
	const struct gpio_info *data;
	unsigned int upto;
	int i;

	for (i = upto = 0, data = gpio_data; i < EXYNOS_GPIO_NUM_PARTS;
			i++, upto = data->max_gpio, data++) {
		if (gpio < data->max_gpio) {
			struct s5p_gpio_bank *bank;

			bank = (struct s5p_gpio_bank *)data->reg_addr;
			bank += (gpio - upto) / GPIO_PER_BANK;
			return bank;
		}
	}

#ifndef CONFIG_SPL_BUILD
	assert(gpio < GPIO_MAX_PORT);	/* ...which it will not be */
#endif
	return NULL;
}

#define CON_MASK(x)		(0xf << ((x) << 2))
#define CON_SFR(x, v)		((v) << ((x) << 2))

/* This macro gets gpio pin offset from 0..7 */
#define GPIO_BIT(x)     ((x) & 0x7)

void gpio_cfg_pin(int gpio, int cfg)
{
	unsigned int value;
	struct s5p_gpio_bank *bank = gpio_get_bank(gpio);

	value = readl(&bank->con);
	value &= ~CON_MASK(GPIO_BIT(gpio));
	value |= CON_SFR(GPIO_BIT(gpio), cfg);
	writel(value, &bank->con);
}

//static void exynos_spi_copy(unsigned int uboot_size)
static void copy_romstage(uint32_t spi_addr, uint32_t sram_addr, unsigned int len)
{
	int upto, todo;
	int i;
//	struct exynos_spi *regs = (struct exynos_spi *)samsung_get_base_spi1();
	struct exynos_spi *regs = (struct exynos_spi *)0x12d30000;

	clock_set_rate(PERIPH_ID_SPI1, 50000000); /* set spi clock to 50Mhz */
	/* set the spi1 GPIO */
//	exynos_pinmux_config(PERIPH_ID_SPI1, PINMUX_FLAG_NONE);
	gpio_cfg_pin(GPIO_A24, 0x2);
	gpio_cfg_pin(GPIO_A25, 0x2);
	gpio_cfg_pin(GPIO_A26, 0x2);
	gpio_cfg_pin(GPIO_A27, 0x2);

	/* set pktcnt and enable it */
	writel(4 | SPI_PACKET_CNT_EN, &regs->pkt_cnt);
	/* set FB_CLK_SEL */
	writel(SPI_FB_DELAY_180, &regs->fb_clk);
	/* set CH_WIDTH and BUS_WIDTH as word */
	setbits_le32(&regs->mode_cfg, SPI_MODE_CH_WIDTH_WORD |
					SPI_MODE_BUS_WIDTH_WORD);
	clrbits_le32(&regs->ch_cfg, SPI_CH_CPOL_L); /* CPOL: active high */

	/* clear rx and tx channel if set priveously */
	clrbits_le32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON);

	setbits_le32(&regs->swap_cfg, SPI_RX_SWAP_EN |
		SPI_RX_BYTE_SWAP |
		SPI_RX_HWORD_SWAP);

	/* do a soft reset */
	setbits_le32(&regs->ch_cfg, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);

	/* now set rx and tx channel ON */
	setbits_le32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON | SPI_CH_HS_EN);
	clrbits_le32(&regs->cs_reg, SPI_SLAVE_SIG_INACT); /* CS low */

	/* Send read instruction (0x3h) followed by a 24 bit addr */
	writel((SF_READ_DATA_CMD << 24) | spi_addr, &regs->tx_data);

	/* waiting for TX done */
	while (!(readl(&regs->spi_sts) & SPI_ST_TX_DONE));

	for (upto = 0, i = 0; upto < len; upto += todo, i++) {
		todo = MIN(len - upto, (1 << 15));
		spi_rx_tx(regs, todo, (void *)(sram_addr),
					(void *)(spi_addr), i);
	}

	setbits_le32(&regs->cs_reg, SPI_SLAVE_SIG_INACT);/* make the CS high */

	/*
	 * Let put controller mode to BYTE as
	 * SPI driver does not support WORD mode yet
	 */
	clrbits_le32(&regs->mode_cfg, SPI_MODE_CH_WIDTH_WORD |
					SPI_MODE_BUS_WIDTH_WORD);
	writel(0, &regs->swap_cfg);

	/*
	 * Flush spi tx, rx fifos and reset the SPI controller
	 * and clear rx/tx channel
	 */
	clrsetbits_le32(&regs->ch_cfg, SPI_CH_HS_EN, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_TX_CH_ON | SPI_RX_CH_ON);
}

void bootblock_mainboard_init(void);
void bootblock_mainboard_init(void)
{
	/* Copy romstage data from SPI ROM to SRAM */
	/* FIXME: magic constant... */
	copy_romstage(0x2000, CONFIG_ROMSTAGE_BASE, CONFIG_XIP_ROM_SIZE);
	
#if 0
	/* FIXME: for debugging... */
	volatile unsigned long *addr = (unsigned long *)0x1004330c;
	*addr |= 0x100;
	while (1);
#endif
}
