/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018       Facebook, Inc.
 * Copyright 2003-2017  Cavium Inc.  <support@cavium.com>
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
 * Derived from Cavium's BSD-3 Clause OCTEONTX-SDK-6.2.0.
 */

#include <arch/io.h>
#include <console/uart.h>
#include <delay.h>
#include <endian.h>
#include <stdint.h>
#include <soc/clock.h>
#include <soc/uart.h>
#include <assert.h>
#include <soc/addressmap.h>
#include <drivers/uart/pl011.h>

union cn81xx_uart_ctl {
	u64 u;
	struct {
		u64 uctl_rst		: 1;
		u64 uaa_rst		: 1;
		u64			: 2;
		u64 csclk_en		: 1;
		u64 			: 19;
		u64 h_clkdiv_sel	: 3;
		u64 			: 1;
		u64 h_clkdiv_rst	: 1;
		u64 h_clk_byp_sel	: 1;
		u64 h_clk_en		: 1;
		u64 			: 33;
	} s;
};

struct cn81xx_uart {
	struct pl011_uart pl011;
	union cn81xx_uart_ctl uctl_ctl;
	u8 rsvd4[0x8];
	u64 uctl_spare0;
	u8 rsvd5[0xe0];
	u64 uctl_spare1;
};

#define UART_IBRD_BAUD_DIVINT_SHIFT		0
#define UART_IBRD_BAUD_DIVINT_MASK		0xffff

#define UART_FBRD_BAUD_DIVFRAC_SHIFT		0
#define UART_FBRD_BAUD_DIVFRAC_MASK		0x3f


check_member(cn81xx_uart, uctl_ctl, 0x1000);
check_member(cn81xx_uart, uctl_spare1, 0x10f8);

#define UART_SCLK_DIV 3

/**
 * Returns the current UART HCLK divider
 *
 * @param reg      The H_CLKDIV_SEL value
 * @return         The HCLK divider
 */
static size_t uart_sclk_divisor(const size_t reg)
{
	static const u8 div[] = {1, 2, 4, 6, 8, 16, 24, 32};

	assert(reg < ARRAY_SIZE(div));

	return div[reg];
}

/**
 * Returns the current UART HCLK
 *
 * @param uart     The UART to operate on
 * @return         The HCLK in Hz
 */
static size_t uart_hclk(struct cn81xx_uart *uart)
{
	union cn81xx_uart_ctl ctl;
	const uint64_t sclk = thunderx_get_io_clock();

	ctl.u = read64(&uart->uctl_ctl);
	return sclk / uart_sclk_divisor(ctl.s.h_clkdiv_sel);
}

unsigned int uart_platform_refclk(void)
{
	struct cn81xx_uart *uart =
	    (struct cn81xx_uart *)CONFIG_CONSOLE_SERIAL_UART_ADDRESS;

	if (!uart)
		return 0;

	return uart_hclk(uart);
}

uintptr_t uart_platform_base(int idx)
{
	return CONFIG_CONSOLE_SERIAL_UART_ADDRESS;
}

/**
 * Waits given count if HCLK cycles
 *
 * @param uart     The UART to operate on
 * @param hclks    The number of HCLK cycles to wait
 */
static void uart_wait_hclk(struct cn81xx_uart *uart, const size_t hclks)
{
	const size_t hclk = uart_hclk(uart);
	const size_t delay = (hclks * 1000000ULL) / hclk;
	udelay(MAX(delay, 1));
}

/**
 * Returns the UART state.
 *
 * @param bus     The UART to operate on
 * @return        Boolean: True if UART is enabled
 */
int uart_is_enabled(const size_t bus)
{
	struct cn81xx_uart *uart = (struct cn81xx_uart *)UAAx_PF_BAR0(bus);
	union cn81xx_uart_ctl ctl;

	assert(uart);
	if (!uart)
		return 0;

	ctl.u = read64(&uart->uctl_ctl);
	return !!ctl.s.csclk_en;
}

/**
 * Setup UART with desired BAUD rate in 8N1, no parity mode.
 *
 * @param bus          The UART to operate on
 * @param baudrate     baudrate to set up
 *
 * @return             Boolean: True on error
 */
