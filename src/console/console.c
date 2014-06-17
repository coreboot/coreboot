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

#include <console/cbmem_console.h>
#include <console/ne2k.h>
#include <console/qemu_debugcon.h>
#include <console/spkmodem.h>
#include <console/streams.h>
#include <console/uart.h>
#include <console/usb.h>
#include <rules.h>

void console_hw_init(void)
{
	__cbmemc_init();
	__spkmodem_init();
	__qemu_debugcon_init();

	__uart_init();
	__ne2k_init();
	__usbdebug_init();
}

void console_tx_byte(unsigned char byte)
{
	__cbmemc_tx_byte(byte);
	__spkmodem_tx_byte(byte);
	__qemu_debugcon_tx_byte(byte);

	__uart_tx_byte(byte);
	__ne2k_tx_byte(byte);
	__usb_tx_byte(byte);
}

void console_tx_flush(void)
{
	__uart_tx_flush();
	__ne2k_tx_flush();
	__usb_tx_flush();
}


#if CONFIG_GDB_STUB && (ENV_ROMSTAGE || ENV_RAMSTAGE)
void gdb_hw_init(void)
{
	__gdb_hw_init();
}

void gdb_tx_byte(unsigned char byte)
{
	__gdb_tx_byte(byte);
}

void gdb_tx_flush(void)
{
	__gdb_tx_flush();
}

unsigned char gdb_rx_byte(void)
{
	return __gdb_rx_byte();
}
#endif
