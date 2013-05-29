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

void bootblock_mainboard_init(void);
void bootblock_mainboard_init(void)
{
	/* Enable the GPIO module */
	writel((0x2 << 0) | (1 << 18), (uint32_t *)(0x44e00000 + 0xac));

	/* Disable interrupts from these GPIOs */
	setbits_le32((uint32_t *)(0x4804c000 + 0x3c), 0xf << 21);

	/* Enable output */
	clrbits_le32((uint32_t *)(0x4804c000 + 0x134), 0xf << 21);

	/* Set every other light */
	clrbits_le32((uint32_t *)(0x4804c000 + 0x13c), 0xf << 21);
	setbits_le32((uint32_t *)(0x4804c000 + 0x13c), 0x5 << 21);

	/* Start monotonic timer */
	//rtc_start();

	console_init();
}
