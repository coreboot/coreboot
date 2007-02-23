/*
 * serial.c -- uart init function
 *
 * Copright (C) 2007 coresystems GmbH <stepan@coresystems.de>
 * Copright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 *
 */

#include <arch/io.h>

/* Base Address */
#if defined(CONFIG_CONSOLE_SERIAL_COM1)
#define TTYSx_BASE 0x3f8
#elif defined(CONFIG_CONSOLE_SERIAL_COM2)
#define TTYSx_BASE 0x2f8
#else
#define TTYSx_BASE 0x3f8
#warning no serial port set
#endif

#if defined(CONFIG_CONSOLE_SERIAL_115200)
#define TTYSx_BAUD 115200
#elif defined(CONFIG_CONSOLE_SERIAL_57600)
#define TTYSx_BAUD 57600
#elif defined(CONFIG_CONSOLE_SERIAL_38400)
#define TTYSx_BAUD 38400
#elif defined(CONFIG_CONSOLE_SERIAL_19200)
#define TTYSx_BAUD 19200
#elif defined(CONFIG_CONSOLE_SERIAL_9600)
#define TTYSx_BAUD 9600
#else // default
#define TTYSx_BAUD 115200
#warning no serial speed set
#endif

#if ((115200%TTYSx_BAUD) != 0)
#error Bad ttys0 baud rate
#endif

#define TTYSx_DIV	(115200/TTYSx_BAUD)

/* Line Control Settings */
#ifndef TTYSx_LCS
/* Set 8bit, 1 stop bit, no parity */
#define TTYSx_LCS	0x3
#endif

#define UART_LCS	TTYSx_LCS


void uart8250_init(unsigned base_port, unsigned divisor, unsigned lcs);

void uart_init(void)
{
        unsigned ttysx_div;
#if 0
	unsigned ttysx_index;
        static const unsigned char divisor[] = { 1,2,3,6,12,24,48,96 };

	// read CMOS settings?
        ttysx_index = read_option(CMOS_VSTART_baud_rate, CMOS_VLEN_baud_rate, 0);
        ttysx_index &= 7;
        ttysx_div = divisor[ttysx_index];
#else
	ttysx_div = TTYSx_DIV;
#endif
	uart8250_init(TTYSx_BASE, ttysx_div, UART_LCS);
}

