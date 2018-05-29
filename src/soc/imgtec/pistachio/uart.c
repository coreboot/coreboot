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
 */

#include <arch/io.h>
#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <console/uart.h>
#include <device/device.h>
#include <delay.h>
#include <drivers/uart/uart8250reg.h>

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

unsigned int uart_platform_refclk(void)
{
	/* 1.8433179 MHz */
	return 1843318;
}

void uart_init(int idx)
{
	u32 base = CONFIG_CONSOLE_SERIAL_UART_ADDRESS;
	if (!base)
		return;

	unsigned int div;
	div = uart_baudrate_divisor(get_uart_baudrate(),
				    uart_platform_refclk(), 16);
	uart8250_mem_init(base, div);
}

void uart_tx_byte(int idx, unsigned char data)
{
	uart8250_mem_tx_byte(CONFIG_CONSOLE_SERIAL_UART_ADDRESS, data);
}

unsigned char uart_rx_byte(int idx)
{
	return uart8250_mem_rx_byte(CONFIG_CONSOLE_SERIAL_UART_ADDRESS);
}

void uart_tx_flush(int idx)
{
	uart8250_mem_tx_flush(CONFIG_CONSOLE_SERIAL_UART_ADDRESS);
}

#ifndef __PRE_RAM__
void uart_fill_lb(void *data)
{
	struct lb_serial serial;
	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = CONFIG_CONSOLE_SERIAL_UART_ADDRESS;
	serial.baud = get_uart_baudrate();
	serial.regwidth = 1 << UART_SHIFT;
	lb_add_serial(&serial, data);

	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, data);
}
#endif
