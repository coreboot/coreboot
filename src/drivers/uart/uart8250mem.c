/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
 * Copyright (C) 2006-2010 coresystems GmbH
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

#include <arch/io.h>
#include <console/uart.h>
#include <device/device.h>
#include <delay.h>
#include "uart8250reg.h"

/* Should support 8250, 16450, 16550, 16550A type UARTs */

/* Expected character delay at 1200bps is 9ms for a working UART
 * and no flow-control. Assume UART as stuck if shift register
 * or FIFO takes more than 50ms per character to appear empty.
 */
#define SINGLE_CHAR_TIMEOUT	(50 * 1000)
#define FIFO_TIMEOUT		(16 * SINGLE_CHAR_TIMEOUT)

static int uart8250_mem_can_tx_byte(unsigned base_port)
{
	return read8(base_port + UART8250_LSR) & UART8250_LSR_THRE;
}

static void uart8250_mem_tx_byte(unsigned base_port, unsigned char data)
{
	unsigned long int i = SINGLE_CHAR_TIMEOUT;
	while(i-- && !uart8250_mem_can_tx_byte(base_port))
		udelay(1);
	write8(base_port + UART8250_TBR, data);
}

static void uart8250_mem_tx_flush(unsigned base_port)
{
	unsigned long int i = FIFO_TIMEOUT;
	while(i-- && !(read8(base_port + UART8250_LSR) & UART8250_LSR_TEMT))
		udelay(1);
}

static int uart8250_mem_can_rx_byte(unsigned base_port)
{
	return read8(base_port + UART8250_LSR) & UART8250_LSR_DR;
}

static unsigned char uart8250_mem_rx_byte(unsigned base_port)
{
	unsigned long int i = SINGLE_CHAR_TIMEOUT;
	while(i-- && !uart8250_mem_can_rx_byte(base_port))
		udelay(1);
	if (i)
		return read8(base_port + UART8250_RBR);
	else
		return 0x0;
}

static void uart8250_mem_init(unsigned base_port, unsigned divisor)
{
	/* Disable interrupts */
	write8(base_port + UART8250_IER, 0x0);
	/* Enable FIFOs */
	write8(base_port + UART8250_FCR, UART8250_FCR_FIFO_EN);

	/* Assert DTR and RTS so the other end is happy */
	write8(base_port + UART8250_MCR, UART8250_MCR_DTR | UART8250_MCR_RTS);

	/* DLAB on */
	write8(base_port + UART8250_LCR, UART8250_LCR_DLAB | CONFIG_TTYS0_LCS);

	write8(base_port + UART8250_DLL, divisor & 0xFF);
	write8(base_port + UART8250_DLM, (divisor >> 8) & 0xFF);

	/* Set to 3 for 8N1 */
	write8(base_port + UART8250_LCR, CONFIG_TTYS0_LCS);
}

void uart_init(int idx)
{
	u32 base = uart_platform_base(idx);
	if (!base)
		return;

	unsigned int div;
	div = uart_baudrate_divisor(default_baudrate(), uart_platform_refclk(), 16);
	uart8250_mem_init(base, div);
}

void uart_tx_byte(int idx, unsigned char data)
{
	u32 base = uart_platform_base(idx);
	if (!base)
		return;
	uart8250_mem_tx_byte(base, data);
}

unsigned char uart_rx_byte(int idx)
{
	u32 base = uart_platform_base(idx);
	if (!base)
		return 0xff;
	return uart8250_mem_rx_byte(base);
}

void uart_tx_flush(int idx)
{
	u32 base = uart_platform_base(idx);
	if (!base)
		return;
	uart8250_mem_tx_flush(base);
}
