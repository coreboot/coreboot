/*
 *
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2014, The Linux Foundation. All rights reserved.
 * Copyright 2014 Google Inc.
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
 * Foundation, Inc.
 */

#include <console/console.h>
#include <soc/iomap.h>
#include <reset.h>

/* Watchdog bite time set to default reset value */
#define RESET_WDT_BITE_TIME 0x31F3

/* Watchdog bark time value is kept larger than the watchdog timeout
 * of 0x31F3, effectively disabling the watchdog bark interrupt
 */
#define RESET_WDT_BARK_TIME (5 * RESET_WDT_BITE_TIME)

static void wdog_reset(void)
{
	printk(BIOS_DEBUG, "\nResetting with watchdog!\n");

	write32(APCS_WDT0_EN, 0);
	write32(APCS_WDT0_RST, 1);
	write32(APCS_WDT0_BARK_TIME, RESET_WDT_BARK_TIME);
	write32(APCS_WDT0_BITE_TIME, RESET_WDT_BITE_TIME);
	write32(APCS_WDT0_EN, 1);
	write32(APCS_WDT0_CPU0_WDOG_EXPIRED_ENABLE, 1);

	for (;;)
		;
}

void hard_reset(void)
{
	wdog_reset();
}
