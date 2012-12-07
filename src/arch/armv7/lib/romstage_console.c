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
#if CONFIG_SERIAL_CONSOLE
#include <uart.h>
#endif
#if CONFIG_USBDEBUG
#include <usbdebug.h>
#endif

/* FIXME: need to make console driver more generic */
void console_tx_byte(unsigned char byte)
{
	if (byte == '\n')
		console_tx_byte('\r');

#if CONFIG_SERIAL_CONSOLE
	uart_tx_byte(byte);
#endif
#if CONFIG_USBDEBUG
	usbdebug_tx_byte(0, byte);
#endif
#if CONFIG_CONSOLE_CBMEM
	cbmemc_tx_byte(byte);
#endif
}

/* FIXME(dhendrix): add this back in */
#if 0
static void console_tx_flush(void)
{
#if CONFIG_CONSOLE_SERIAL
	uart_tx_flush(CONFIG_CONSOLE_SERIAL_UART_ADDRESS);
#endif
#if CONFIG_USBDEBUG
	usbdebug_tx_flush(0);
#endif
}
#endif

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

//	console_tx_flush();

	return i;
}
