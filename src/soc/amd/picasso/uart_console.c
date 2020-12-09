/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/uart.h>
#include <soc/uart.h>

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
	return CONFIG(AMD_SOC_UART_48MZ) ? 48000000 : 115200 * 16;
}
