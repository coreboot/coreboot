/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <string.h>
#include <console/console.h>
#include <usbdebug.h>

static void dbgp_init(void)
{
	usbdebug_init();
}

static void dbgp_tx_byte(unsigned char data)
{
	usbdebug_tx_byte(dbgp_console_output(), data);
}

static unsigned char dbgp_rx_byte(void)
{
	unsigned char data = 0xff;

	if (dbgp_ep_is_active(dbgp_console_input()))
		dbgp_bulk_read_x(dbgp_console_input(), &data, 1);

	return data;
}

static void dbgp_tx_flush(void)
{
	usbdebug_tx_flush(dbgp_console_output());
}

static int dbgp_tst_byte(void)
{
	return dbgp_ep_is_active(dbgp_console_input());
}

static const struct console_driver usbdebug_direct_console __console = {
	.init     = dbgp_init,
	.tx_byte  = dbgp_tx_byte,
	.tx_flush = dbgp_tx_flush,
	.rx_byte  = dbgp_rx_byte,
	.tst_byte = dbgp_tst_byte,
};
