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
#include <console/uart.h>

static void uartmem_init(void)
{
	uart_init();
}

static void uartmem_tx_byte(unsigned char data)
{
	uart_tx_byte(data);
}

static void uartmem_tx_flush(void)
{
	uart_tx_flush();
}

static unsigned char uartmem_rx_byte(void)
{
	return uart_rx_byte();
}

static int uartmem_tst_byte(void)
{
	return uart_can_rx_byte();
}

static const struct console_driver uart8250mem_console __console = {
	.init     = uartmem_init,
	.tx_byte  = uartmem_tx_byte,
	.tx_flush = uartmem_tx_flush,
	.rx_byte  = uartmem_rx_byte,
	.tst_byte = uartmem_tst_byte,
};
