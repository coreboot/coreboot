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
#include <console/usb.h>

static void dbgp_init(void)
{
	usbdebug_init();
}

static void dbgp_tx_byte(unsigned char data)
{
	usb_tx_byte(0, data);
}

static unsigned char dbgp_rx_byte(void)
{
	return usb_rx_byte(0);
}

static void dbgp_tx_flush(void)
{
	usb_tx_flush(0);
}

static int dbgp_tst_byte(void)
{
	return usb_can_rx_byte(0);
}

static const struct console_driver usbdebug_direct_console __console = {
	.init     = dbgp_init,
	.tx_byte  = dbgp_tx_byte,
	.tx_flush = dbgp_tx_flush,
	.rx_byte  = dbgp_rx_byte,
	.tst_byte = dbgp_tst_byte,
};
