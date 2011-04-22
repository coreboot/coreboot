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

/* Should support 8250, 16450, 16550, 16550A type UARTs */

static inline int uart8250_can_tx_byte(unsigned base_port)
{
	return inb(base_port + UART_LSR) & UART_MSR_DSR;
}

static inline void uart8250_wait_to_tx_byte(unsigned base_port)
{
	while(!uart8250_can_tx_byte(base_port))
		;
}

static inline void uart8250_wait_until_sent(unsigned base_port)
{
	while(!(inb(base_port + UART_LSR) & UART_LSR_TEMT))
		;
}

void uart8250_tx_byte(unsigned base_port, unsigned char data)
{
	uart8250_wait_to_tx_byte(base_port);
	outb(data, base_port + UART_TBR);
	/* Make certain the data clears the fifos */
	uart8250_wait_until_sent(base_port);
}

int uart8250_can_rx_byte(unsigned base_port)
{
	return inb(base_port + UART_LSR) & UART_LSR_DR;
}

unsigned char uart8250_rx_byte(unsigned base_port)
{
	while(!uart8250_can_rx_byte(base_port))
		;
	return inb(base_port + UART_RBR);
}

void uart8250_init(unsigned base_port, unsigned divisor)
{
	/* Disable interrupts */
	outb(0x0, base_port + UART_IER);
	/* Enable FIFOs */
	outb(UART_FCR_FIFO_EN, base_port + UART_FCR);

	/* assert DTR and RTS so the other end is happy */
	outb(UART_MCR_DTR | UART_MCR_RTS, base_port + UART_MCR);

	/* DLAB on */
	outb(UART_LCR_DLAB | CONFIG_TTYS0_LCS, base_port + UART_LCR);

	/* Set Baud Rate Divisor. 12 ==> 115200 Baud */
	outb(divisor & 0xFF,   base_port + UART_DLL);
	outb((divisor >> 8) & 0xFF,    base_port + UART_DLM);

	/* Set to 3 for 8N1 */
	outb(CONFIG_TTYS0_LCS, base_port + UART_LCR);
}

#if defined(__PRE_RAM__) || defined(__SMM__)
void uart_init(void)
{
	/* TODO the divisor calculation is hard coded to standard UARTs. Some
	 * UARTs won't work with these values. This should be a property of the
	 * UART used, worst case a Kconfig variable. For now live with hard
	 * codes as the only devices that might be different are the iWave
	 * iRainbowG6 and the OXPCIe952 card (and the latter is memory mapped)
	 */
#if CONFIG_USE_OPTION_TABLE && !defined(__SMM__)
        static const unsigned char divisor[] = { 1, 2, 3, 6, 12, 24, 48, 96 };
        unsigned ttys0_div, ttys0_index;
        ttys0_index = read_option(CMOS_VSTART_baud_rate, CMOS_VLEN_baud_rate, 0);
        ttys0_index &= 7;
        ttys0_div = divisor[ttys0_index];

	uart8250_init(CONFIG_TTYS0_BASE, ttys0_div);
#else
	uart8250_init(CONFIG_TTYS0_BASE, (115200 / CONFIG_TTYS0_BAUD));
#endif
}
#endif
