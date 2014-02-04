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

#include <console/console.h>
#include <console/uart.h>

#if CONFIG_DEBUG_SMI
void console_tx_flush(void)
{
}

void console_tx_byte(unsigned char byte)
{
#if CONFIG_CONSOLE_SERIAL
	uart_tx_byte(byte);
#endif
}
#endif

void console_init(void)
{
#if CONFIG_DEBUG_SMI
	console_loglevel = CONFIG_DEFAULT_CONSOLE_LOGLEVEL;
#if CONFIG_CONSOLE_SERIAL
	uart_init();
#endif
#endif
}

