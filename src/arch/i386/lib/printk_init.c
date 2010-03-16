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
#include <console/loglevel.h>
#include <uart8250.h>

/* Using a global varible can cause problems when we reset the stack 
 * from cache as ram to ram. If we make this a define USE_SHARED_STACK
 * we could use the same code on all architectures.
 */
#if 0
int console_loglevel = CONFIG_DEFAULT_CONSOLE_LOGLEVEL;
#else
#define console_loglevel CONFIG_DEFAULT_CONSOLE_LOGLEVEL
#endif

void console_tx_byte(unsigned char byte)
{
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

	return i;
}