int uart_setup(const size_t bus, int baudrate)
{
	union cn81xx_uart_ctl ctl;
	struct cn81xx_uart *uart = (struct cn81xx_uart *)UAAx_PF_BAR0(bus);

	assert(uart);
	if (!uart)
		return 1;

	/* 1.2.1 Initialization Sequence (Power-On/Hard/Cold Reset) */
	/* 1. Wait for IOI reset (srst_n) to deassert. */

	/**
	 * 2. Assert all resets:
	 * a. UAA reset: UCTL_CTL[UAA_RST] = 1
	 * b. UCTL reset: UCTL_CTL[UCTL_RST] = 1
	 */
	ctl.u = read64(&uart->uctl_ctl);
	ctl.s.uctl_rst = 1;
	ctl.s.uaa_rst = 1;
	write64(&uart->uctl_ctl, ctl.u);

	/**
	 * 3. Configure the HCLK:
	 * a. Reset the clock dividers: UCTL_CTL[H_CLKDIV_RST] = 1.
	 * b. Select the HCLK frequency
	 * i. UCTL_CTL[H_CLKDIV] = desired value,
	 * ii. UCTL_CTL[H_CLKDIV_EN] = 1 to enable the HCLK.
	 * iii. Readback UCTL_CTL to ensure the values take effect.
	 * c. Deassert the HCLK clock divider reset: UCTL_CTL[H_CLKDIV_RST] = 0.
	 */
	ctl.u = read64(&uart->uctl_ctl);
	ctl.s.h_clkdiv_sel = UART_SCLK_DIV;
	write64(&uart->uctl_ctl, ctl.u);

	ctl.u = read64(&uart->uctl_ctl);
	ctl.s.h_clk_byp_sel = 0;
	write64(&uart->uctl_ctl, ctl.u);

	ctl.u = read64(&uart->uctl_ctl);
	ctl.s.h_clk_en = 1;
	write64(&uart->uctl_ctl, ctl.u);

	ctl.u = read64(&uart->uctl_ctl);
	ctl.s.h_clkdiv_rst = 0;
	write64(&uart->uctl_ctl, ctl.u);

	/**
	 * 4. Wait 20 HCLK cycles from step 3 for HCLK to start and async fifo
	 * to properly reset.
	 */
	uart_wait_hclk(uart, 20 + 1);

	/**
	 * 5. Deassert UCTL and UAHC resets:
	 *  a. UCTL_CTL[UCTL_RST] = 0
	 * b. Wait 10 HCLK cycles.
	 * c. UCTL_CTL[UAHC_RST] = 0
	 * d. You will have to wait 10 HCLK cycles before accessing any
	 * HCLK-only registers.
	 */
	ctl.u = read64(&uart->uctl_ctl);
	ctl.s.uctl_rst = 0;
	write64(&uart->uctl_ctl, ctl.u);

	uart_wait_hclk(uart, 10 + 1);

	ctl.u = read64(&uart->uctl_ctl);
	ctl.s.uaa_rst = 0;
	write64(&uart->uctl_ctl, ctl.u);

	uart_wait_hclk(uart, 10 + 1);

	/**
	 * 6. Enable conditional SCLK of UCTL by writing
	 * UCTL_CTL[CSCLK_EN] = 1.
	 */
	ctl.u = read64(&uart->uctl_ctl);
	ctl.s.csclk_en = 1;
	write64(&uart->uctl_ctl, ctl.u);

	/**
	 * 7. Initialize the integer and fractional baud rate divider registers
	 * UARTIBRD and UARTFBRD as follows:
	 * a. Baud Rate Divisor = UARTCLK/(16xBaud Rate) = BRDI + BRDF
	 * b. The fractional register BRDF, m is calculated as
	 * integer(BRDF x 64 + 0.5)
	 * Example calculation:
	 * If the required baud rate is 230400 and hclk = 4MHz then:
	 * Baud Rate Divisor = (4x10^6)/(16x230400) = 1.085
	 * This means BRDI = 1 and BRDF = 0.085.
	 * Therefore, fractional part, BRDF = integer((0.085x64)+0.5) = 5
	 * Generated baud rate divider = 1+5/64 = 1.078
	 */
	u64 divisor = thunderx_get_io_clock() /
		(baudrate * 16 * uart_sclk_divisor(UART_SCLK_DIV) / 64);
	write32(&uart->pl011.ibrd, divisor >> 6);
	write32(&uart->pl011.fbrd, divisor & UART_FBRD_BAUD_DIVFRAC_MASK);

	/**
	 * 8. Program the line control register UAA(0..1)_LCR_H and the control
	 * register UAA(0..1)_CR
	 */
	/* 8-bits, FIFO enable */
	write32(&uart->pl011.lcr_h, PL011_UARTLCR_H_WLEN_8 |
				    PL011_UARTLCR_H_FEN);
	/* RX/TX enable, UART enable */
	write32(&uart->pl011.cr, PL011_UARTCR_RXE | PL011_UARTCR_TXE |
				 PL011_UARTCR_UARTEN);

	return 0;
}
