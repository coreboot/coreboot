/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/uart.h>
#include <soc/southbridge.h>
#include <types.h>

uintptr_t get_uart_base(unsigned int idx)
{
	if (CONFIG_UART_FOR_CONSOLE < 0 || CONFIG_UART_FOR_CONSOLE > 1)
		return 0;

	return (uintptr_t)(APU_UART0_BASE + 0x2000 * (idx & 1));
}
