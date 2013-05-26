/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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

#include <stdint.h>
#include <delay.h>
#include <timer.h>

#include "dmtimer.h"

static struct monotonic_counter {
	int initialized;
	struct mono_time time;
	uint64_t last_value;
} mono_counter;

static const uint32_t clocks_per_usec = OSC_HZ/1000000;

void timer_monotonic_get(struct mono_time *mt)
{
	uint64_t current_tick;
	uint64_t usecs_elapsed;

	if (!mono_counter.initialized) {
		init_timer();
		mono_counter.last_value = dmtimer_raw_value(0);
		mono_counter.initialized = 1;
	}

	current_tick = dmtimer_raw_value(0);
	usecs_elapsed = (current_tick - mono_counter.last_value) /
							clocks_per_usec;

	/* Update current time and tick values only if a full tick occurred. */
	if (usecs_elapsed) {
		mono_time_add_usecs(&mono_counter.time, usecs_elapsed);
		mono_counter.last_value = current_tick;
	}

	/* Save result. */
	*mt = mono_counter.time;
}
