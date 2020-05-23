/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/uart.h>
#include <gpio.h>

#define UART_TX_PIN GPIO(4)

static void set_tx(int line_state)
{
	gpio_set(UART_TX_PIN, line_state);
}

void uart_init(int idx)
{
	gpio_output(UART_TX_PIN, 1);
}

void uart_tx_byte(int idx, unsigned char data)
{
	uart_bitbang_tx_byte(data, set_tx);
}

void uart_tx_flush(int idx)
{
	/* unnecessary, PIO Tx means transaction is over when tx_byte returns */
}

unsigned char uart_rx_byte(int idx)
{
	return 0;	/* not implemented */
}
