/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/uart.h>
#include <console/uart.h>

/*
 * uart_platform_base and uart_platform_refclk are used by the console UART driver and need to
 * be provided exactly once and only by the UART that is used for console.
 */

uintptr_t uart_platform_base(unsigned int idx)
{
	return get_uart_base(idx);
}

unsigned int uart_platform_refclk(void)
{
	return 48000000;
}
