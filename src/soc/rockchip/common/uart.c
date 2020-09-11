/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/uart.h>
#include <soc/clock.h>
#include <stdint.h>

unsigned int uart_platform_refclk(void)
{
	return OSC_HZ;
}

uintptr_t uart_platform_base(unsigned int idx)
{
	return CONFIG_CONSOLE_SERIAL_UART_ADDRESS;
}
