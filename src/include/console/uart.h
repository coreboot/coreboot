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

void uart_init(void);
void uart_tx_byte(unsigned char data);
void uart_tx_flush(void);
unsigned char uart_rx_byte(void);
int uart_can_rx_byte(void);


unsigned uart_divisor(unsigned basefreq);
unsigned uart_platform_divisor(void);

unsigned uart_platform_base(int idx);
uint32_t uartmem_getbaseaddr(void);

void oxford_init(void);
void oxford_remap(u32 new_base);

#endif /* CONSOLE_UART_H */
