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

#define uchar unsigned char
#define uint  unsigned int

#include <stdlib.h>
#include <types.h>
#include <lib.h>
#include <arch/io.h>
#include "cpu/samsung/exynos5250/clk.h"
#include "cpu/samsung/exynos5250/cpu.h"
#include "cpu/samsung/exynos5250/dmc.h"
#include "cpu/samsung/exynos5250/gpio.h"
#include "cpu/samsung/exynos5250/periph.h"
#include "cpu/samsung/exynos5250/setup.h"
#include "cpu/samsung/exynos5250/clock_init.h"
#include "cpu/samsung/s5p-common/gpio.h"
#include "cpu/samsung/s5p-common/s3c24x0_i2c.h"
#include "cpu/samsung/exynos5-common/spi.h"
#include "cpu/samsung/exynos5-common/uart.h"

#include <device/i2c.h>
#include <drivers/maxim/max77686/max77686.h>

#include <console/console.h>

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

#if 0
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
#endif
/* Pull mode */
#define EXYNOS_GPIO_PULL_NONE	0x0
#define EXYNOS_GPIO_PULL_DOWN	0x1
#define EXYNOS_GPIO_PULL_UP	0x3

#define PULL_MASK(x)		(0x3 << ((x) << 1))
#define PULL_MODE(x, v)		((v) << ((x) << 1))

void gpio_set_pull(int gpio, int mode)
{
	unsigned int value;
	struct s5p_gpio_bank *bank = gpio_get_bank(gpio);

	value = readl(&bank->pull);
	value &= ~PULL_MASK(GPIO_BIT(gpio));

	switch (mode) {
	case EXYNOS_GPIO_PULL_DOWN:
	case EXYNOS_GPIO_PULL_UP:
		value |= PULL_MODE(GPIO_BIT(gpio), mode);
		break;
	default:
		break;
	}

	writel(value, &bank->pull);
}

static uint32_t uart3_base = CONFIG_CONSOLE_SERIAL_UART_ADDRESS;

#define CONFIG_SYS_CLK_FREQ            24000000

