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

#include <console/console.h>
#include <console/qemu_debugcon.h>
#include <uart.h>

void uart_init(void)
{
}

void uart_tx_byte(unsigned char data)
{
	qemu_debugcon_tx_byte(data);
}

void uart_tx_flush(void)
{
}

#if !defined(__PRE_RAM__)
static const struct console_driver pl011_uart_console __console = {
	.init     = uart_init,
	.tx_byte  = uart_tx_byte,
	.tx_flush = uart_tx_flush,
};
#endif
