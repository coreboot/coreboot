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
 */

#ifndef CONSOLE_UART_H
#define CONSOLE_UART_H

#include <rules.h>
#include <stdint.h>

/* Return the clock frequency UART uses as reference clock for
 * baudrate generator. */
unsigned int uart_platform_refclk(void);

#if IS_ENABLED(CONFIG_UART_OVERRIDE_BAUDRATE)
/* Return the baudrate, define this in your platform when using the above
   configuration. */
unsigned int get_uart_baudrate(void);
#else
static inline unsigned int get_uart_baudrate(void)
{
	return CONFIG_TTYS0_BAUD;
}
#endif

/* Returns the divisor value for a given baudrate.
 * The formula to satisfy is:
 *    refclk / divisor = baudrate * oversample
 */
unsigned int uart_baudrate_divisor(unsigned int baudrate,
	unsigned int refclk, unsigned int oversample);

/* Returns the oversample divisor multiplied by any other divisors that act
 * on the input clock
 */
unsigned int uart_input_clock_divider(void);

/* Bitbang out one byte on an 8n1 UART through the output function set_tx(). */
void uart_bitbang_tx_byte(unsigned char data, void (*set_tx)(int line_state));

void uart_init(int idx);
void uart_tx_byte(int idx, unsigned char data);
void uart_tx_flush(int idx);
unsigned char uart_rx_byte(int idx);

uintptr_t uart_platform_base(int idx);

#if !defined(__ROMCC__)
static inline void *uart_platform_baseptr(int idx)
{
	return (void *)uart_platform_base(idx);
}

void oxford_remap(unsigned int new_base);

#define __CONSOLE_SERIAL_ENABLE__	(IS_ENABLED(CONFIG_CONSOLE_SERIAL) && \
	(ENV_BOOTBLOCK || ENV_ROMSTAGE || ENV_RAMSTAGE || ENV_VERSTAGE || \
	ENV_POSTCAR || (ENV_SMM && IS_ENABLED(CONFIG_DEBUG_SMI))))

#if __CONSOLE_SERIAL_ENABLE__
static inline void __uart_init(void)
{
	uart_init(CONFIG_UART_FOR_CONSOLE);
}
static inline void __uart_tx_byte(u8 data)
{
	uart_tx_byte(CONFIG_UART_FOR_CONSOLE, data);
}
static inline void __uart_tx_flush(void)
{
	uart_tx_flush(CONFIG_UART_FOR_CONSOLE);
}
#else
static inline void __uart_init(void)		{}
static inline void __uart_tx_byte(u8 data)	{}
static inline void __uart_tx_flush(void)	{}
#endif

#if IS_ENABLED(CONFIG_GDB_STUB) && (ENV_ROMSTAGE || ENV_RAMSTAGE)
#define CONFIG_UART_FOR_GDB	CONFIG_UART_FOR_CONSOLE
static inline void __gdb_hw_init(void)	{ uart_init(CONFIG_UART_FOR_GDB); }
static inline void __gdb_tx_byte(u8 data)
{
	uart_tx_byte(CONFIG_UART_FOR_GDB, data);
}
static inline void __gdb_tx_flush(void)	{ uart_tx_flush(CONFIG_UART_FOR_GDB); }
static inline u8 __gdb_rx_byte(void)
{
	return uart_rx_byte(CONFIG_UART_FOR_GDB);
}
#endif

#endif /* __ROMCC__ */

#endif /* CONSOLE_UART_H */
