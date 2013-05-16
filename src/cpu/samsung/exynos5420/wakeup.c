/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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

#include <console/console.h>
#include "power.h"
#include "wakeup.h"

void wakeup(void)
{
	if (wakeup_need_reset())
		power_reset();

	power_init();  /* Ensure ps_hold_setup() for early wakeup. */
	power_exit_wakeup();
	/* Should never return. */
	die("Failed to wake up.\n");
}

int get_wakeup_state(void)
{
	uint32_t status = power_read_reset_status();

	/* DIDLE/LPA can be resumed without clock reset (ex, bootblock),
	 * and SLEEP requires resetting clock (should be done in ROM stage).
	 */

	if (status == S5P_CHECK_DIDLE || status == S5P_CHECK_LPA)
		return WAKEUP_DIRECT;

	if (status == S5P_CHECK_SLEEP)
		return WAKEUP_NEED_CLOCK_RESET;

	return IS_NOT_WAKEUP;
}
