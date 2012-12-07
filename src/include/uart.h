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
 *
 * FIXME(dhendrix): This file contains generic prototypes for UART functions.
 * The existing headers are too specific to the 8250, so we need a better
 * abstraction for use with non-8250 UARTs.
 */

#ifndef UART_H
#define UART_H

unsigned char uart_rx_byte(void);
void uart_tx_byte(unsigned char data);
void uart_tx_flush(void);

void uart_init(void);

#endif /* UART_H */
