/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/uart.h>

unsigned int get_uart_baudrate(void)
{
	/* SOL console baud rate. */
	return 57600;
}
