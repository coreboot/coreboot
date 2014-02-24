/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/uart.h>

static void pl011_uart_tx_byte(unsigned int *uart_base, unsigned char data)
{
	*uart_base = (unsigned int)data;
}

void *uart_platform_base(int idx)
{
	return (void *)CONFIG_CONSOLE_SERIAL_UART_ADDRESS;
}

#if !defined(__PRE_RAM__)
uint32_t uartmem_getbaseaddr(void)
{
	return CONFIG_CONSOLE_SERIAL_UART_ADDRESS;
}
#endif

void uart_init(void)
{
}

void uart_tx_byte(unsigned char data)
{
	unsigned int *uart_base = (unsigned int *) uart_platform_base(0);
	pl011_uart_tx_byte(uart_base, data);
}

void uart_tx_flush(void)
{
}

unsigned char uart_rx_byte(void)
{
	return 0;
}
