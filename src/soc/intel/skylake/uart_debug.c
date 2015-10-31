/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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
#include <console/uart.h>
#include <soc/iomap.h>
#include <soc/serialio.h>

unsigned int uart_platform_refclk(void)
{
	/*
	 * Set M and N divisor inputs and enable clock.
	 * Main reference frequency to UART is:
	 *  120MHz * M / N = 120MHz * 48 / 3125 = 1843200Hz
	 * The different order below is to handle integer math overflow.
	 */
	return 120 * MHz / SIO_REG_PPR_CLOCK_N_DIV * SIO_REG_PPR_CLOCK_M_DIV;
}

uintptr_t uart_platform_base(int idx)
{
	/* Same base address for all debug port usage. In reality UART2
	 * is currently only supported. */
	return UART_DEBUG_BASE_ADDRESS;
}
