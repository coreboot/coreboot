/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <elog.h>
#include <reset.h>

#include "chromeos.h"
#include "symbols.h"

#define WATCHDOG_TOMBSTONE_MAGIC	0x9d2f41a7

void elog_add_watchdog_reset(void)
{
	if (readl(_watchdog_tombstone) == WATCHDOG_TOMBSTONE_MAGIC)
		elog_add_event(ELOG_TYPE_ASYNC_HW_TIMER_EXPIRED);
	writel(0, _watchdog_tombstone);
}

void reboot_from_watchdog(void)
{
	printk(BIOS_INFO, "Last reset was watchdog, reboot again to reset TPM!\n");
	writel(WATCHDOG_TOMBSTONE_MAGIC, _watchdog_tombstone);
	hard_reset();
}