/* exynos5: return pll clock frequency */
unsigned long get_pll_clk(int pllreg);
unsigned long get_pll_clk(int pllreg)
{
	struct exynos5_clock *clk =
		(struct exynos5_clock *)EXYNOS5_CLOCK_BASE;
	unsigned long r, m, p, s, k = 0, mask, fout;
	unsigned int freq;

	switch (pllreg) {
	case APLL:
		r = readl(&clk->apll_con0);
		break;
	case BPLL:
		r = readl(&clk->bpll_con0);
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
	default:
//		printk(BIOS_DEBUG, "Unsupported PLL (%d)\n", pllreg);
		return 0;
	}

	/*
	 * APLL_CON: MIDV [25:16]
	 * MPLL_CON: MIDV [25:16]
	 * EPLL_CON: MIDV [24:16]
	 * VPLL_CON: MIDV [24:16]
	 */
	if (pllreg == APLL || pllreg == BPLL || pllreg == MPLL)
		mask = 0x3ff;
	else
		mask = 0x1ff;

	m = (r >> 16) & mask;

	/* PDIV [13:8] */
	p = (r >> 8) & 0x3f;
	/* SDIV [2:0] */
	s = r & 0x7;

	freq = CONFIG_SYS_CLK_FREQ;

	if (pllreg == EPLL) {
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

/* src_bit div_bit prediv_bit */
static struct clk_bit_info clk_bit_info[PERIPH_ID_COUNT] = {
	{0,	4,	0,	-1},
	{4,	4,	4,	-1},
	{8,	4,	8,	-1},
	{12,	4,	12,	-1},
	{0,	4,	0,	8},
	{4,	4,	16,	24},
	{8,	4,	0,	8},
	{12,	4,	16,	24},
	{-1,	-1,	-1,	-1},
	{16,	4,	0,	8}, /* PERIPH_ID_SROMC */
	{20,	4,	16,	24},
	{24,	4,	0,	8},
	{0,	4,	0,	4},
	{4,	4,	12,	16},
	{-1,	4,	-1,	-1},
	{-1,	4,	-1,	-1},
	{-1,	4,	24,	0},
	{-1,	4,	24,	0},
	{-1,	4,	24,	0},
	{-1,	4,	24,	0},
	{-1,	4,	24,	0},
	{-1,	4,	24,	0},
	{-1,	4,	24,	0},
	{-1,	4,	24,	0},
	{24,	4,	0,	-1},
	{24,	4,	0,	-1},
	{24,	4,	0,	-1},
	{24,	4,	0,	-1},
	{24,	4,	0,	-1},
	{-1,	-1,	-1,	-1},
	{-1,	-1,	-1,	-1},
	{-1,	-1,	-1,	-1}, /* PERIPH_ID_I2S1 */
	{24,	1,	20,	-1}, /* PERIPH_ID_SATA */
};

static unsigned long my_clock_get_periph_rate(enum periph_id peripheral)
{
//	struct exynos5_clock *clk =
//		(struct exynos5_clock *)samsung_get_base_clock();
	struct exynos5_clock *clk =
		(struct exynos5_clock *)EXYNOS5_CLOCK_BASE;
	struct clk_bit_info *bit_info = &clk_bit_info[peripheral];
//	struct clk_bit_info bit_info = { 12, 4, 12, -1 };
	unsigned long sclk, sub_clk;
	unsigned int src, div, sub_div;

	switch (peripheral) {
	case PERIPH_ID_UART0:
	case PERIPH_ID_UART1:
	case PERIPH_ID_UART2:
	case PERIPH_ID_UART3:
		src = readl(&clk->src_peric0);
		div = readl(&clk->div_peric0);
		break;
	case PERIPH_ID_I2C0:
	case PERIPH_ID_I2C1:
	case PERIPH_ID_I2C2:
	case PERIPH_ID_I2C3:
	case PERIPH_ID_I2C4:
	case PERIPH_ID_I2C5:
	case PERIPH_ID_I2C6:
	case PERIPH_ID_I2C7:
		src = 0;
		sclk = get_pll_clk(MPLL);
		sub_div = ((readl(&clk->div_top1) >> bit_info->div_bit) & 0x7) + 1;
		div = ((readl(&clk->div_top0) >> bit_info->prediv_bit) & 0x7) + 1;
		return (sclk / sub_div) / div;
	default:
		return -1;
	};

	src = (src >> bit_info->src_bit) & ((1 << bit_info->n_src_bits) - 1);
	if (src == SRC_MPLL)
		sclk = get_pll_clk(MPLL);
	else if (src == SRC_EPLL)
		sclk = get_pll_clk(EPLL);
	else if (src == SRC_VPLL)
		sclk = get_pll_clk(VPLL);
	else
		return 0;

	sub_div = (div >> bit_info->div_bit) & 0xf;
	sub_clk = sclk / (sub_div + 1);

	return sub_clk;
}

static void serial_setbrg_dev(void)
{
//	struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	struct s5p_uart *uart = (struct s5p_uart *)uart3_base;
	u32 uclk;
	u32 baudrate = CONFIG_TTYS0_BAUD;
	u32 val;
//	enum periph_id periph;

//	periph = exynos5_get_periph_id(base_port);
	uclk = my_clock_get_periph_rate(PERIPH_ID_UART3);
	val = uclk / baudrate;

	writel(val / 16 - 1, &uart->ubrdiv);

	/*
	 * FIXME(dhendrix): the original uart.h had a "br_rest" value which
	 * does not seem relevant to the exynos5250... not entirely sure
	 * where/if we need to worry about it here
	 */
#if 0
	if (s5p_uart_divslot())
		writew(udivslot[val % 16], &uart->rest.slot);
	else
		writeb(val % 16, &uart->rest.value);
#endif
}

/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 */
static void exynos5_init_dev(void)
{
//	struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	struct s5p_uart *uart = (struct s5p_uart *)uart3_base;

	/* enable FIFOs */
	writel(0x1, &uart->ufcon);
	writel(0, &uart->umcon);
	/* 8N1 */
	writel(0x3, &uart->ulcon);
	/* No interrupts, no DMA, pure polling */
	writel(0x245, &uart->ucon);

	serial_setbrg_dev();
}

static int exynos5_uart_err_check(int op)
{
	//struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	struct s5p_uart *uart = (struct s5p_uart *)uart3_base;
	unsigned int mask;

	/*
	 * UERSTAT
	 * Break Detect	[3]
	 * Frame Err	[2] : receive operation
	 * Parity Err	[1] : receive operation
	 * Overrun Err	[0] : receive operation
	 */
	if (op)
		mask = 0x8;
	else
		mask = 0xf;

	return readl(&uart->uerstat) & mask;
}

#define RX_FIFO_COUNT_MASK	0xff
#define RX_FIFO_FULL_MASK	(1 << 8)
#define TX_FIFO_FULL_MASK	(1 << 24)

#if 0
/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
static unsigned char exynos5_uart_rx_byte(void)
{
//	struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	struct s5p_uart *uart = (struct s5p_uart *)uart3_base;

	/* wait for character to arrive */
	while (!(readl(&uart->ufstat) & (RX_FIFO_COUNT_MASK |
					 RX_FIFO_FULL_MASK))) {
		if (exynos5_uart_err_check(0))
			return 0;
	}

	return readb(&uart->urxh) & 0xff;
}
#endif

/*
 * Output a single byte to the serial port.
 */
static void exynos5_uart_tx_byte(unsigned char data)
{
//	struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	struct s5p_uart *uart = (struct s5p_uart *)uart3_base;

	if (data == '\n')
		exynos5_uart_tx_byte('\r');

	/* wait for room in the tx FIFO */
	while ((readl(uart->ufstat) & TX_FIFO_FULL_MASK)) {
		if (exynos5_uart_err_check(1))
			return;
	}

	writeb(data, &uart->utxh);
}

void puts(const char *s);
void puts(const char *s)
{
	int n = 0;

	while (*s) {
		if (*s == '\n') {
			exynos5_uart_tx_byte(0xd);	/* CR */
		}

		exynos5_uart_tx_byte(*s++);
		n++;
	}
}

static void do_serial(void)
{
	//exynos_pinmux_config(PERIPH_ID_UART3, PINMUX_FLAG_NONE);
	gpio_set_pull(GPIO_A14, EXYNOS_GPIO_PULL_NONE);
	gpio_cfg_pin(GPIO_A15, EXYNOS_GPIO_FUNC(0x2));

	exynos5_init_dev();
}

#define I2C_WRITE	0
#define I2C_READ	1

#define I2C_OK		0
#define I2C_NOK		1
#define I2C_NACK	2
#define I2C_NOK_LA	3	/* Lost arbitration */
#define I2C_NOK_TOUT	4	/* time out */

#define I2CSTAT_BSY	0x20	/* Busy bit */
#define I2CSTAT_NACK	0x01	/* Nack bit */
#define I2CCON_ACKGEN	0x80	/* Acknowledge generation */
#define I2CCON_IRPND	0x10	/* Interrupt pending bit */
#define I2C_MODE_MT	0xC0	/* Master Transmit Mode */
#define I2C_MODE_MR	0x80	/* Master Receive Mode */
#define I2C_START_STOP	0x20	/* START / STOP */
#define I2C_TXRX_ENA	0x10	/* I2C Tx/Rx enable */


/* The timeouts we live by */
enum {
	I2C_XFER_TIMEOUT_MS	= 35,	/* xfer to complete */
	I2C_INIT_TIMEOUT_MS	= 1000,	/* bus free on init */
	I2C_IDLE_TIMEOUT_MS	= 100,	/* waiting for bus idle */
	I2C_STOP_TIMEOUT_US	= 200,	/* waiting for stop events */
};

#define I2C0_BASE	0x12c60000
struct s3c24x0_i2c_bus i2c0 = {
	.node = 0,
	.bus_num = 0,
	.regs = (struct s3c24x0_i2c *)I2C0_BASE,
	.id = PERIPH_ID_I2C0,
};

static void i2c_ch_init(struct s3c24x0_i2c *i2c, int speed, int slaveadd)
{
	unsigned long freq, pres = 16, div;

	freq = my_clock_get_periph_rate(PERIPH_ID_I2C0);
	/* calculate prescaler and divisor values */
	if ((freq / pres / (16 + 1)) > speed)
		/* set prescaler to 512 */
		pres = 512;

	div = 0;

	while ((freq / pres / (div + 1)) > speed)
		div++;

	/* set prescaler, divisor according to freq, also set ACKGEN, IRQ */
	writel((div & 0x0F) | 0xA0 | ((pres == 512) ? 0x40 : 0), &i2c->iiccon);

	/* init to SLAVE REVEIVE and set slaveaddr */
	writel(0, &i2c->iicstat);
	writel(slaveadd, &i2c->iicadd);
	/* program Master Transmit (and implicit STOP) */
	writel(I2C_MODE_MT | I2C_TXRX_ENA, &i2c->iicstat);
}

static void i2c_bus_init(struct s3c24x0_i2c_bus *i2c, unsigned int bus)
{
//	exynos_pinmux_config(i2c->id, 0);
	gpio_cfg_pin(GPIO_B30, EXYNOS_GPIO_FUNC(0x2));
	gpio_cfg_pin(GPIO_B31, EXYNOS_GPIO_FUNC(0x2));
	gpio_set_pull(GPIO_B30, EXYNOS_GPIO_PULL_NONE);
	gpio_set_pull(GPIO_B31, EXYNOS_GPIO_PULL_NONE);

	i2c_ch_init(i2c->regs, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
}

void do_barriers(void);
void do_barriers(void)
{
	/*
	 * The reason we don't write out the instructions dsb/isb/sev:
	 * While ARM Cortex-A8 supports ARM v7 instruction set (-march=armv7a),
	 * we compile with -march=armv5 to allow more compilers to work.
	 * For U-Boot code this has no performance impact.
	 */
	__asm__ __volatile__(
#if defined(__thumb__)
	".hword 0xF3BF, 0x8F4F\n"  /* dsb; darn -march=armv5 */
	".hword 0xF3BF, 0x8F6F\n"  /* isb; darn -march=armv5 */
	".hword 0xBF40\n"          /* sev; darn -march=armv5 */
#else
	".word  0xF57FF04F\n"      /* dsb; darn -march=armv5 */
	".word  0xF57FF06F\n"      /* isb; darn -march=armv5 */
	".word  0xE320F004\n"      /* sev; darn -march=armv5 */
#endif
	);
}

void sdelay(unsigned long loops);
void sdelay(unsigned long loops)
{
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n"
			  "bne 1b":"=r" (loops):"0"(loops));
}

/* is this right? meh, it seems to work well enough... */
void my_udelay(unsigned int n);
void my_udelay(unsigned int n)
{
	sdelay(n * 1000);
}


void i2c_init(int speed, int slaveadd)
{
	struct s3c24x0_i2c_bus *i2c = &i2c0;
	struct exynos5_gpio_part1 *gpio;
	int i;
	uint32_t x;

#if 0
	/* By default i2c channel 0 is the current bus */
	g_current_bus = 0;

	i2c = get_bus(g_current_bus);
	if (!i2c)
		return;
#endif

	i2c_bus_init(i2c, 0);

	/* wait for some time to give previous transfer a chance to finish */
	i = I2C_INIT_TIMEOUT_MS * 20;
	while ((readl(&i2c->regs->iicstat) & I2CSTAT_BSY) && (i > 0)) {
		my_udelay(50);
		i--;
	}

	gpio = (struct exynos5_gpio_part1 *)(EXYNOS5_GPIO_PART1_BASE);
	/* FIXME(dhendrix): cannot use nested macro (compilation failure) */
//	writel((readl(&gpio->b3.con) & ~0x00FF) | 0x0022, &gpio->b3.con);
	x = readl(&gpio->b3.con);
	writel((x & ~0x00FF) | 0x0022, &gpio->b3.con);

	i2c_ch_init(i2c->regs, speed, slaveadd);
}

static int WaitForXfer(struct s3c24x0_i2c *i2c)
{
	int i;

	i = I2C_XFER_TIMEOUT_MS * 20;
	while (!(readl(&i2c->iiccon) & I2CCON_IRPND)) {
		if (i == 0) {
			//debug("%s: i2c xfer timeout\n", __func__);
			return I2C_NOK_TOUT;
		}
		my_udelay(50);
		i--;
	}

	return I2C_OK;
}

static int IsACK(struct s3c24x0_i2c *i2c)
{
	return !(readl(&i2c->iicstat) & I2CSTAT_NACK);
}

static void ReadWriteByte(struct s3c24x0_i2c *i2c)
{
	uint32_t x;

	x = readl(&i2c->iiccon);
	writel(x & ~I2CCON_IRPND, &i2c->iiccon);
	/* FIXME(dhendrix): cannot use nested macro (compilation failure) */
//	writel(readl(&i2c->iiccon) & ~I2CCON_IRPND, &i2c->iiccon);
}

/*
 * Verify the whether I2C ACK was received or not
 *
 * @param i2c	pointer to I2C register base
 * @param buf	array of data
 * @param len	length of data
 * return	I2C_OK when transmission done
 *		I2C_NACK otherwise
 */
static int i2c_send_verify(struct s3c24x0_i2c *i2c, unsigned char buf[],
			   unsigned char len)
{
	int i, result = I2C_OK;

	if (IsACK(i2c)) {
		for (i = 0; (i < len) && (result == I2C_OK); i++) {
			writel(buf[i], &i2c->iicds);
			ReadWriteByte(i2c);
			result = WaitForXfer(i2c);
			if (result == I2C_OK && !IsACK(i2c))
				result = I2C_NACK;
		}
	} else {
		result = I2C_NACK;
	}

	return result;
}

/*
 * Send a STOP event and wait for it to have completed
 *
 * @param mode	If it is a master transmitter or receiver
 * @return I2C_OK if the line became idle before timeout I2C_NOK_TOUT otherwise
 */
static int i2c_send_stop(struct s3c24x0_i2c *i2c, int mode)
{
	int timeout;

	/* Setting the STOP event to fire */
	writel(mode | I2C_TXRX_ENA, &i2c->iicstat);
	ReadWriteByte(i2c);

	/* Wait for the STOP to send and the bus to go idle */
	for (timeout = I2C_STOP_TIMEOUT_US; timeout > 0; timeout -= 5) {
		if (!(readl(&i2c->iicstat) & I2CSTAT_BSY))
			return I2C_OK;
		my_udelay(5);
	}

	return I2C_NOK_TOUT;
}

/*
 * cmd_type is 0 for write, 1 for read.
 *
 * addr_len can take any value from 0-255, it is only limited
 * by the char, we could make it larger if needed. If it is
 * 0 we skip the address write cycle.
 */
static int i2c_transfer(struct s3c24x0_i2c *i2c,
			unsigned char cmd_type,
			unsigned char chip,
			unsigned char addr[],
			unsigned char addr_len,
			unsigned char data[],
			unsigned short data_len)
{
	int i, result, stop_bit_result;
	uint32_t x;

	if (data == 0 || data_len == 0) {
		/* Don't support data transfer of no length or to address 0 */
		//debug("i2c_transfer: bad call\n");
		return I2C_NOK;
	}

	/* Check I2C bus idle */
	i = I2C_IDLE_TIMEOUT_MS * 20;
	while ((readl(&i2c->iicstat) & I2CSTAT_BSY) && (i > 0)) {
		my_udelay(50);
		i--;
	}

	if (readl(&i2c->iicstat) & I2CSTAT_BSY) {
		//debug("%s: bus busy\n", __func__);
		return I2C_NOK_TOUT;
	}

	/* FIXME(dhendrix): cannot use nested macro (compilation failure) */
	//writel(readl(&i2c->iiccon) | I2CCON_ACKGEN, &i2c->iiccon);
	x = readl(&i2c->iiccon);
	writel(x | I2CCON_ACKGEN, &i2c->iiccon);

	if (addr && addr_len) {
		writel(chip, &i2c->iicds);
		/* send START */
		writel(I2C_MODE_MT | I2C_TXRX_ENA | I2C_START_STOP,
			&i2c->iicstat);
		if (WaitForXfer(i2c) == I2C_OK)
			result = i2c_send_verify(i2c, addr, addr_len);
		else
			result = I2C_NACK;
	} else
		result = I2C_NACK;

	switch (cmd_type) {
	case I2C_WRITE:
		if (result == I2C_OK)
			result = i2c_send_verify(i2c, data, data_len);
		else {
			writel(chip, &i2c->iicds);
			/* send START */
			writel(I2C_MODE_MT | I2C_TXRX_ENA | I2C_START_STOP,
				&i2c->iicstat);
			if (WaitForXfer(i2c) == I2C_OK)
				result = i2c_send_verify(i2c, data, data_len);
		}

		if (result == I2C_OK)
			result = WaitForXfer(i2c);

		stop_bit_result = i2c_send_stop(i2c, I2C_MODE_MT);
		break;

	case I2C_READ:
	{
		int was_ok = (result == I2C_OK);

		writel(chip, &i2c->iicds);
		/* resend START */
		writel(I2C_MODE_MR | I2C_TXRX_ENA |
					I2C_START_STOP, &i2c->iicstat);
		ReadWriteByte(i2c);
		result = WaitForXfer(i2c);

		if (was_ok || IsACK(i2c)) {
			i = 0;
			while ((i < data_len) && (result == I2C_OK)) {
				/* disable ACK for final READ */
				if (i == data_len - 1) {
					/* FIXME(dhendrix): nested macro */
#if 0
					writel(readl(&i2c->iiccon) &
					       ~I2CCON_ACKGEN,
					       &i2c->iiccon);
#endif
					x = readl(&i2c->iiccon) & ~I2CCON_ACKGEN;
					writel(x, &i2c->iiccon);
				}
				ReadWriteByte(i2c);
				result = WaitForXfer(i2c);
				data[i] = readl(&i2c->iicds);
				i++;
			}
		} else {
			result = I2C_NACK;
		}

		stop_bit_result = i2c_send_stop(i2c, I2C_MODE_MR);
		break;
	}

	default:
		//debug("i2c_transfer: bad call\n");
		result = stop_bit_result = I2C_NOK;
		break;
	}

	/*
	 * If the transmission went fine, then only the stop bit was left to
	 * fail.  Otherwise, the real failure we're interested in came before
	 * that, during the actual transmission.
	 */
	return (result == I2C_OK) ? stop_bit_result : result;
}


int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	struct s3c24x0_i2c_bus *i2c = &i2c0;
	uchar xaddr[4];
	int ret;

	if (alen > 4) {
		//debug("I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (alen > 0) {
		xaddr[0] = (addr >> 24) & 0xFF;
		xaddr[1] = (addr >> 16) & 0xFF;
		xaddr[2] = (addr >> 8) & 0xFF;
		xaddr[3] = addr & 0xFF;
	}

#ifdef CONFIG_SYS_I2C_EEPROM_ADDR_OVERFLOW
	/*
	 * EEPROM chips that implement "address overflow" are ones
	 * like Catalyst 24WC04/08/16 which has 9/10/11 bits of
	 * address and the extra bits end up in the "chip address"
	 * bit slots. This makes a 24WC08 (1Kbyte) chip look like
	 * four 256 byte chips.
	 *
	 * Note that we consider the length of the address field to
	 * still be one byte because the extra address bits are
	 * hidden in the chip address.
	 */
	if (alen > 0)
		chip |= ((addr >> (alen * 8)) &
			 CONFIG_SYS_I2C_EEPROM_ADDR_OVERFLOW);
#endif
	if (!i2c)
		return -1;
#if 0
	if (board_i2c_claim_bus(i2c->node)) {
		debug("I2C cannot claim bus %d\n", i2c->bus_num);
		return -1;
	}
#endif
	ret = i2c_transfer(i2c->regs, I2C_READ, chip << 1, &xaddr[4 - alen],
			   alen, buffer, len);
	//board_i2c_release_bus(i2c->node);
	if (ret) {
		//debug("I2c read: failed %d\n", ret);
		return 1;
	}
	return 0;
}

int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	struct s3c24x0_i2c_bus *i2c;
	uchar xaddr[4];
	int ret;

	if (alen > 4) {
		//debug("I2C write: addr len %d not supported\n", alen);
		return 1;
	}

	if (alen > 0) {
		xaddr[0] = (addr >> 24) & 0xFF;
		xaddr[1] = (addr >> 16) & 0xFF;
		xaddr[2] = (addr >> 8) & 0xFF;
		xaddr[3] = addr & 0xFF;
	}
#ifdef CONFIG_SYS_I2C_EEPROM_ADDR_OVERFLOW
	/*
	 * EEPROM chips that implement "address overflow" are ones
	 * like Catalyst 24WC04/08/16 which has 9/10/11 bits of
	 * address and the extra bits end up in the "chip address"
	 * bit slots. This makes a 24WC08 (1Kbyte) chip look like
	 * four 256 byte chips.
	 *
	 * Note that we consider the length of the address field to
	 * still be one byte because the extra address bits are
	 * hidden in the chip address.
	 */
	if (alen > 0)
		chip |= ((addr >> (alen * 8)) &
			 CONFIG_SYS_I2C_EEPROM_ADDR_OVERFLOW);
#endif
	//i2c = get_bus(g_current_bus);
	i2c = &i2c0;
	if (!i2c)
		return -1;
#if 0
	if (board_i2c_claim_bus(i2c->node)) {
		//debug("I2C cannot claim bus %d\n", i2c->bus_num);
		return -1;
	}
#endif
	ret = i2c_transfer(i2c->regs, I2C_WRITE, chip << 1, &xaddr[4 - alen],
			   alen, buffer, len);
	//board_i2c_release_bus(i2c->node);

	return ret != 0;
}

