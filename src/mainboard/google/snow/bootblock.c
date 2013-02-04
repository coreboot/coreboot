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

#include "cpu/samsung/exynos5-common/uart.h"
#include "cpu/samsung/exynos5250/clk.h"
#include "cpu/samsung/exynos5250/gpio.h"
#include "cpu/samsung/exynos5250/power.h"
#include "cpu/samsung/exynos5250/clock_init.h"
#include <console/console.h>

static uint32_t uart3_base = CONFIG_CONSOLE_SERIAL_UART_ADDRESS;

static void serial_setbrg_dev(void)
{
//	struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	struct s5p_uart *uart = (struct s5p_uart *)uart3_base;
	u32 uclk;
	u32 baudrate = CONFIG_TTYS0_BAUD;
	u32 val;
//	enum periph_id periph;

//	periph = exynos5_get_periph_id(base_port);
	uclk = clock_get_periph_rate(PERIPH_ID_UART3);
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

void __div0 (void);
void __div0 (void)
{
	die("divide by zero detected");
	while(1) ;
}

void bootblock_mainboard_init(void);
void bootblock_mainboard_init(void)
{
	/* FIXME: we should not need UART in bootblock, this is only
	   done for testing purposes */
	power_init();
	system_clock_init();
#if CONFIG_EARLY_SERIAL_CONSOLE
	do_serial();
	printk(BIOS_INFO, "\n\n\n%s: UART initialized\n", __func__);
#endif
}
