/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#include <types.h>
#include <console/uart.h>
#include <arch/io.h>
#include <boot/coreboot_tables.h>
#include <spike_util.h>

uintptr_t uart_platform_base(int idx)
{
	return (uintptr_t) 0x42000000;
}

/* these are currently not quite right but they are here for reference
 * and will be fixed when lowrisc gives us a standard clock
 * and set of values. */
// divisor = clk_freq / (16 * Baud)
unsigned int uart_input_clock_divider(void)
{
	return (25 * 1000 * 1000u / (16u * 115200u)) % 0x100;
}

// System clock 25 MHz, 115200 baud rate
unsigned int uart_platform_refclk(void)
{
	return (25 * 1000 * 1000u / (16u * 115200u)) >> 8;
}