/*
 * Max77686 parameters values
 * see max77686.h for parameters details
 */
struct max77686_para max77686_param[] = {/*{vol_addr, vol_bitpos,
	vol_bitmask, reg_enaddr, reg_enbitpos, reg_enbitmask, reg_enbiton,
	reg_enbitoff, vol_min, vol_div}*/
	{/* PMIC_BUCK1 */ 0x11, 0x0, 0x3F, 0x10, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK2 */ 0x14, 0x0, 0xFF, 0x12, 0x4, 0x3, 0x1, 0x0, 600, 12500},
	{/* PMIC_BUCK3 */ 0x1E, 0x0, 0xFF, 0x1C, 0x4, 0x3, 0x1, 0x0, 600, 12500},
	{/* PMIC_BUCK4 */ 0x28, 0x0, 0xFF, 0x26, 0x4, 0x3, 0x1, 0x0, 600, 12500},
	{/* PMIC_BUCK5 */ 0x31, 0x0, 0x3F, 0x30, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK6 */ 0x33, 0x0, 0x3F, 0x32, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK7 */ 0x35, 0x0, 0x3F, 0x34, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK8 */ 0x37, 0x0, 0x3F, 0x36, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK9 */ 0x39, 0x0, 0x3F, 0x38, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_LDO1 */  0x40, 0x0, 0x3F, 0x40, 0x6, 0x3, 0x3, 0x0, 800, 25000},
	{/* PMIC_LDO2 */  0x41, 0x0, 0x3F, 0x41, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO3 */  0x42, 0x0, 0x3F, 0x42, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO4 */  0x43, 0x0, 0x3F, 0x43, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO5 */  0x44, 0x0, 0x3F, 0x44, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO6 */  0x45, 0x0, 0x3F, 0x45, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO7 */  0x46, 0x0, 0x3F, 0x46, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO8 */  0x47, 0x0, 0x3F, 0x47, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO9 */  0x48, 0x0, 0x3F, 0x48, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO10 */ 0x49, 0x0, 0x3F, 0x49, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO11 */ 0x4A, 0x0, 0x3F, 0x4A, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO12 */ 0x4B, 0x0, 0x3F, 0x4B, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO13 */ 0x4C, 0x0, 0x3F, 0x4C, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO14 */ 0x4D, 0x0, 0x3F, 0x4D, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO15 */ 0x4E, 0x0, 0x3F, 0x4E, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO16 */ 0x4F, 0x0, 0x3F, 0x4F, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO17 */ 0x50, 0x0, 0x3F, 0x50, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO18 */ 0x51, 0x0, 0x3F, 0x51, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO19 */ 0x52, 0x0, 0x3F, 0x52, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO20 */ 0x53, 0x0, 0x3F, 0x53, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO21 */ 0x54, 0x0, 0x3F, 0x54, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO22 */ 0x55, 0x0, 0x3F, 0x55, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO23 */ 0x56, 0x0, 0x3F, 0x56, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO24 */ 0x57, 0x0, 0x3F, 0x57, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO25 */ 0x58, 0x0, 0x3F, 0x58, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO26 */ 0x59, 0x0, 0x3F, 0x59, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_EN32KHZ_CP */ 0x0, 0x0, 0x0, 0x7F, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0},
};

