/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 Rob Taylor, Flying Pig Systems. robt@flyingpig.com.
 * Copyright (C) Broadcom Corporation
 * Copyright (C) 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <arch/io.h>
#include <boot/coreboot_tables.h>
#include <console/console.h>	/* for __console definition */
#include <console/uart.h>
#include <delay.h>
#include <soc/ns16550.h>

#define SYS_NS16550_CLK		100000000
#define SYS_NS16550_BAUDRATE	115200
#define MODE_X_DIV		16
#define SINGLE_CHAR_TIMEOUT	(50 * 1000)

static struct ns16550 * const regs =
	(void *)CONFIG_CONSOLE_SERIAL_UART_ADDRESS;

static int calc_divisor(void)
{
	/* Compute divisor value. Normally, we should simply return:
	 *   ns16550_clk / MODE_X_DIV / baudrate
	 * but we need to round that value by adding 0.5.
	 * Rounding is especially important at high baud rates.
	 */
	int div = MODE_X_DIV * SYS_NS16550_BAUDRATE;
	return (SYS_NS16550_CLK + div / 2) / div;
}

static void ns16550_init(void)
{
	int baud_divisor = calc_divisor();

	while (!(readl(&regs->lsr) & UART_LSR_TEMT))
		;

	writel(0, &regs->ier);
	writel(UART_LCR_BKSE | UART_LCR_8N1, &regs->lcr);
	writel(0, &regs->dll);
	writel(0, &regs->dlm);
	writel(UART_LCR_8N1, &regs->lcr);
	writel(UART_MCR_DTR | UART_MCR_RTS, &regs->mcr);
	/* clear & enable FIFOs */
	writel(UART_FCR_FIFO_EN | UART_FCR_RXSR | UART_FCR_TXSR, &regs->fcr);
	writel(UART_LCR_BKSE | UART_LCR_8N1, &regs->lcr);
	writel(baud_divisor & 0xff, &regs->dll);
	writel((baud_divisor >> 8) & 0xff, &regs->dlm);
	writel(UART_LCR_8N1, &regs->lcr);
}

static void ns16550_tx_byte(unsigned char data)
{
	while ((readl(&regs->lsr) & UART_LSR_THRE) == 0)
		;
	writel(data, &regs->thr);
}

static void ns16550_tx_flush(void)
{
	while (!(readl(&regs->lsr) & UART_LSR_TEMT))
		;
}

static int ns16550_tst_byte(void)
{
	return (readl(&regs->lsr) & UART_LSR_DR) != 0;
}

static unsigned char ns16550_rx_byte(void)
{
	unsigned long int i = SINGLE_CHAR_TIMEOUT;
	while (i-- && !ns16550_tst_byte())
		udelay(1);
	if (i)
		return readl(&regs->rbr);
	else
		return 0x0;
}

void uart_init(int idx)
{
	ns16550_init();
}

void uart_tx_byte(int idx, unsigned char data)
{
	ns16550_tx_byte(data);
}

void uart_tx_flush(int idx)
{
	ns16550_tx_flush();
}

unsigned char uart_rx_byte(int idx)
{
	return ns16550_rx_byte();
}

#ifndef __PRE_RAM__
void uart_fill_lb(void *data)
{
	struct lb_serial serial;
	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = (uintptr_t)regs;
	serial.baud = default_baudrate();
	serial.regwidth = 1;
	lb_add_serial(&serial, data);

	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, data);
}
#endif
