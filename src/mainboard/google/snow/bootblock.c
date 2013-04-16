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
#include <cbfs.h>
#include <uart.h>
#include <time.h>
#include <console/console.h>
#include <cpu/samsung/exynos5-common/exynos5-common.h>
#include <cpu/samsung/exynos5250/periph.h>
#include <cpu/samsung/exynos5250/pinmux.h>
#include <cpu/samsung/exynos5250/power.h>
#include "mainboard.h"

void bootblock_mainboard_init(void);
void bootblock_mainboard_init(void)
{
	int reset_status, is_resume = 0;

	reset_status = power_read_reset_status();
	switch (reset_status) {
		case S5P_CHECK_DIDLE:
		case S5P_CHECK_LPA:
			wakeup_system();
			/* Never returns. */
			break;

		case S5P_CHECK_SLEEP:
			is_resume = 1;
			break;

		default:
			/* Kick off the microsecond timer on normal boot.
			 * We want to do this as early as we can.
			 */
			timer_start();
			break;
	}

#if CONFIG_EARLY_CONSOLE
	if (!is_resume) {
		exynos_pinmux_config(PERIPH_ID_UART3, PINMUX_FLAG_NONE);
		console_init();
		printk(BIOS_INFO, "\n\n\n%s: UART initialized\n", __func__);
	}
#endif
	exynos_pinmux_config(PERIPH_ID_SPI1, PINMUX_FLAG_NONE);
}
