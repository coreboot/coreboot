/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <console/vtxprintf.h>
#if CONFIG_CONSOLE_SERIAL8250 || CONFIG_CONSOLE_SERIAL8250MEM
#include <uart8250.h>
#endif
#if CONFIG_USBDEBUG
#include <usbdebug.h>
#endif
#if CONFIG_CONSOLE_NE2K
#include <console/ne2k.h>
#endif

static void console_tx_byte(unsigned char byte)
{
	if (byte == '\n')
		console_tx_byte('\r');

#if CONFIG_CONSOLE_SERIAL8250MEM
	uart8250_mem_tx_byte(CONFIG_OXFORD_OXPCIE_BASE_ADDRESS + 0x1000, byte);
#endif
#if CONFIG_CONSOLE_SERIAL8250
	uart8250_tx_byte(CONFIG_TTYS0_BASE, byte);
#endif
#if CONFIG_USBDEBUG
	usbdebug_tx_byte(byte);
#endif
#if CONFIG_CONSOLE_NE2K
	ne2k_append_data(&byte, 1, CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
}

int do_printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	if (msg_level > console_loglevel) {
		return 0;
	}

	va_start(args, fmt);
	i = vtxprintf(console_tx_byte, fmt, args);
	va_end(args);
#if CONFIG_CONSOLE_NE2K
	ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
	return i;
}
