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

#include <console/console.h>
#include <uart8250.h>
#include <pc80/mc146818rtc.h>

static void ttyS0_init(void)
{
	uart_init();
}

static void ttyS0_tx_byte(unsigned char data)
{
	uart8250_tx_byte(CONFIG_TTYS0_BASE, data);
}

static void ttyS0_tx_flush(void)
{
	uart8250_tx_flush(CONFIG_TTYS0_BASE);
}

static unsigned char ttyS0_rx_byte(void)
{
	return uart8250_rx_byte(CONFIG_TTYS0_BASE);
}

static int ttyS0_tst_byte(void)
{
	return uart8250_can_rx_byte(CONFIG_TTYS0_BASE);
}

static const struct console_driver uart8250_console __console = {
	.init     = ttyS0_init,
	.tx_byte  = ttyS0_tx_byte,
	.tx_flush = ttyS0_tx_flush,
	.rx_byte  = ttyS0_rx_byte,
	.tst_byte = ttyS0_tst_byte,
};
