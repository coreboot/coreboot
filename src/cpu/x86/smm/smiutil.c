/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <arch/io.h>
#include <arch/romcc_io.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>

/* ********************* smi_util ************************* */

/* Data */
#define UART_RBR 0x00
#define UART_TBR 0x00

/* Control */
#define UART_IER 0x01
#define UART_IIR 0x02
#define UART_FCR 0x02
#define UART_LCR 0x03
#define UART_MCR 0x04
#define UART_DLL 0x00
#define UART_DLM 0x01

/* Status */
#define UART_LSR 0x05
#define UART_MSR 0x06
#define UART_SCR 0x07

#ifndef CONFIG_TTYS0_BASE
#define CONFIG_TTYS0_BASE 0x3f8
#endif

#ifndef CONFIG_TTYS0_BAUD
#define CONFIG_TTYS0_BAUD 115200
#endif

#ifndef CONFIG_TTYS0_DIV
#define CONFIG_TTYS0_DIV	(115200/CONFIG_TTYS0_BAUD)
#endif

/* Line Control Settings */
#ifndef CONFIG_TTYS0_LCS
/* Set 8bit, 1 stop bit, no parity */
#define CONFIG_TTYS0_LCS	0x3
#endif

#define UART_LCS	CONFIG_TTYS0_LCS

static int uart_can_tx_byte(void)
{
	return inb(CONFIG_TTYS0_BASE + UART_LSR) & 0x20;
}

static void uart_wait_to_tx_byte(void)
{
	while(!uart_can_tx_byte())
	;
}

static void uart_wait_until_sent(void)
{
	while(!(inb(CONFIG_TTYS0_BASE + UART_LSR) & 0x40))
	;
}

static void uart_tx_byte(unsigned char data)
{
	uart_wait_to_tx_byte();
	outb(data, CONFIG_TTYS0_BASE + UART_TBR);
	/* Make certain the data clears the fifos */
	uart_wait_until_sent();
}

void console_tx_flush(void)
{
	uart_wait_to_tx_byte();
}

void console_tx_byte(unsigned char byte)
{
	if (byte == '\n')
		uart_tx_byte('\r');
	uart_tx_byte(byte);
}

#if CONFIG_DEBUG_SMI
static void uart_init(void)
{
	/* disable interrupts */
	outb(0x0, CONFIG_TTYS0_BASE + UART_IER);
	/* enable fifo's */
	outb(0x01, CONFIG_TTYS0_BASE + UART_FCR);
	/* Set Baud Rate Divisor to 12 ==> 115200 Baud */
	outb(0x80 | UART_LCS, CONFIG_TTYS0_BASE + UART_LCR);
	outb(CONFIG_TTYS0_DIV & 0xFF,   CONFIG_TTYS0_BASE + UART_DLL);
	outb((CONFIG_TTYS0_DIV >> 8) & 0xFF,    CONFIG_TTYS0_BASE + UART_DLM);
	outb(UART_LCS, CONFIG_TTYS0_BASE + UART_LCR);
}
#endif

void console_init(void)
{
#if CONFIG_DEBUG_SMI
	console_loglevel = CONFIG_DEFAULT_CONSOLE_LOGLEVEL;
	uart_init();
#else
	console_loglevel = 1;
#endif
}

/* ********************* smi_util ************************* */
