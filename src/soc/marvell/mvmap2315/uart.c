/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <assert.h>
#include <console/uart.h>
#include <rules.h>
#include <stdint.h>
#include <soc/uart.h>

#if	ENV_BOOTBLOCK
u32 uart_num;
#endif

unsigned int uart_platform_refclk(void)
{
	return 160690;
}

uintptr_t uart_platform_base(int idx)
{
	/* Default to UART 0 */
	u32  base = CONFIG_CONSOLE_SERIAL_MVMAP2315_UART_ADDRESS;

#if	ENV_BOOTBLOCK
	assert((uart_num >= 0) && (uart_num <= 1));
	base += uart_num * 0x1000;
#else
	assert((idx >= 0) && (idx <= 1));
	base += idx * 0x1000;
#endif

	return base;
}
