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

/* FIXME: need to make console driver more generic */
void console_tx_byte(unsigned char byte)
{
	if (byte == '\n')
		console_tx_byte('\r');

#if CONFIG_CONSOLE_SERIAL8250MEM
	if (oxford_oxpcie_present) {
		uart8250_mem_tx_byte(
			CONFIG_OXFORD_OXPCIE_BASE_ADDRESS + 0x1000, byte);
	}
#endif
#if CONFIG_CONSOLE_SERIAL8250
	uart8250_tx_byte(CONFIG_TTYS0_BASE, byte);
#endif
#if CONFIG_CONSOLE_SERIAL_UART
	uart_tx_byte(byte);
#endif
#if CONFIG_USBDEBUG
	usbdebug_tx_byte(0, byte);
#endif
#if CONFIG_CONSOLE_CBMEM && !defined(__BOOT_BLOCK__)
	cbmemc_tx_byte(byte);
#endif
}

static void _console_tx_flush(void)
{
#if CONFIG_CONSOLE_SERIAL8250MEM
	uart8250_mem_tx_flush(CONFIG_OXFORD_OXPCIE_BASE_ADDRESS + 0x1000);
#endif
#if CONFIG_CONSOLE_SERIAL8250
	uart8250_tx_flush(CONFIG_TTYS0_BASE);
#endif
#if CONFIG_CONSOLE_SERIAL_UART
	uart_tx_flush();
#endif
#if CONFIG_USBDEBUG
	usbdebug_tx_flush(0);
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

	_console_tx_flush();

	return i;
}
