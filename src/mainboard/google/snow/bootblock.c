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

#include <types.h>
#include <arch/io.h>
#include <device/i2c.h>
#include <cpu/samsung/exynos5250/clk.h>
#include <cpu/samsung/exynos5250/dmc.h>
#include <cpu/samsung/exynos5250/periph.h>
#include <cpu/samsung/exynos5250/clock_init.h>
#include <src/cpu/samsung/exynos5250/power.h>
#include <drivers/maxim/max77686/max77686.h>
#include <console/console.h>

#define I2C0_BASE	0x12c60000

void bootblock_mainboard_init(void);
void bootblock_mainboard_init(void)
{
	struct mem_timings *mem;
	struct arm_clk_ratios *arm_ratios;

	mem = get_mem_timings();
	arm_ratios = get_arm_clk_ratios();
	system_clock_init(mem, arm_ratios);
	exynos_pinmux_config(PERIPH_ID_UART3, PINMUX_FLAG_NONE);

	console_init();
	printk(BIOS_INFO, "\n\n\n%s: UART initialized\n", __func__);
}
