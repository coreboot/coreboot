/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <console/uart.h>
#include <mainboard/addressmap.h>

uintptr_t uart_platform_base(unsigned int idx)
{
	return (uintptr_t)QEMU_VIRT_UART0;
}