/*
 * Write a value to a register
 *
 * @param chip_addr	i2c addr for max77686
 * @param reg		reg number to write
 * @param val		value to be written
 *
 */
static inline int max77686_i2c_write(unsigned char chip_addr,
					unsigned int reg, unsigned char val)
{
	return i2c_write(chip_addr, reg, 1, &val, 1);
}

/*
 * Read a value from a register
 *
 * @param chip_addr	i2c addr for max77686
 * @param reg		reg number to write
 * @param val		value to be written
 *
 */
static inline int max77686_i2c_read(unsigned char chip_addr,
					unsigned int reg, unsigned char *val)
{
	return i2c_read(chip_addr, reg, 1, val, 1);
}

/* Chip register numbers (not exported from this module) */
enum {
	REG_BBAT		= 0x7e,

	/* Bits for BBAT */
	BBAT_BBCHOSTEN_MASK	= 1 << 0,
	BBAT_BBCVS_SHIFT	= 3,
	BBAT_BBCVS_MASK		= 3 << BBAT_BBCVS_SHIFT,
};

int max77686_disable_backup_batt(void)
{
	unsigned char val;
	int ret;

	//i2c_set_bus_num(0);
	ret = max77686_i2c_read(MAX77686_I2C_ADDR, REG_BBAT, &val);
	if (ret) {
//		debug("max77686 i2c read failed\n");
		return ret;
	}

	/* If we already have the correct values, exit */
	if ((val & (BBAT_BBCVS_MASK | BBAT_BBCHOSTEN_MASK)) ==
			BBAT_BBCVS_MASK)
		return 0;

	/* First disable charging */
	val &= ~BBAT_BBCHOSTEN_MASK;
	ret = max77686_i2c_write(MAX77686_I2C_ADDR, REG_BBAT, val);
	if (ret) {
		//debug("max77686 i2c write failed\n");
		return -1;
	}

	/* Finally select 3.5V to minimize power consumption */
	val |= BBAT_BBCVS_MASK;
	ret = max77686_i2c_write(MAX77686_I2C_ADDR, REG_BBAT, val);
	if (ret) {
		//debug("max77686 i2c write failed\n");
		return -1;
	}

	return 0;
}

