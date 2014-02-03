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
#include <console/cbmem_console.h>
#include <console/uart.h>
#include <console/usb.h>
#include <console/ne2k.h>
#include <console/spkmodem.h>

void console_hw_init(void)
{
#if CONFIG_CONSOLE_SERIAL
	uart_init();
#endif
#if CONFIG_CONSOLE_NE2K
	ne2k_init(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
#if CONFIG_CONSOLE_CBMEM && CONFIG_EARLY_CBMEM_INIT && !defined(__BOOT_BLOCK__)
	cbmemc_init();
#endif
#if CONFIG_SPKMODEM
	spkmodem_init();
#endif
#if CONFIG_CONSOLE_USB && CONFIG_USBDEBUG_IN_ROMSTAGE && !defined(__BOOT_BLOCK__)
	usbdebug_init();
#endif
}

#ifndef __PRE_RAM__
void console_tx_byte(unsigned char byte)
{
}

void console_tx_flush(void)
{
}
#endif
