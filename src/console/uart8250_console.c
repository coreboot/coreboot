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
	static const unsigned char div[8] = { 1, 2, 3, 6, 12, 24, 48, 96 };
	int b_index = 0;
	/* TODO the divisor calculation is hard coded to standard UARTs. Some
	 * UARTs won't work with these values. This should be a property of the
	 * UART used, worst case a Kconfig variable. For now live with hard
	 * codes as the only devices that might be different are the iWave
	 * iRainbowG6 and the OXPCIe952 card (and the latter is memory mapped)
	 */
	unsigned int divisor = 115200 / CONFIG_TTYS0_BAUD;

	if (get_option(&b_index, "baud_rate") == 0) {
		divisor = div[b_index];
	}
	uart8250_init(CONFIG_TTYS0_BASE, divisor);
}

static void ttyS0_tx_byte(unsigned char data)
{
	uart8250_tx_byte(CONFIG_TTYS0_BASE, data);
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
	.rx_byte  = ttyS0_rx_byte,
	.tst_byte = ttyS0_tst_byte,
};
