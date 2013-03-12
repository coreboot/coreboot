/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
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
#include <console/spkmodem.h>

static void spkmodem_tx_flush(void)
{
}

static unsigned char spkmodem_rx_byte(void)
{
	return 0;
}

static int spkmodem_tst_byte(void)
{
	return 0;
}


static const struct console_driver spkmodem_console __console = {
	.init     = spkmodem_init,
	.tx_byte  = spkmodem_tx_byte,
	.tx_flush = spkmodem_tx_flush,
	.rx_byte  = spkmodem_rx_byte,
	.tst_byte = spkmodem_tst_byte,
};

