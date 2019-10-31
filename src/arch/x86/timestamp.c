/*
 * This file is part of the coreboot project.
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

#include <cpu/x86/tsc.h>
#include <timestamp.h>

uint64_t timestamp_get(void)
{
	return rdtscll();
}

int timestamp_tick_freq_mhz(void)
{
	/* Chipsets that have a constant TSC provide this value correctly. */
	if (tsc_constant_rate())
		return tsc_freq_mhz();

	/* Filling tick_freq_mhz = 0 in timestamps-table will trigger
	 * userspace utility to try deduce it from the running system.
	 */
	return 0;
}
