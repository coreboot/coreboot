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

#include <boot/coreboot_tables.h>
#include <console/uart.h>

static void pl011_uart_tx_byte(unsigned int *uart_base, unsigned char data)
{
	*uart_base = (unsigned int)data;
}

unsigned int uart_platform_base(int idx)
{
	return CONFIG_CONSOLE_SERIAL_UART_ADDRESS;
}

void uart_init(void)
{
}

void uart_tx_byte(unsigned char data)
{
	unsigned int *uart_base = uart_platform_baseptr(0);
	pl011_uart_tx_byte(uart_base, data);
}

void uart_tx_flush(void)
{
}

unsigned char uart_rx_byte(void)
{
	return 0;
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
