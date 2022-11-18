/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <console/uart.h>

uintptr_t uart_platform_base(unsigned int idx)
{
	return (uintptr_t)0x02100000;
}
