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
 */

#include <arch/io.h>
#include <types.h>
#include <bootblock_common.h>
#include <console/uart.h>
#include <console/console.h>
#include <cpu/ti/am335x/pinmux.h>

void bootblock_mainboard_init(void)
{
	void *uart_clock_ctrl = NULL;

	/* Enable the GPIO module */
	write32((uint32_t *)(0x44e00000 + 0xac), (0x2 << 0) | (1 << 18));

	/* Disable interrupts from these GPIOs */
	setbits_le32((uint32_t *)(0x4804c000 + 0x3c), 0xf << 21);

	/* Enable output */
	clrbits_le32((uint32_t *)(0x4804c000 + 0x134), 0xf << 21);

	/* Set every other light */
	clrbits_le32((uint32_t *)(0x4804c000 + 0x13c), 0xf << 21);
	setbits_le32((uint32_t *)(0x4804c000 + 0x13c), 0x5 << 21);

	/* Set up the UART we're going to use */
	if (CONFIG_UART_FOR_CONSOLE == 0) {
		am335x_pinmux_uart0();
		uart_clock_ctrl = (void *)(uintptr_t)(0x44e00400 + 0xb4);
	} else if (CONFIG_UART_FOR_CONSOLE == 1) {
		am335x_pinmux_uart1();
		uart_clock_ctrl = (void *)(uintptr_t)(0x44e00000 + 0x6c);
	} else if (CONFIG_UART_FOR_CONSOLE == 2) {
		am335x_pinmux_uart2();
		uart_clock_ctrl = (void *)(uintptr_t)(0x44e00000 + 0x70);
	} else if (CONFIG_UART_FOR_CONSOLE == 3) {
		am335x_pinmux_uart3();
		uart_clock_ctrl = (void *)(uintptr_t)(0x44e00000 + 0x74);
	} else if (CONFIG_UART_FOR_CONSOLE == 4) {
		am335x_pinmux_uart4();
		uart_clock_ctrl = (void *)(uintptr_t)(0x44e00000 + 0x78);
	} else if (CONFIG_UART_FOR_CONSOLE == 5) {
		am335x_pinmux_uart5();
		uart_clock_ctrl = (void *)(uintptr_t)(0x44e00000 + 0x38);
	}
	if (uart_clock_ctrl)
		write32(uart_clock_ctrl, 0x2);

	/* Start monotonic timer */
	//rtc_start();
}
