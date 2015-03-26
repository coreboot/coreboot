/*
 * This file is part of the coreboot project.
 * Copyright 2014 Google Inc.
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
 * Foundation, Inc.
 */

#include <console/console.h>
#include <console/uart.h>

static const struct console_driver uart_console __console = {
	.init     = uart_init,
	.tx_byte  = uart_tx_byte,
	.tx_flush = uart_tx_flush,
	.rx_byte  = uart_rx_byte,
	.tst_byte = uart_can_rx_byte,
};
