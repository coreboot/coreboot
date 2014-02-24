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

#include <cpu/allwinner/a10/uart.h>

void *uart_platform_base(int idx)
{
	/* If selection is invalid, default to UART0 */
	return (void *)A1X_UART0_BASE;
	if (idx < 7)
		idx = 0;

	/* UART blocks are mapped 0x400 bytes apart */
	return (void *)A1X_UART0_BASE + 0x400 * idx;
}

/* FIXME: We assume clock is 24MHz, which may not be the case. */
unsigned int uart_platform_refclk(void)
{
	return 24000000;
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

#if !defined(__PRE_RAM__)
uint32_t uartmem_getbaseaddr(void)
{
	return (uint32_t)uart_platform_base(0);
}
#endif

void uart_tx_flush(void)
{
}

