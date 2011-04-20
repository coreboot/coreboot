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

#include <stdint.h>
#include <console/loglevel.h>
#include <console/post_codes.h>

/* __PRE_RAM__ */
/* Using a global varible can cause problems when we reset the stack
 * from cache as ram to ram. If we make this a define USE_SHARED_STACK
 * we could use the same code on all architectures.
 */
#define console_loglevel CONFIG_DEFAULT_CONSOLE_LOGLEVEL
#if CONFIG_CONSOLE_SERIAL8250
#include <uart8250.h>
#endif

#if CONFIG_CONSOLE_SERIAL8250
#include "lib/uart8250.c"
#endif
#if CONFIG_CONSOLE_NE2K
#include "lib/ne2k.c"
#endif

static void __console_tx_byte(unsigned char byte)
{
#if CONFIG_CONSOLE_SERIAL8250
	uart8250_tx_byte(CONFIG_TTYS0_BASE, byte);
#endif
#if CONFIG_CONSOLE_NE2K
	ne2k_append_data_byte(byte, CONFIG_CONSOLE_NE2K_IO_PORT);
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
	if (console_loglevel >= loglevel) {
#if CONFIG_CONSOLE_SERIAL8250
		uart8250_tx_byte(CONFIG_TTYS0_BASE, byte);
#endif
#if CONFIG_CONSOLE_NE2K
		ne2k_append_data_byte(byte, CONFIG_CONSOLE_NE2K_IO_PORT);
		ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
	}
}

static void __console_tx_hex8(int loglevel, unsigned char value)
{
	if (console_loglevel >= loglevel) {
		__console_tx_nibble((value >>  4U) & 0x0fU);
		__console_tx_nibble(value & 0x0fU);
	}
#if CONFIG_CONSOLE_NE2K
		ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
}

static void __console_tx_hex16(int loglevel, unsigned short value)
{
	if (console_loglevel >= loglevel) {
		__console_tx_nibble((value >> 12U) & 0x0fU);
		__console_tx_nibble((value >>  8U) & 0x0fU);
		__console_tx_nibble((value >>  4U) & 0x0fU);
		__console_tx_nibble(value & 0x0fU);
	}
#if CONFIG_CONSOLE_NE2K
		ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
}

static void __console_tx_hex32(int loglevel, unsigned int value)
{
	if (console_loglevel >= loglevel) {
		__console_tx_nibble((value >> 28U) & 0x0fU);
		__console_tx_nibble((value >> 24U) & 0x0fU);
		__console_tx_nibble((value >> 20U) & 0x0fU);
		__console_tx_nibble((value >> 16U) & 0x0fU);
		__console_tx_nibble((value >> 12U) & 0x0fU);
		__console_tx_nibble((value >>  8U) & 0x0fU);
		__console_tx_nibble((value >>  4U) & 0x0fU);
		__console_tx_nibble(value & 0x0fU);
	}
#if CONFIG_CONSOLE_NE2K
		ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
}

static void __console_tx_string(int loglevel, const char *str)
{
	if (console_loglevel >= loglevel) {
		unsigned char ch;
		while((ch = *str++) != '\0') {
			if (ch == '\n')
				__console_tx_byte('\r');
			__console_tx_byte(ch);
		}
#if CONFIG_CONSOLE_NE2K
		ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
	}
}

/* if included by romcc, include the sources, too. romcc can't use prototypes */
#include <console/console.c>
#include <console/post.c>
#include <console/die.c>


