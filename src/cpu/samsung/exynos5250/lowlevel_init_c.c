/*
 * Lowlevel setup for SMDK5250 board based on S5PC520
 *
 * Copyright (C) 2012 Samsung Electronics
 * Copyright (c) 2012 The Chromium OS Authors.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <config.h>
#include <cpu/samsung/exynos5-common/exynos5-common.h>
#include <cpu/samsung/exynos5-common/spl.h>
#include <cpu/samsung/exynos5250/clock_init.h>
#include <cpu/samsung/exynos5250/cpu.h>
#include <cpu/samsung/exynos5250/dmc.h>
#include <cpu/samsung/exynos5250/pinmux.h>
#include <cpu/samsung/exynos5250/power.h>
#include <cpu/samsung/exynos5250/setup.h>
#include <cpu/samsung/exynos5250/tzpc.h>
#include "setup.h"

#include <uart.h>
#include <console/console.h>

void do_barriers(void);	/* FIXME: make gcc shut up about "no previous prototype" */

/* FIXME: copy + pasted a bunch of exynos5 uart code here since we found
   that we weren't really executing uart_init and uart_tx_byte as expected... */

#include <cpu/samsung/exynos5-common/exynos5-common.h>
#include <cpu/samsung/exynos5250/clk.h>
#include <cpu/samsung/exynos5250/uart.h>

#define RX_FIFO_COUNT_MASK	0xff
#define RX_FIFO_FULL_MASK	(1 << 8)
#define TX_FIFO_FULL_MASK	(1 << 24)

static uint32_t base_port = 0x12c30000;

static const int udivslot[] = {
	0,
	0x0080,
	0x0808,
	0x0888,
	0x2222,
	0x4924,
	0x4a52,
	0x54aa,
	0x5555,
	0xd555,
	0xd5d5,
	0xddd5,
	0xdddd,
	0xdfdd,
	0xdfdf,
	0xffdf,
};

static void serial_setbrg_dev(void)
{
//	struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	volatile struct s5p_uart *uart = (struct s5p_uart *)base_port;
	u32 uclk;
	u32 baudrate = CONFIG_TTYS0_BAUD;
	u32 val;
	enum periph_id periph;

	periph = exynos5_get_periph_id(base_port);
	uclk = clock_get_periph_rate(periph);
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


static int exynos5_uart_err_check(int op)
{
	//struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	volatile struct s5p_uart *uart = (struct s5p_uart *)base_port;
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

/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 */
static void exynos5_init_dev(void)
{
//	struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	volatile struct s5p_uart *uart = (struct s5p_uart *)base_port;

	/* enable FIFOs */
	writel(0x1, &uart->ufcon);
	writel(0, &uart->umcon);
	/* 8N1 */
	writel(0x3, &uart->ulcon);
	/* No interrupts, no DMA, pure polling */
	writel(0x245, &uart->ucon);

	serial_setbrg_dev();
}

static void exynos5_uart_tx_byte(unsigned char data)
{
//	struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	struct s5p_uart *uart = (struct s5p_uart *)base_port;


	/* wait for room in the tx FIFO */
	while ((readl(uart->ufstat) & TX_FIFO_FULL_MASK)) {
		if (exynos5_uart_err_check(1))
			return;
	}

	writeb(data, &uart->utxh);
}



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

/* These are the things we can do during low-level init */
enum {
	DO_WAKEUP	= 1 << 0,
	DO_UART		= 1 << 1,
	DO_CLOCKS	= 1 << 2,
	DO_POWER	= 1 << 3,
};

extern int test(int x);
#if 0
int test(int x)
{
	if (x < 10)
		x = test(x + 1);
	return x;
}
#endif

int lowlevel_init_subsystems(void)
{
//	uint32_t reset_status;
//	int actions = 0;
	volatile unsigned long *addr = (unsigned long *)0x1004330c;

	do_barriers();

	/* Setup cpu info which is needed to select correct register offsets */
	cpu_info_init();

#if 0
	reset_status = power_read_reset_status();

	switch (reset_status) {
	case S5P_CHECK_SLEEP:
		actions = DO_CLOCKS | DO_WAKEUP;
		break;
	case S5P_CHECK_DIDLE:
	case S5P_CHECK_LPA:
		actions = DO_WAKEUP;
	default:
		/* This is a normal boot (not a wake from sleep) */
		actions = DO_UART | DO_CLOCKS | DO_POWER;
	}
#endif

//	actions = DO_UART | DO_CLOCKS | DO_POWER;
#if 0
	if (actions & DO_POWER)
		power_init();
	if (actions & DO_CLOCKS)
		system_clock_init();
	if (actions & DO_UART) {
#endif
//	power_init();

#if 0
	/* cheap memtest */
	volatile unsigned long *x = (unsigned long *)0x02060000;
	while (x < (unsigned long *)(0x0206efff)) {
		*x = 0xdeadbeef;
		if (*x != 0xdeadbeef)
			while (1);
		x++;
	}
#endif

#if 0
	if (test(0) == 10)
		*addr |= 0x100;
	while (1);
#endif

	power_init();
	system_clock_init();

		/* Set up serial UART so we can printf() */
//		exynos_pinmux_config(EXYNOS_UART, PINMUX_FLAG_NONE);
		/* FIXME(dhendrix): add a function for mapping
		   CONFIG_CONSOLE_SERIAL_UART_ADDRESS to PERIPH_ID_UARTn */
		exynos_pinmux_config(PERIPH_ID_UART3, PINMUX_FLAG_NONE);
		/* FIXME(dhendrix): serial_init() does not seem to
		   actually do anything !?!? */
//		serial_init();

	exynos5_init_dev();
	*addr |= 0x100;
	while (1)
		exynos5_uart_tx_byte('C');

	init_timer();	/* FIXME(dhendrix): was timer_init() */

#if 0
	if (actions & DO_CLOCKS) {
		mem_ctrl_init();
		tzpc_init();
	}
#endif

//	return actions & DO_WAKEUP;
	return 0;
}
