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

static void pl011_uart_tx_byte(unsigned char data)
{
	static volatile unsigned int *uart0_address =
		(unsigned int *) CONFIG_CONSOLE_SERIAL_UART_ADDRESS;

	*uart0_address = (unsigned int)data;
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
	pl011_uart_tx_byte(data);
}

void uart_tx_flush(void)
{
}
