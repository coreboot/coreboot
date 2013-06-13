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
#include <pc80/mc146818rtc.h>

static struct ehci_debug_info dbg_info;

void set_ehci_base(unsigned ehci_base)
{
	unsigned diff;

	if (!dbg_info.ehci_debug)
		return;

	diff = dbg_info.ehci_caps - ehci_base;
	dbg_info.ehci_regs -= diff;
	dbg_info.ehci_debug -= diff;
	dbg_info.ehci_caps = ehci_base;
}

void set_ehci_debug(unsigned ehci_debug)
{
	dbg_info.ehci_debug = ehci_debug;
}

unsigned get_ehci_debug(void)
{
	return dbg_info.ehci_debug;
}

static void dbgp_init(void)
{
#if !CONFIG_EARLY_CONSOLE
	enable_usbdebug(CONFIG_USBDEBUG_DEFAULT_PORT);
#endif
	usbdebug_init(CONFIG_EHCI_BAR, CONFIG_EHCI_DEBUG_OFFSET, &dbg_info);
}

static void dbgp_tx_byte(unsigned char data)
{
	usbdebug_tx_byte(&dbg_info, data);
}

static unsigned char dbgp_rx_byte(void)
{
	unsigned char data = 0xff;

	if (dbg_info.ehci_debug)
		dbgp_bulk_read_x(&dbg_info, &data, 1);

	return data;
}

static void dbgp_tx_flush(void)
{
	usbdebug_tx_flush(&dbg_info);
}

static int dbgp_tst_byte(void)
{
	return (int)dbg_info.ehci_debug;
}

static const struct console_driver usbdebug_direct_console __console = {
	.init     = dbgp_init,
	.tx_byte  = dbgp_tx_byte,
	.tx_flush = dbgp_tx_flush,
	.rx_byte  = dbgp_rx_byte,
	.tst_byte = dbgp_tst_byte,
};
