/*
 * This file is part of the coreboot project.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/uart.h>

#define VEXPRESS_UART0_IO_ADDRESS      (0x10009000)

uintptr_t uart_platform_base(int idx)
{
	return VEXPRESS_UART0_IO_ADDRESS;
}
