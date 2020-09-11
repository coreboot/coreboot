/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/uart.h>
#include <soc/southbridge.h>

uintptr_t uart_platform_base(unsigned int idx)
{
	if (CONFIG_UART_FOR_CONSOLE < 0 || CONFIG_UART_FOR_CONSOLE > 1)
		return 0;

	return (uintptr_t)(APU_UART0_BASE + 0x2000 * (idx & 1));
}

unsigned int uart_platform_refclk(void)
{
	return 48000000;
}