static int max77686_enablereg(enum max77686_regnum reg, int enable)
{
	struct max77686_para *pmic;
	unsigned char read_data;
	int ret;

	pmic = &max77686_param[reg];

	ret = max77686_i2c_read(MAX77686_I2C_ADDR, pmic->reg_enaddr,
				&read_data);
	if (ret != 0) {
		//debug("max77686 i2c read failed.\n");
		return -1;
	}

	if (enable == REG_DISABLE) {
		clrbits_8(&read_data,
				pmic->reg_enbitmask << pmic->reg_enbitpos);
	} else {
		clrsetbits_8(&read_data,
				pmic->reg_enbitmask << pmic->reg_enbitpos,
				pmic->reg_enbiton << pmic->reg_enbitpos);
	}

	ret = max77686_i2c_write(MAX77686_I2C_ADDR,
				 pmic->reg_enaddr, read_data);
	if (ret != 0) {
		//debug("max77686 i2c write failed.\n");
		return -1;
	}

	return 0;
}

static int max77686_do_volsetting(enum max77686_regnum reg, unsigned int volt,
				  int enable, int volt_units)
{
	struct max77686_para *pmic;
	unsigned char read_data;
	int vol_level = 0;
	int ret;

	pmic = &max77686_param[reg];

	if (pmic->vol_addr == 0) {
		//debug("not a voltage register.\n");
		return -1;
	}

	ret = max77686_i2c_read(MAX77686_I2C_ADDR, pmic->vol_addr, &read_data);
	if (ret != 0) {
		//debug("max77686 i2c read failed.\n");
		return -1;
	}

	if (volt_units == MAX77686_UV)
		vol_level = volt - (u32)pmic->vol_min * 1000;
	else
		vol_level = (volt - (u32)pmic->vol_min) * 1000;

	if (vol_level < 0) {
		//debug("Not a valid voltage level to set\n");
		return -1;
	}
	vol_level /= (u32)pmic->vol_div;

	clrsetbits_8(&read_data, pmic->vol_bitmask << pmic->vol_bitpos,
			vol_level << pmic->vol_bitpos);

	ret = max77686_i2c_write(MAX77686_I2C_ADDR, pmic->vol_addr, read_data);
	if (ret != 0) {
		//debug("max77686 i2c write failed.\n");
		return -1;
	}

	ret = max77686_enablereg(reg, enable);
	if (ret != 0) {
		//debug("Failed to enable buck/ldo.\n");
		return -1;
	}
	return 0;
}

