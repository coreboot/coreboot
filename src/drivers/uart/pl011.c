/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright 2018-present Facebook, Inc.
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

#include <arch/io.h>
#include <boot/coreboot_tables.h>
#include <console/uart.h>
#include <drivers/uart/pl011.h>

void uart_init(int idx)
{
}

void uart_tx_byte(int idx, unsigned char data)
{
	struct pl011_uart *regs = uart_platform_baseptr(idx);

	write8(&regs->dr, data);
	uart_tx_flush(idx);
}

void uart_tx_flush(int idx)
{
	struct pl011_uart *regs = uart_platform_baseptr(idx);

	/* FIXME: add a timeout */
	while (!(read32(&regs->fr) & PL011_UARTFR_TXFE))
		;
}

unsigned char uart_rx_byte(int idx)
{
	return 0;
}

#ifndef __PRE_RAM__
void uart_fill_lb(void *data)
{
	struct lb_serial serial;
	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	serial.baud = get_uart_baudrate();
	serial.regwidth = 1;
	serial.input_hertz = uart_platform_refclk();
	serial.uart_pci_addr = CONFIG_UART_PCI_ADDR;
	lb_add_serial(&serial, data);

	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, data);
}
#endif
