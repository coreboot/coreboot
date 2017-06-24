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
#if IS_ENABLED(CONFIG_USE_OPTION_TABLE)
#include <option.h>
#include "option_table.h"
#endif

unsigned int default_baudrate(void)
{
#if !defined(__SMM__) && IS_ENABLED(CONFIG_USE_OPTION_TABLE)
	static const unsigned baud[8] =
		{ 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200 };
	unsigned b_index = 0;
#if defined(__ROMCC__)
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

#if !IS_ENABLED(CONFIG_UART_OVERRIDE_INPUT_CLOCK_DIVIDER)
unsigned int uart_input_clock_divider(void)
{
	/* Specify the default oversample rate for the UART.
	 *
	 * UARTs oversample the receive data.  The UART's input clock first
	 * enters the baud-rate divider to generate the oversample clock.  Then
	 * the UART typically divides the result by 16.  The asynchronous
	 * receive data is synchronized with the oversample clock and when a
	 * start bit is detected the UART delays half a bit time using the
	 * oversample clock.  Samples are then taken to verify the start bit and
	 * if present, samples are taken for the rest of the frame.
	 */
	return 16;
}
#endif

#if !IS_ENABLED(CONFIG_UART_OVERRIDE_REFCLK)
unsigned int uart_platform_refclk(void)
{
	/* Specify the default input clock frequency for the UART.
	 *
	 * The older UART's used an input clock frequency of 1.8432 MHz which
	 * with the 16x oversampling provided the maximum baud-rate of 115200.
	 * Specify this as maximum baud-rate multiplied by oversample so that
	 * it is obvious that the maximum baud rate is 115200 when divided by
	 * oversample clock.  Also note that crystal on the board does not
	 * change when software selects another input clock divider.
	 */
	return 115200 * 16;
}
#endif