int max77686_volsetting(enum max77686_regnum reg, unsigned int volt,
			int enable, int volt_units)
{
//	int old_bus = i2c_get_bus_num();
	int ret;

//	i2c_set_bus_num(0);
	ret = max77686_do_volsetting(reg, volt, enable, volt_units);
//	i2c_set_bus_num(old_bus);
	return ret;
}

/* power_init() stuff */
static void power_init(void)
{
	max77686_disable_backup_batt();

#if 0
	/* FIXME: for debugging... */
	volatile unsigned long *addr = (unsigned long *)0x1004330c;
	*addr |= 0x100;
	while (1);
#endif

	max77686_volsetting(PMIC_BUCK2, CONFIG_VDD_ARM_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_BUCK3, CONFIG_VDD_INT_UV,
						REG_ENABLE, MAX77686_UV);
	max77686_volsetting(PMIC_BUCK1, CONFIG_VDD_MIF_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_BUCK4, CONFIG_VDD_G3D_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_LDO2, CONFIG_VDD_LDO2_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_LDO3, CONFIG_VDD_LDO3_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_LDO5, CONFIG_VDD_LDO5_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_LDO10, CONFIG_VDD_LDO10_MV,
						REG_ENABLE, MAX77686_MV);
}

struct mem_timings mem_timings[] = {
	{
		.mem_manuf = MEM_MANUF_ELPIDA,
		.mem_type = DDR_MODE_DDR3,
		.frequency_mhz = 800,
		.mpll_mdiv = 0x64,
		.mpll_pdiv = 0x3,
		.mpll_sdiv = 0x0,
		.cpll_mdiv = 0xde,
		.cpll_pdiv = 0x4,
		.cpll_sdiv = 0x2,
		.gpll_mdiv = 0x215,
		.gpll_pdiv = 0xc,
		.gpll_sdiv = 0x1,
		.epll_mdiv = 0x60,
		.epll_pdiv = 0x3,
		.epll_sdiv = 0x3,
		.vpll_mdiv = 0x96,
		.vpll_pdiv = 0x3,
		.vpll_sdiv = 0x2,

		.bpll_mdiv = 0x64,
		.bpll_pdiv = 0x3,
		.bpll_sdiv = 0x0,
		.use_bpll = 0,
		.pclk_cdrex_ratio = 0x5,
		.direct_cmd_msr = {
			0x00020018, 0x00030000, 0x00010042, 0x00000d70
		},
		.timing_ref = 0x000000bb,
		.timing_row = 0x8c36660f,
		.timing_data = 0x3630580b,
		.timing_power = 0x41000a44,
		.phy0_dqs = 0x08080808,
		.phy1_dqs = 0x08080808,
		.phy0_dq = 0x08080808,
		.phy1_dq = 0x08080808,
		.phy0_tFS = 0x4,
		.phy1_tFS = 0x4,
		.phy0_pulld_dqs = 0xf,
		.phy1_pulld_dqs = 0xf,

