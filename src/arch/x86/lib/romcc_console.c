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

#include <console/uart.h>
#include <console/ne2k.h>

/* While in romstage, console loglevel is built-time constant. */
#define console_log_level(msg_level) (CONFIG_DEFAULT_CONSOLE_LOGLEVEL >= msg_level)

#if CONFIG_CONSOLE_SERIAL && CONFIG_DRIVERS_UART_8250IO
#include "drivers/uart/util.c"
#include "drivers/uart/uart8250io.c"
#endif
#if CONFIG_CONSOLE_NE2K
#include "drivers/net/ne2k.c"
#endif

static void __console_tx_byte(unsigned char byte)
{
#if CONFIG_CONSOLE_SERIAL
	uart_tx_byte(byte);
#endif
#if CONFIG_CONSOLE_NE2K
	ne2k_append_data_byte(byte, CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
}

static void __console_tx_flush(void)
{
#if CONFIG_CONSOLE_SERIAL
	uart_tx_flush();
#endif
#if CONFIG_CONSOLE_NE2K
	ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
}

static void __console_tx_nibble(unsigned nibble)
{
	unsigned char digit;
	digit = nibble + '0';
	if (digit > '9') {
		digit += 39;
	}
	__console_tx_byte(digit);
}

static void __console_tx_char(int loglevel, unsigned char byte)
{
	if (console_log_level(loglevel)) {
		__console_tx_byte(byte);
		__console_tx_flush();
	}
}

static void __console_tx_hex8(int loglevel, unsigned char value)
{
	if (console_log_level(loglevel)) {
		__console_tx_nibble((value >>  4U) & 0x0fU);
		__console_tx_nibble(value & 0x0fU);
		__console_tx_flush();
	}
}

static void __console_tx_hex16(int loglevel, unsigned short value)
{
	if (console_log_level(loglevel)) {
		__console_tx_nibble((value >> 12U) & 0x0fU);
		__console_tx_nibble((value >>  8U) & 0x0fU);
		__console_tx_nibble((value >>  4U) & 0x0fU);
		__console_tx_nibble(value & 0x0fU);
		__console_tx_flush();
	}
}

static void __console_tx_hex32(int loglevel, unsigned int value)
{
	if (console_log_level(loglevel)) {
		__console_tx_nibble((value >> 28U) & 0x0fU);
		__console_tx_nibble((value >> 24U) & 0x0fU);
		__console_tx_nibble((value >> 20U) & 0x0fU);
		__console_tx_nibble((value >> 16U) & 0x0fU);
		__console_tx_nibble((value >> 12U) & 0x0fU);
		__console_tx_nibble((value >>  8U) & 0x0fU);
		__console_tx_nibble((value >>  4U) & 0x0fU);
		__console_tx_nibble(value & 0x0fU);
		__console_tx_flush();
	}
}

static void __console_tx_string(int loglevel, const char *str)
{
	if (console_log_level(loglevel)) {
		unsigned char ch;
		while((ch = *str++) != '\0') {
			if (ch == '\n')
				__console_tx_byte('\r');
			__console_tx_byte(ch);
		}
		__console_tx_flush();
	}
}

/* if included by romcc, include the sources, too. romcc can't use prototypes */
#include <console/console.c>
#include <console/init.c>
#include <console/post.c>
#include <console/die.c>
