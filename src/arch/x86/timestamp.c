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
 */

#include <compiler.h>
#include <cpu/x86/tsc.h>
#include <timestamp.h>

uint64_t timestamp_get(void)
{
	return rdtscll();
}

unsigned long __weak tsc_freq_mhz(void)
{
	/* Default to not knowing TSC frequency. cbmem will have to fallback
	 * on trying to determine it in userspace. */
	return 0;
}

int timestamp_tick_freq_mhz(void)
{
	/* Chipsets that have a constant TSC provide this value correctly. */
	return tsc_freq_mhz();
}
