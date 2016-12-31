/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corporation.
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
#include <cpu/x86/tsc.h>
#include <timer.h>

unsigned long tsc_freq_mhz(void)
{
	/* CPU freq = 400 MHz */
	return 400;
}

void timer_monotonic_get(struct mono_time *mt)
{
	uint64_t tsc_value;

	tsc_value = rdtscll();
	mt->microseconds = tsc_value / tsc_freq_mhz();
}
