/*
 * serial.c -- uart init function
 *
 * Copyright (C) 2007 coresystems GmbH
 * Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH.
 *
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
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
 */

#include <arch/io.h>
#include <uart8250.h>

void uart_init(void)
{
	unsigned ttysx_div;
#if 0
	unsigned ttysx_index;
	static const unsigned char divisor[] = { 1, 2, 3, 6, 12, 24, 48, 96 };

	// read CMOS settings?
	ttysx_index = read_option(CMOS_VSTART_baud_rate, CMOS_VLEN_baud_rate, 0);
	ttysx_index &= 7;
	ttysx_div = divisor[ttysx_index];
#else
	ttysx_div = TTYSx_DIV;
#endif
	uart8250_init(TTYSx_BASE, ttysx_div, UART_LCS);
}
