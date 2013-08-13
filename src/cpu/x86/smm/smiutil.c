/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <arch/io.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <console/console.h>
#include <console/vtxprintf.h>

#if CONFIG_CONSOLE_SERIAL8250MEM
static u32 serial8250mem_base_address = 0;
#endif

void console_tx_flush(void)
{
}

void console_tx_byte(unsigned char byte)
{
	if (byte == '\n')
		console_tx_byte('\r');

#if CONFIG_CONSOLE_SERIAL8250MEM
	if (serial8250mem_base_address)
		uart8250_mem_tx_byte(serial8250mem_base_address, byte);
#endif
#if CONFIG_CONSOLE_SERIAL8250
	uart8250_tx_byte(CONFIG_TTYS0_BASE, byte);
#endif
#if CONFIG_CONSOLE_NE2K
	ne2k_append_data(&byte, 1, CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
}

void console_init(void)
{
#if CONFIG_DEBUG_SMI
	console_loglevel = CONFIG_DEFAULT_CONSOLE_LOGLEVEL;
#if CONFIG_CONSOLE_SERIAL8250
	uart_init();
#endif
#if CONFIG_CONSOLE_SERIAL8250MEM
	serial8250mem_base_address = uart_mem_init();
#endif
#else
	console_loglevel = 1;
#endif
}