		.lpddr3_ctrl_phy_reset = 0x1,
		.ctrl_start_point = 0x10,
		.ctrl_inc = 0x10,
		.ctrl_start = 0x1,
		.ctrl_dll_on = 0x1,
		.ctrl_ref = 0x8,

		.ctrl_force = 0x1a,
		.ctrl_rdlat = 0x0b,
		.ctrl_bstlen = 0x08,

		.fp_resync = 0x8,
		.iv_size = 0x7,
		.dfi_init_start = 1,
		.aref_en = 1,

		.rd_fetch = 0x3,

		.zq_mode_dds = 0x7,
		.zq_mode_term = 0x1,
		.zq_mode_noterm = 0,

		/*
		* Dynamic Clock: Always Running
		* Memory Burst length: 8
		* Number of chips: 1
		* Memory Bus width: 32 bit
		* Memory Type: DDR3
		* Additional Latancy for PLL: 0 Cycle
		*/
		.memcontrol = DMC_MEMCONTROL_CLK_STOP_DISABLE |
			DMC_MEMCONTROL_DPWRDN_DISABLE |
			DMC_MEMCONTROL_DPWRDN_ACTIVE_PRECHARGE |
			DMC_MEMCONTROL_TP_DISABLE |
			DMC_MEMCONTROL_DSREF_ENABLE |
			DMC_MEMCONTROL_ADD_LAT_PALL_CYCLE(0) |
			DMC_MEMCONTROL_MEM_TYPE_DDR3 |
			DMC_MEMCONTROL_MEM_WIDTH_32BIT |
			DMC_MEMCONTROL_NUM_CHIP_1 |
			DMC_MEMCONTROL_BL_8 |
			DMC_MEMCONTROL_PZQ_DISABLE |
			DMC_MEMCONTROL_MRR_BYTE_7_0,
		.memconfig = DMC_MEMCONFIGx_CHIP_MAP_INTERLEAVED |
			DMC_MEMCONFIGx_CHIP_COL_10 |
			DMC_MEMCONFIGx_CHIP_ROW_15 |
			DMC_MEMCONFIGx_CHIP_BANK_8,
		.membaseconfig0 = DMC_MEMBASECONFIG_VAL(0x40),
		.membaseconfig1 = DMC_MEMBASECONFIG_VAL(0x80),
		.prechconfig_tp_cnt = 0xff,
		.dpwrdn_cyc = 0xff,
		.dsref_cyc = 0xffff,
		.concontrol = DMC_CONCONTROL_DFI_INIT_START_DISABLE |
			DMC_CONCONTROL_TIMEOUT_LEVEL0 |
			DMC_CONCONTROL_RD_FETCH_DISABLE |
			DMC_CONCONTROL_EMPTY_DISABLE |
			DMC_CONCONTROL_AREF_EN_DISABLE |
			DMC_CONCONTROL_IO_PD_CON_DISABLE,
		.dmc_channels = 2,
		.chips_per_channel = 2,
		.chips_to_configure = 1,
		.send_zq_init = 1,
		.impedance = IMP_OUTPUT_DRV_30_OHM,
		.gate_leveling_enable = 0,
	},
};

struct arm_clk_ratios arm_clk_ratios[] = {
	{
		.arm_freq_mhz = 1700,

		.apll_mdiv = 0x1a9,
		.apll_pdiv = 0x6,
		.apll_sdiv = 0x0,

		.arm2_ratio = 0x0,
		.apll_ratio = 0x3,
		.pclk_dbg_ratio = 0x1,
		.atb_ratio = 0x6,
		.periph_ratio = 0x7,
		.acp_ratio = 0x7,
		.cpud_ratio = 0x3,
		.arm_ratio = 0x0,
	}
};

static void clock_init(void)
{
	struct exynos5_clock *clk = (struct exynos5_clock *)EXYNOS5_CLOCK_BASE;
	struct exynos5_mct_regs *mct_regs =
		(struct exynos5_mct_regs *)EXYNOS5_MULTI_CORE_TIMER_BASE;
	struct mem_timings *mem = &mem_timings[0];
	struct arm_clk_ratios *arm_clk_ratio = &arm_clk_ratios[0];
	u32 val, tmp;

	/* Turn on the MCT as early as possible. */
	mct_regs->g_tcon |= (1 << 8);

//	mem = clock_get_mem_timings();
//	arm_clk_ratio = get_arm_ratios();

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

#include <console/vtxprintf.h>
#include <string.h>
#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */
/* haha, don't need ctype.c */
#define isdigit(c)	((c) >= '0' && (c) <= '9')
#define is_digit isdigit
#define isxdigit(c)	(((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))

void __div0 (void);
void __div0 (void)
{
	puts("divide by zero detected");
	while(1) ;
}

static int skip_atoi(const char **s)
{
	int i=0;

	while (is_digit(**s))
		i = i*10 + *((*s)++) - '0';
	return i;
}

#include <div64.h>
static int number(void (*tx_byte)(unsigned char byte),
	unsigned long long num, int base, int size, int precision, int type)
{
	char c,sign,tmp[66];
	const char *digits="0123456789abcdefghijklmnopqrstuvwxyz";
	int i;
	int count = 0;

	if (type & LARGE)
		digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if ((signed long long)num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0) {
		tmp[i++]='0';
	} else while (num != 0) {
		/* FIXME: do_div was broken */
//		tmp[i++] = digits[do_div(num,base)];
		tmp[i++] = digits[num & 0xf];
		num = num >> 4;
	}
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT)))
		while(size-->0)
			tx_byte(' '), count++;
	if (sign)
		tx_byte(sign), count++;
	if (type & SPECIAL) {
		if (base==8)
			tx_byte('0'), count++;
		else if (base==16) {
			tx_byte('0'), count++;
			tx_byte(digits[33]), count++;
		}
	}
	if (!(type & LEFT))
		while (size-- > 0)
			tx_byte(c), count++;
	while (i < precision--)
		tx_byte('0'), count++;
	while (i-- > 0)
		tx_byte(tmp[i]), count++;
	while (size-- > 0)
		tx_byte(' '), count++;
	return count;
}

