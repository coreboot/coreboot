/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <timer.h>
#include <arch/lib_helpers.h>

void timer_monotonic_get(struct mono_time *mt)
{
	uint64_t tvalue = raw_read_cntpct_el0();
	uint32_t tfreq  = raw_read_cntfrq_el0();
	long usecs = (tvalue * 1000000) / tfreq;
	mono_time_set_usecs(mt, usecs);
}
