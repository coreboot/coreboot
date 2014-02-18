/*
 * Glue to UART code to enable serial console
 *
 * Copyright 2013 Google Inc.
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include <config.h>
#include <types.h>
#include <console/uart.h>
#include <arch/io.h>
#include <boot/coreboot_tables.h>

#include <cpu/allwinner/a10/uart.h>

static void *get_console_uart_base_addr(void)
{
	/* This big block gets compiled to a constant, not a function call */
	if (CONFIG_CONSOLE_SERIAL_UART0)
		return (void *)A1X_UART0_BASE;
	else if (CONFIG_CONSOLE_SERIAL_UART1)
		return (void *)A1X_UART1_BASE;
	else if (CONFIG_CONSOLE_SERIAL_UART2)
		return (void *)A1X_UART2_BASE;
	else if (CONFIG_CONSOLE_SERIAL_UART3)
		return (void *)A1X_UART3_BASE;
	else if (CONFIG_CONSOLE_SERIAL_UART4)
		return (void *)A1X_UART4_BASE;
	else if (CONFIG_CONSOLE_SERIAL_UART5)
		return (void *)A1X_UART5_BASE;
	else if (CONFIG_CONSOLE_SERIAL_UART6)
		return (void *)A1X_UART6_BASE;
	else if (CONFIG_CONSOLE_SERIAL_UART7)
		return (void *)A1X_UART7_BASE;

	/* If selection is invalid, default to UART0 */
	return (void *)A1X_UART0_BASE;
}

/* FIXME: We assume clock is 24MHz, which may not be the case. */
unsigned int uart_platform_refclk(void)
{
	return 24000000;
}

unsigned int uart_platform_base(int idx)
{
	return (unsigned int)get_console_uart_base_addr();
}

void uart_init(void)
{
	void *uart_base = (void *) uart_platform_base(0);

	/* Use default 8N1 encoding */
	a10_uart_configure(uart_base, default_baudrate(),
		8, UART_PARITY_NONE, 1);
	a10_uart_enable_fifos(uart_base);
}

unsigned char uart_rx_byte(void)
{
	void *uart_base = (void *) uart_platform_base(0);
	return a10_uart_rx_blocking(uart_base);
}

void uart_tx_byte(unsigned char data)
{
	void *uart_base = (void *) uart_platform_base(0);
	a10_uart_tx_blocking(uart_base, data);
}

void uart_tx_flush(void)
{
}

#ifndef __PRE_RAM__
void uart_fill_lb(void *data)
{
	struct lb_serial serial;
	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = uart_platform_base(0);
	serial.baud = default_baudrate();
	lb_add_serial(&serial, data);

	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, data);
}
#endif
