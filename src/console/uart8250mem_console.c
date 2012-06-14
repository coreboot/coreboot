/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
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

#include <console/console.h>
#include <uart8250.h>
#include <pc80/mc146818rtc.h>

static u32 uart_bar = 0;

void uartmem_init(void)
{
	uart_bar = uart_mem_init();
}

u32 uartmem_getbaseaddr(void)
{
	return uart_bar;
}

static void uartmem_tx_byte(unsigned char data)
{
	if (!uart_bar)
		return;

	uart8250_mem_tx_byte(uart_bar, data);
}

static void uartmem_tx_flush(void)
{
	uart8250_mem_tx_flush(uart_bar);
}

static unsigned char uartmem_rx_byte(void)
{
	if (!uart_bar)
		return 0;

	return uart8250_mem_rx_byte(uart_bar);
}

static int uartmem_tst_byte(void)
{
	if (!uart_bar)
		return 0;

	return uart8250_mem_can_rx_byte(uart_bar);
}

static const struct console_driver uart8250mem_console __console = {
	.init     = uartmem_init,
	.tx_byte  = uartmem_tx_byte,
	.tx_flush = uartmem_tx_flush,
	.rx_byte  = uartmem_rx_byte,
	.tst_byte = uartmem_tst_byte,
};
