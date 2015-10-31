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
 */

#include <console/console.h>
#include <console/uart.h>
#if CONFIG_USE_OPTION_TABLE
#include <option.h>
#include "option_table.h"
#endif

unsigned int default_baudrate(void)
{
#if !defined(__SMM__) && CONFIG_USE_OPTION_TABLE
	static const unsigned baud[8] =
		{ 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200 };
	unsigned b_index = 0;
#if defined(__PRE_RAM__)
	b_index = read_option(baud_rate, 0xff);
#else
	if (get_option(&b_index, "baud_rate") != CB_SUCCESS)
		b_index = 0xff;
#endif
	if (b_index < 8)
		return baud[b_index];
#endif
	return CONFIG_TTYS0_BAUD;
}

/* Calculate divisor. Do not floor but round to nearest integer. */
unsigned int uart_baudrate_divisor(unsigned int baudrate,
	unsigned int refclk, unsigned int oversample)
{
	return (1 + (2 * refclk) / (baudrate * oversample)) / 2;
}
