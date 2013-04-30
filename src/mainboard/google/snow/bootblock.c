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
#include <cpu/samsung/exynos5250/periph.h>
#include <cpu/samsung/exynos5250/pinmux.h>
#include "mainboard.h"

void bootblock_mainboard_init(void);
void bootblock_mainboard_init(void)
{
	switch (snow_get_wakeup_state()) {
		case SNOW_WAKEUP_DIRECT:
			snow_wakeup();
			break;

		case SNOW_IS_NOT_WAKEUP:
			/* kick off the microsecond timer.
			 * We want to do this as early as we can.
			 */
			timer_start();
			break;
	}

	/* For most ARM systems, we have to initialize firmware media source
	 * (ex, SPI, SD/MMC, or eMMC) now; but for Exynos platform, that is
	 * already handled by iROM so there's no need to setup again.
	 */

	console_init();
}
