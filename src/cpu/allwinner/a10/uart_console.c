/*
 * Glue to UART code to enable serial console
 *
 * Copyright 2013 Google Inc.
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include <config.h>
#include <types.h>
#include <uart.h>
#include <arch/io.h>

#include <console/console.h>
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

static u32 get_console_uart_baud(void)
{
	if (CONFIG_CONSOLE_SERIAL_115200)
		return 115200;
	else if (CONFIG_CONSOLE_SERIAL_57600)
		return 57600;
	else if (CONFIG_CONSOLE_SERIAL_38400)
		return 34800;
	else if (CONFIG_CONSOLE_SERIAL_19200)
		return 19200;
	else if (CONFIG_CONSOLE_SERIAL_9600)
		return 9600;

	/* Default to 115200 if selection is invalid */
	return 115200;
}

static void a10_uart_init_dev(void)
{
	void *uart_base = get_console_uart_base_addr();
	/* Use default 8N1 encoding */
	a10_uart_configure(uart_base, get_console_uart_baud(),
			   8, UART_PARITY_NONE, 1);
	a10_uart_enable_fifos(uart_base);
}

static unsigned char a10_uart_rx_byte(void)
{
	return a10_uart_rx_blocking(get_console_uart_base_addr());
}

static void a10_uart_tx_byte(unsigned char data)
{
	a10_uart_tx_blocking(get_console_uart_base_addr(), data);
}

uint32_t uartmem_getbaseaddr(void)
{
	return (uint32_t) get_console_uart_base_addr();
}

#if !defined(__PRE_RAM__)
static const struct console_driver a10_uart_console __console = {
	.init = a10_uart_init_dev,
	.tx_byte = a10_uart_tx_byte,
	.rx_byte = a10_uart_rx_byte,
};
#else

void uart_init(void)
{
	a10_uart_init_dev();
}

unsigned char uart_rx_byte(void)
{
	return a10_uart_rx_byte();
}

void uart_tx_byte(unsigned char data)
{
	a10_uart_tx_byte(data);
}

void uart_tx_flush(void)
{
}

#endif
