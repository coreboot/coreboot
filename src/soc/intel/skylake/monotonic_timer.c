/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <stdint.h>
#include <cpu/x86/msr.h>
#include <timer.h>
#include <soc/msr.h>

static inline uint32_t read_counter_msr(void)
{
	/*
	 * Even though the MSR is 64-bit it is assumed that the hardware
	 * is polled frequently enough to only use the lower 32-bits.
	 */
	msr_t counter_msr;

	counter_msr = rdmsr(MSR_COUNTER_24_MHZ);

	return counter_msr.lo;
}

void timer_monotonic_get(struct mono_time *mt)
{
	/* Always increases. Don't normalize to 0 between stages. */
	mono_time_set_usecs(mt, read_counter_msr() / 24);
}
