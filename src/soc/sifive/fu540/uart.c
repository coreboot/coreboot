/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Jonathan Neuschäfer
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
#include <console/uart.h>
#include <soc/addressmap.h>
#include <soc/clock.h>

uintptr_t uart_platform_base(int idx)
{
	if (idx < 2)
		return FU540_UART(idx);
	else
		return 0;
}

unsigned int uart_platform_refclk(void)
{
	/*
	 * The SiFive UART uses tlclk, which is coreclk/2 as input
	 */

	if (ENV_BOOTBLOCK)
		return 33330000 / 2;
	else
		return clock_get_coreclk_khz() * KHz / 2;
}
