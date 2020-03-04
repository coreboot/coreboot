/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
