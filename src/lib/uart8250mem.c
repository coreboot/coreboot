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
#include <uart8250.h>
#include <pc80/mc146818rtc.h>
#if CONFIG_USE_OPTION_TABLE
#include "option_table.h"
#endif
#include <device/device.h>
#include <delay.h>

/* Should support 8250, 16450, 16550, 16550A type UARTs */

/* Expected character delay at 1200bps is 9ms for a working UART
 * and no flow-control. Assume UART as stuck if shift register
 * or FIFO takes more than 50ms per character to appear empty.
 */
#define SINGLE_CHAR_TIMEOUT	(50 * 1000)
#define FIFO_TIMEOUT		(16 * SINGLE_CHAR_TIMEOUT)

static inline int uart8250_mem_can_tx_byte(unsigned base_port)
{
	return read8(base_port + UART_LSR) & UART_LSR_THRE;
}

static inline void uart8250_mem_wait_to_tx_byte(unsigned base_port)
{
	unsigned long int i = SINGLE_CHAR_TIMEOUT;
	while(i-- && !uart8250_mem_can_tx_byte(base_port))
		udelay(1);
}

static inline void uart8250_mem_wait_until_sent(unsigned base_port)
{
	unsigned long int i = FIFO_TIMEOUT;
	while(i-- && !(read8(base_port + UART_LSR) & UART_LSR_TEMT))
		udelay(1);
}

void uart8250_mem_tx_byte(unsigned base_port, unsigned char data)
{
	uart8250_mem_wait_to_tx_byte(base_port);
	write8(base_port + UART_TBR, data);
}

void uart8250_mem_tx_flush(unsigned base_port)
{
	uart8250_mem_wait_until_sent(base_port);
}

int uart8250_mem_can_rx_byte(unsigned base_port)
{
	return read8(base_port + UART_LSR) & UART_LSR_DR;
}

unsigned char uart8250_mem_rx_byte(unsigned base_port)
{
	unsigned long int i = SINGLE_CHAR_TIMEOUT;
	while(i-- && !uart8250_mem_can_rx_byte(base_port))
		udelay(1);
	if (i)
		return read8(base_port + UART_RBR);
	else
		return 0x0;
}

void uart8250_mem_init(unsigned base_port, unsigned divisor)
{
	/* Disable interrupts */
	write8(base_port + UART_IER, 0x0);
	/* Enable FIFOs */
	write8(base_port + UART_FCR, UART_FCR_FIFO_EN);

	/* Assert DTR and RTS so the other end is happy */
	write8(base_port + UART_MCR, UART_MCR_DTR | UART_MCR_RTS);

	/* DLAB on */
	write8(base_port + UART_LCR, UART_LCR_DLAB | CONFIG_TTYS0_LCS);

	/* Set Baud Rate Divisor. 12 ==> 9600 Baud */
	write8(base_port + UART_DLL, divisor & 0xFF);
	write8(base_port + UART_DLM, (divisor >> 8) & 0xFF);

	/* Set to 3 for 8N1 */
	write8(base_port + UART_LCR, CONFIG_TTYS0_LCS);
}

u32 uart_mem_init(void)
{
	unsigned uart_baud = CONFIG_TTYS0_BAUD;
	u32 uart_bar = 0;
	unsigned div;

	/* find out the correct baud rate */
#if !defined(__SMM__) && CONFIG_USE_OPTION_TABLE
	static const unsigned baud[8] = { 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200 };
	unsigned b_index = 0;
#if defined(__PRE_RAM__)
	b_index = read_option(baud_rate, 0);
	b_index &= 7;
	uart_baud = baud[b_index];
#else
	if (get_option(&b_index, "baud_rate") == 0) {
		uart_baud = baud[b_index];
	}
#endif
#endif

	/* Now find the UART base address and calculate the divisor */
#if CONFIG_DRIVERS_OXFORD_OXPCIE

#if defined(MORE_TESTING) && !defined(__SIMPLE_DEVICE__)
	device_t dev = dev_find_device(0x1415, 0xc158, NULL);
	if (!dev)
		dev = dev_find_device(0x1415, 0xc11b, NULL);

	if (dev) {
		struct resource *res = find_resource(dev, 0x10);

		if (res) {
			uart_bar = res->base + 0x1000; // for 1st UART
			// uart_bar = res->base + 0x2000; // for 2nd UART
		}
	}

	if (!uart_bar)
#endif
	uart_bar = CONFIG_OXFORD_OXPCIE_BASE_ADDRESS + 0x1000; // 1st UART
	// uart_bar = CONFIG_OXFORD_OXPCIE_BASE_ADDRESS + 0x2000; // 2nd UART

	div = 4000000 / uart_baud;
#endif

	if (uart_bar)
		uart8250_mem_init(uart_bar, div);

	return uart_bar;
}
