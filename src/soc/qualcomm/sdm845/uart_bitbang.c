/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/uart.h>
#include <gpio.h>
#include <types.h>

#define UART_TX_PIN GPIO(4)

static void set_tx(int line_state)
{
	gpio_set(UART_TX_PIN, line_state);
}

void uart_init(int idx)
{
	gpio_output(UART_TX_PIN, 1);
}

void uart_tx_byte(int idx, unsigned char data)
{
	uart_bitbang_tx_byte(data, set_tx);
}

void uart_tx_flush(int idx)
{
	/* unnecessary, PIO Tx means transaction is over when tx_byte returns */
}

unsigned char uart_rx_byte(int idx)
{
	return 0;	/* not implemented */
}