int vtxprintf(void (*tx_byte)(unsigned char byte), const char *fmt, va_list args)
{
	int len;
	unsigned long long num;
	int i, base;
	const char *s;

	int flags;		/* flags to number() */

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */
	int qualifier;		/* 'h', 'l', or 'L' for integer fields */

	int count;

	for (count=0; *fmt ; ++fmt) {
		if (*fmt != '%') {
			tx_byte(*fmt), count++;
			continue;
		}

		/* process flags */
		flags = 0;
repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt) {
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
				}

		/* get field width */
		field_width = -1;
		if (is_digit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;
			if (is_digit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'z') {
			qualifier = *fmt;
			++fmt;
			if (*fmt == 'l') {
				qualifier = 'L';
				++fmt;
			}
		}

		/* default base */
		base = 10;

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					tx_byte(' '), count++;
			tx_byte((unsigned char) va_arg(args, int)), count++;
			while (--field_width > 0)
				tx_byte(' '), count++;
			continue;

		case 's':
			s = va_arg(args, char *);
			if (!s)
				s = "<NULL>";

			len = strnlen(s, precision);

			if (!(flags & LEFT))
				while (len < field_width--)
					tx_byte(' '), count++;
			for (i = 0; i < len; ++i)
				tx_byte(*s++), count++;
			while (len < field_width--)
				tx_byte(' '), count++;
			continue;

		case 'p':
			if (field_width == -1) {
				field_width = 2*sizeof(void *);
				flags |= ZEROPAD;
			}
			count += number(tx_byte,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			continue;

		case 'n':
			if (qualifier == 'L') {
				long long *ip = va_arg(args, long long *);
				*ip = count;
			} else if (qualifier == 'l') {
				long * ip = va_arg(args, long *);
				*ip = count;
			} else {
				int * ip = va_arg(args, int *);
				*ip = count;
			}
			continue;

		case '%':
			tx_byte('%'), count++;
			continue;

		/* integer number formats - set up the flags and "break" */
		case 'o':
			base = 8;
			break;

		case 'X':
			flags |= LARGE;
		case 'x':
			base = 16;
			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			break;

		default:
			tx_byte('%'), count++;
			if (*fmt)
				tx_byte(*fmt), count++;
			else
				--fmt;
			continue;
		}
		if (qualifier == 'L') {
			num = va_arg(args, unsigned long long);
		} else if (qualifier == 'l') {
			num = va_arg(args, unsigned long);
		} else if (qualifier == 'z') {
			num = va_arg(args, size_t);
		} else if (qualifier == 'h') {
			num = (unsigned short) va_arg(args, int);
			if (flags & SIGN)
				num = (short) num;
		} else if (flags & SIGN) {
			num = va_arg(args, int);
		} else {
			num = va_arg(args, unsigned int);
		}
		count += number(tx_byte, num, base, field_width, precision, flags);
	}
	return count;
}

int do_printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vtxprintf(exynos5_uart_tx_byte, fmt, args);
	va_end(args);

	return i;
}

void *stream_start(void *v)
{
//	struct exynos_spi *regs = (struct exynos_spi *)samsung_get_base_spi1();
	struct exynos_spi *regs = v;

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
	return (void *)regs;
}

int stream_read(void *stream, void **where, u32 len, u32 off)
{
	int upto, todo;
	int i;
	struct exynos_spi *regs = stream;
	/* Send read instruction (0x3h) followed by a 24 bit addr */
	writel((SF_READ_DATA_CMD << 24) | off, &regs->tx_data);

	/* waiting for TX done */
	while (!(readl(&regs->spi_sts) & SPI_ST_TX_DONE));

	for (upto = 0, i = 0; upto < len; upto += todo, i++) {
		todo = MIN(len - upto, (1 << 15));
		spi_rx_tx(regs, todo, (void *)(*where),
					(void *)(off), i);
	}

	setbits_le32(&regs->cs_reg, SPI_SLAVE_SIG_INACT);/* make the CS high */

	/*
	 * Let put controller mode to BYTE as
	 * SPI driver does not support WORD mode yet
	 */
	clrbits_le32(&regs->mode_cfg, SPI_MODE_CH_WIDTH_WORD |
					SPI_MODE_BUS_WIDTH_WORD);
	writel(0, &regs->swap_cfg);

	return len;
}

void stream_fini(void *stream)
{
	struct exynos_spi *regs = stream;

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
	/* FIXME: we should not need UART in bootblock, this is only
	   done for testing purposes */
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	power_init();
	clock_init();
	do_serial();
	printk(BIOS_INFO, "%s: UART initialized\n", __func__);
#if 0
	/* Copy romstage data from SPI ROM to SRAM */
	printk(BIOS_INFO, "Copying romstage:\n"
			"\tSPI offset: 0x%06x\n"
			"\tiRAM offset: 0x%08x\n"
			"\tSize: 0x%x\n",
			0, CONFIG_SPI_IMAGE_HACK, CONFIG_ROMSTAGE_SIZE);

	stream = stream_start((void *)0x12d30000);
	len = stream_read(stream, &where, CONFIG_ROMSTAGE_SIZE, 0);
	if (len < CONFIG_ROMSTAGE_SIZE)
		printk(BIOS_ERR, "Did not read all of the SPI! Wanted %d, got %d\n", CONFIG_ROMSTAGE_SIZE, len);
	stream_fini(stream);
#endif
#if 0
	/* FIXME: dump SRAM content for sanity checking */
	uint32_t u;
	for (u = CONFIG_SPI_IMAGE_HACK; u < CONFIG_SPI_IMAGE_HACK + 128; u++) {
		if (u % 16 == 0)
			printk(BIOS_INFO, "\n0x%08x: ", u);
		else
			printk(BIOS_INFO, " ");
		printk(BIOS_INFO, "%02x", *(uint8_t *)(u));
	}
	printk(BIOS_INFO, "\n");
#endif
	printk(BIOS_INFO, "%s: finished\n", __func__);
}
