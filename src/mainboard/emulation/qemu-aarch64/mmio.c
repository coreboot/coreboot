/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/uart.h>
#include <mainboard/addressmap.h>

uintptr_t uart_platform_base(unsigned int idx)
{
	return VIRT_UART_BASE;
}
