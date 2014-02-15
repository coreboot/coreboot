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

#ifndef UART8250_H
#define UART8250_H

#if CONFIG_CONSOLE_SERIAL8250 || CONFIG_CONSOLE_SERIAL8250MEM

#if ((115200 % CONFIG_TTYS0_BAUD) != 0)
#error Bad ttyS0 baud rate
#endif

unsigned uart_platform_divisor(void);
unsigned uart_divisor(unsigned basefreq);

#if CONFIG_CONSOLE_SERIAL8250MEM
void uartmem_init(void);

/* and the same for memory mapped uarts */
unsigned char uart8250_mem_rx_byte(unsigned base_port);
int uart8250_mem_can_rx_byte(unsigned base_port);
void uart8250_mem_tx_byte(unsigned base_port, unsigned char data);
void uart8250_mem_tx_flush(unsigned base_port);
void uart8250_mem_init(unsigned base_port, unsigned divisor);
u32 uart_mem_init(void);

#if defined(__PRE_RAM__) && CONFIG_DRIVERS_OXFORD_OXPCIE
/* and special init for OXPCIe based cards */
extern int oxford_oxpcie_present;

void oxford_init(void);
#endif
#endif

#endif /* CONFIG_CONSOLE_SERIAL8250 || CONFIG_CONSOLE_SERIAL8250MEM */

#endif /* UART8250_H */
