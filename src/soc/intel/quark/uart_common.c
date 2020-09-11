/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/uart.h>
#include <soc/iomap.h>

unsigned int uart_platform_refclk(void)
{
	return 44236800;
}

uintptr_t uart_platform_base(unsigned int idx)
{
	return UART_BASE_ADDRESS;
}
