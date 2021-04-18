/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/uart.h>
#include <types.h>

uintptr_t uart_platform_base(unsigned int idx)
{
	if (idx == CONFIG_UART_FOR_CONSOLE)
		return CONFIG_CONSOLE_UART_BASE_ADDRESS;

	return 0;
}
