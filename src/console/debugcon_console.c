/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Red Hat Inc.
 * Written by Gerd Hoffmann <kraxel@redhat.com>
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
#include <arch/io.h>

static void debugcon_init(void)
{
}

static void debugcon_tx_byte(unsigned char data)
{
	outb(data, CONFIG_CONSOLE_DEBUGCON_PORT);
}

static void debugcon_tx_flush(void)
{
}

static unsigned char debugcon_rx_byte(void)
{
	return 0;
}

static int debugcon_tst_byte(void)
{
	return 0;
}

static const struct console_driver debugcon_console __console = {
	.init = debugcon_init,
	.tx_byte = debugcon_tx_byte,
	.tx_flush = debugcon_tx_flush,
	.rx_byte = debugcon_rx_byte,
	.tst_byte = debugcon_tst_byte,
};
