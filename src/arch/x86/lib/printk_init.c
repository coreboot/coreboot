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
#include <uart8250.h>

#if CONFIG_CONSOLE_NE2K
#include <console/ne2k.h>
#endif

static void console_tx_byte(unsigned char byte)
{
#if CONFIG_CONSOLE_NE2K
#ifdef __PRE_RAM__
	ne2k_append_data(&byte, 1, CONFIG_CONSOLE_NE2K_IO_PORT);
#endif
#endif
	if (byte == '\n')
		uart8250_tx_byte(CONFIG_TTYS0_BASE, '\r');

	uart8250_tx_byte(CONFIG_TTYS0_BASE, byte);
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
