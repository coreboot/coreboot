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

#include <console/streams.h>
#include <console/early_print.h>

/* Include the sources. */
#if CONFIG_CONSOLE_SERIAL && CONFIG_DRIVERS_UART_8250IO
#include "drivers/uart/util.c"
#include "drivers/uart/uart8250io.c"
#endif
#if CONFIG_CONSOLE_NE2K
#include "drivers/net/ne2k.c"
#endif

void console_hw_init(void)
{
#if CONFIG_CONSOLE_SERIAL
	uart_init();
#endif
#if CONFIG_CONSOLE_NE2K
	ne2k_init(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
}

void console_tx_byte(unsigned char byte)
{
#if CONFIG_CONSOLE_SERIAL
	uart_tx_byte(byte);
#endif
#if CONFIG_CONSOLE_NE2K
	ne2k_append_data_byte(byte, CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
}

void console_tx_flush(void)
{
#if CONFIG_CONSOLE_SERIAL
	uart_tx_flush();
#endif
#if CONFIG_CONSOLE_NE2K
	ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
}

#include <console/early_print.c>
#include <console/init.c>
#include <console/post.c>
#include <console/die.c>
