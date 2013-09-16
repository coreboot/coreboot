/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <types.h>
#include <uart.h>
#include <console/console.h>
#include <cpu/ti/am335x/pinmux.h>

void bootblock_mainboard_init(void);
void bootblock_mainboard_init(void)
{
	void *uart_clock_ctrl = NULL;

	/* Enable the GPIO module */
	writel((0x2 << 0) | (1 << 18), (uint32_t *)(0x44e00000 + 0xac));

	/* Disable interrupts from these GPIOs */
	setbits_le32((uint32_t *)(0x4804c000 + 0x3c), 0xf << 21);

	/* Enable output */
	clrbits_le32((uint32_t *)(0x4804c000 + 0x134), 0xf << 21);

	/* Set every other light */
	clrbits_le32((uint32_t *)(0x4804c000 + 0x13c), 0xf << 21);
	setbits_le32((uint32_t *)(0x4804c000 + 0x13c), 0x5 << 21);

	/* Set up the UART we're going to use */
	if (CONFIG_CONSOLE_SERIAL_UART0) {
		am335x_pinmux_uart0();
		uart_clock_ctrl = (void *)(uintptr_t)(0x44e00400 + 0xb4);
	} else if (CONFIG_CONSOLE_SERIAL_UART1) {
		am335x_pinmux_uart1();
		uart_clock_ctrl = (void *)(uintptr_t)(0x44e00000 + 0x6c);
	} else if (CONFIG_CONSOLE_SERIAL_UART2) {
		am335x_pinmux_uart2();
		uart_clock_ctrl = (void *)(uintptr_t)(0x44e00000 + 0x70);
	} else if (CONFIG_CONSOLE_SERIAL_UART3) {
		am335x_pinmux_uart3();
		uart_clock_ctrl = (void *)(uintptr_t)(0x44e00000 + 0x74);
	} else if (CONFIG_CONSOLE_SERIAL_UART4) {
		am335x_pinmux_uart4();
		uart_clock_ctrl = (void *)(uintptr_t)(0x44e00000 + 0x78);
	} else if (CONFIG_CONSOLE_SERIAL_UART5) {
		am335x_pinmux_uart5();
		uart_clock_ctrl = (void *)(uintptr_t)(0x44e00000 + 0x38);
	}
	if (uart_clock_ctrl)
		writel(0x2, uart_clock_ctrl);

	/* Start monotonic timer */
	//rtc_start();
}
