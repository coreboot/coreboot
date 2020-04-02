/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <types.h>
#include <console/uart.h>

uintptr_t uart_platform_base(int idx)
{
	return (uintptr_t) 0x02100000;
}
