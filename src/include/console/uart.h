/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
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

#ifndef CONSOLE_UART_H
#define CONSOLE_UART_H

#include <stdint.h>

/* Return the clock frequency UART uses as reference clock for
 * baudrate generator. */
unsigned int uart_platform_refclk(void);

/* Return the baudrate determined from option_table, or when that is
 * not used, CONFIG_TTYS0_BAUD.
 */
unsigned int default_baudrate(void);

/* Returns the divisor value for a given baudrate.
 * The formula to satisfy is:
 *    refclk / divisor = baudrate * oversample
 */
unsigned int uart_baudrate_divisor(unsigned int baudrate,
	unsigned int refclk, unsigned int oversample);


void uart_init(void);
void uart_tx_byte(unsigned char data);
void uart_tx_flush(void);
unsigned char uart_rx_byte(void);
int uart_can_rx_byte(void);

unsigned int uart_platform_base(int idx);

void oxford_init(void);
void oxford_remap(unsigned int new_base);

#endif /* CONSOLE_UART_H */
