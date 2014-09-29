/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
 * Copyright (C) 2006-2010 coresystems GmbH
 * Copyright (C) 2014 Imagination Technologies
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
#include <console/console.h>
#include <device/device.h>
#include <delay.h>
#include <uart.h>
#include <uart8250.h>

/* Should support 8250, 16450, 16550, 16550A type UARTs */

/* Expected character delay at 1200bps is 9ms for a working UART
 * and no flow-control. Assume UART as stuck if shift register
 * or FIFO takes more than 50ms per character to appear empty.
 */
#define SINGLE_CHAR_TIMEOUT	(50 * 1000)
#define FIFO_TIMEOUT		(16 * SINGLE_CHAR_TIMEOUT)
#define UART_SHIFT		2

#define GEN_ACCESSOR(name, idx)						\
static inline uint8_t read_##name(unsigned base_port)			\
{									\
	return read8(base_port + (idx << UART_SHIFT));			\
}									\
									\
static inline void write_##name(unsigned base_port, uint8_t val)	\
{									\
	write8(base_port + (idx << UART_SHIFT), val);			\
}

GEN_ACCESSOR(rbr, UART8250_RBR)
GEN_ACCESSOR(tbr, UART8250_TBR)
GEN_ACCESSOR(ier, UART8250_IER)
GEN_ACCESSOR(fcr, UART8250_FCR)
GEN_ACCESSOR(lcr, UART8250_LCR)
GEN_ACCESSOR(mcr, UART8250_MCR)
GEN_ACCESSOR(lsr, UART8250_LSR)
GEN_ACCESSOR(dll, UART8250_DLL)
GEN_ACCESSOR(dlm, UART8250_DLM)

static int uart8250_mem_can_tx_byte(unsigned base_port)
{
	return read_lsr(base_port) & UART8250_LSR_THRE;
}

static void uart8250_mem_tx_byte(unsigned base_port, unsigned char data)
{
	unsigned long int i = SINGLE_CHAR_TIMEOUT;
	while (i-- && !uart8250_mem_can_tx_byte(base_port))
		udelay(1);
	write_tbr(base_port, data);
}

static void uart8250_mem_tx_flush(unsigned base_port)
{
	unsigned long int i = FIFO_TIMEOUT;
	while (i-- && !(read_lsr(base_port) & UART8250_LSR_TEMT))
		udelay(1);
}

static int uart8250_mem_can_rx_byte(unsigned base_port)
{
	return read_lsr(base_port) & UART8250_LSR_DR;
}

static unsigned char uart8250_mem_rx_byte(unsigned base_port)
{
	unsigned long int i = SINGLE_CHAR_TIMEOUT;
	while (i-- && !uart8250_mem_can_rx_byte(base_port))
		udelay(1);
	if (i)
		return read_rbr(base_port);
	else
		return 0x0;
}

static void uart8250_mem_init(unsigned base_port, unsigned divisor)
{
	/* Disable interrupts */
	write_ier(base_port, 0x0);
	/* Enable FIFOs */
	write_fcr(base_port, UART8250_FCR_FIFO_EN);

	/* Assert DTR and RTS so the other end is happy */
	write_mcr(base_port, UART8250_MCR_DTR | UART8250_MCR_RTS);

	/* DLAB on */
	write_lcr(base_port, UART8250_LCR_DLAB | CONFIG_TTYS0_LCS);

	write_dll(base_port, divisor & 0xFF);
	write_dlm(base_port, (divisor >> 8) & 0xFF);

	/* Set to 3 for 8N1 */
	write_lcr(base_port, CONFIG_TTYS0_LCS);
}

static unsigned int uart_platform_refclk(void)
{
	/* TODO: this is entirely arbitrary */
	return 1000000;
}

static unsigned int uart_platform_base(int idx)
{
	switch (idx) {
	case 0:
		return 0xb8101400;

	case 1:
		return 0xb8101500;

	default:
		return 0x0;
	}
}

/* Calculate divisor. Do not floor but round to nearest integer. */
static unsigned int uart_baudrate_divisor(unsigned int baudrate,
	unsigned int refclk, unsigned int oversample)
{
	return (1 + (2 * refclk) / (baudrate * oversample)) / 2;
}

static void pistachio_uart_init(void)
{
	u32 base = uart_platform_base(0);
	if (!base)
		return;

	unsigned int div;
	div = uart_baudrate_divisor(CONFIG_TTYS0_BAUD,
				    uart_platform_refclk(), 16);
	uart8250_mem_init(base, div);
}

static void pistachio_uart_tx_byte(unsigned char data)
{
	u32 base = uart_platform_base(0);
	if (!base)
		return;
	uart8250_mem_tx_byte(base, data);
}

static unsigned char pistachio_uart_rx_byte(void)
{
	u32 base = uart_platform_base(0);
	if (!base)
		return 0xff;
	return uart8250_mem_rx_byte(base);
}

static void pistachio_uart_tx_flush(void)
{
	u32 base = uart_platform_base(0);
	if (!base)
		return;
	uart8250_mem_tx_flush(base);
}

#if !defined(__PRE_RAM__)

static const struct console_driver pistachio_uart_console __console = {
	.init     = pistachio_uart_init,
	.tx_byte  = pistachio_uart_tx_byte,
	.tx_flush = pistachio_uart_tx_flush,
	.rx_byte  = pistachio_uart_rx_byte,
};

uint32_t uartmem_getbaseaddr(void)
{
	return uart_platform_base(0);
}

#else /* __PRE_RAM__ */

void uart_init(void)
{
	pistachio_uart_init();
}

void uart_tx_byte(unsigned char data)
{
	pistachio_uart_tx_byte(data);
}

unsigned char uart_rx_byte(void)
{
	return pistachio_uart_rx_byte();
}

void uart_tx_flush(void)
{
	pistachio_uart_tx_flush();
}

#endif /* __PRE_RAM__ */
