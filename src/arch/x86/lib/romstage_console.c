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

#include <smp/node.h>
#include <console/console.h>
#include <console/cbmem_console.h>
#include <console/uart.h>
#include <console/usb.h>
#include <console/ne2k.h>
#include <console/spkmodem.h>
#include <console/vtxprintf.h>

void console_tx_byte(unsigned char byte)
{
	if (byte == '\n')
		console_tx_byte('\r');

#if CONFIG_CONSOLE_SERIAL8250MEM
	uart_tx_byte(byte);
#endif
#if CONFIG_CONSOLE_SERIAL8250
	uart_tx_byte(byte);
#endif
#if CONFIG_USBDEBUG && (CONFIG_USBDEBUG_IN_ROMSTAGE || !defined(__PRE_RAM__))
	usbdebug_tx_byte(dbgp_console_output(), byte);
#endif
#if CONFIG_CONSOLE_NE2K
	ne2k_append_data(&byte, 1, CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
#if CONFIG_CONSOLE_CBMEM && (CONFIG_EARLY_CBMEM_INIT || !defined(__PRE_RAM__))
	cbmemc_tx_byte(byte);
#endif
#if CONFIG_SPKMODEM
	spkmodem_tx_byte(byte);
#endif
}

void console_tx_flush(void)
{
#if CONFIG_CONSOLE_SERIAL8250MEM
	uart_tx_flush();
#endif
#if CONFIG_CONSOLE_SERIAL8250
	uart_tx_flush();
#endif
#if CONFIG_CONSOLE_NE2K
	ne2k_transmit(CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
#if CONFIG_USBDEBUG && (CONFIG_USBDEBUG_IN_ROMSTAGE || !defined(__PRE_RAM__))
	usbdebug_tx_flush(dbgp_console_output());
#endif
}

int do_printk(int msg_level, const char *fmt, ...)
{
	va_list args;
	int i;

	if (msg_level > console_loglevel) {
		return 0;
	}

#if CONFIG_SQUELCH_EARLY_SMP
	if (!boot_cpu())
		return 0;
#endif

	va_start(args, fmt);
	i = vtxprintf(console_tx_byte, fmt, args);
	va_end(args);

	console_tx_flush();

	return i;
}
