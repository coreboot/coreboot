/*
 * This file is part of the coreboot project.
 *
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

#include <console/uart.h>

uintptr_t uart_platform_base(int idx)
{
	return (uintptr_t)(0xFEDC6000 + 0x2000 * (idx & 1));
}

unsigned int uart_platform_refclk(void)
{
	return 48000000;
}
