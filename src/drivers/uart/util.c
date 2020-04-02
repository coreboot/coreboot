/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/uart.h>
#include <types.h>
#include <timer.h>

/* Calculate divisor. Do not floor but round to nearest integer. */
unsigned int uart_baudrate_divisor(unsigned int baudrate,
	unsigned int refclk, unsigned int oversample)
{
	return (1 + (2 * refclk) / (baudrate * oversample)) / 2;
}

#if !CONFIG(UART_OVERRIDE_INPUT_CLOCK_DIVIDER)
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

#if !CONFIG(UART_OVERRIDE_REFCLK)
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

/* Helper function to allow bitbanging an 8n1 UART. */
void uart_bitbang_tx_byte(unsigned char data, void (*set_tx)(int line_state))
{
	const int baud_rate = get_uart_baudrate();
	int i;
	struct stopwatch sw;
	stopwatch_init(&sw);

	/* Send start bit */
	set_tx(0);
	while (stopwatch_duration_usecs(&sw) < MHz / baud_rate)
		stopwatch_tick(&sw);

	/* 'i' counts the total bits sent at the end of the loop */
	for (i = 2; i < 10; i++) {
		set_tx(data & 1);
		data >>= 1;
		while (stopwatch_duration_usecs(&sw) < i * MHz / baud_rate)
			stopwatch_tick(&sw);
	}

	/* Send stop bit */
	set_tx(1);
	while (stopwatch_duration_usecs(&sw) < i * MHz / baud_rate)
		stopwatch_tick(&sw);
}
