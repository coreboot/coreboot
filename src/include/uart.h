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

/* madness. Uarts are a mess. If you include this file, it
 * includes ALL uart implementations which may be needed.
 * No need to include them separately, and include this file FIRST.
 * At least one (but at most one) of the files needs to define
 * uart_init().
 */
#ifndef UART_H
#define UART_H

#if CONFIG_CONSOLE_SERIAL8250 || CONFIG_CONSOLE_SERIAL8250MEM
#include <uart8250.h>
#endif

#if CONFIG_CONSOLE_SERIAL_UART
unsigned char uart_rx_byte(void);
void uart_tx_byte(unsigned char data);
void uart_tx_flush(void);
void uart_init(void);
#endif

#if CONFIG_HAVE_UART_MEMORY_MAPPED
uint32_t uartmem_getbaseaddr(void);
#endif

#endif /* UART_H */
