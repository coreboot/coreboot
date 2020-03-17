/*
 * This file is part of the coreboot project.
 *
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

#include <console/uart.h>
#include <soc/southbridge.h>

uintptr_t uart_platform_base(int idx)
{
	if (CONFIG_UART_FOR_CONSOLE < 0 || CONFIG_UART_FOR_CONSOLE > 1)
		return 0;

	return (uintptr_t)(APU_UART0_BASE + 0x2000 * (idx & 1));
}

unsigned int uart_platform_refclk(void)
{
	return 48000000;
}
