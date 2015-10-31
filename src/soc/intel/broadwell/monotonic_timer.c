/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#include <stdint.h>
#include <cpu/x86/msr.h>
#include <timer.h>
#include <soc/msr.h>

static struct monotonic_counter {
	int initialized;
	struct mono_time time;
	uint32_t last_value;
} mono_counter;

static inline uint32_t read_counter_msr(void)
{
	/* Even though the MSR is 64-bit it is assumed that the hardware
	 * is polled frequently enough to only use the lower 32-bits. */
	msr_t counter_msr;

	counter_msr = rdmsr(MSR_COUNTER_24_MHZ);

	return counter_msr.lo;
}

void timer_monotonic_get(struct mono_time *mt)
{
	uint32_t current_tick;
	uint32_t usecs_elapsed;

	if (!mono_counter.initialized) {
		mono_counter.last_value = read_counter_msr();
		mono_counter.initialized = 1;
	}

	current_tick = read_counter_msr();
	usecs_elapsed = (current_tick - mono_counter.last_value) / 24;

	/* Update current time and tick values only if a full tick occurred. */
	if (usecs_elapsed) {
		mono_time_add_usecs(&mono_counter.time, usecs_elapsed);
		mono_counter.last_value = current_tick;
	}

	/* Save result. */
	*mt = mono_counter.time;
}
