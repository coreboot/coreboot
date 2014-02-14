/*
 * This file is part of the coreboot project.
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
#include <console/uart.h>
#if CONFIG_USE_OPTION_TABLE
#include <option.h>
#include "option_table.h"
#endif

#if CONFIG_CONSOLE_SERIAL
/* Treat the default base frequency 115200 as a special case
 * to avoid runtime division. Might help ROMCC builds.
 */
unsigned uart_divisor(unsigned basefreq)
{
	unsigned div = (basefreq / CONFIG_TTYS0_BAUD);

#if !defined(__SMM__) && CONFIG_USE_OPTION_TABLE
	static const unsigned char divisor[8] = { 1, 2, 3, 6, 12, 24, 48, 96 };
	static const unsigned baud[8] =
		{ 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200 };
	unsigned b_index = 0;
#if defined(__PRE_RAM__)
	b_index = read_option(baud_rate, 0xff);
#else
	if (get_option(&b_index, "baud_rate") != CB_SUCCESS)
		b_index = 0xff;
#endif
	if (b_index < 8) {
		if (basefreq == 115200)
			div = divisor[b_index];
		else
			div = (basefreq / baud[b_index]);
	}
#endif
	return div;
}

#endif
