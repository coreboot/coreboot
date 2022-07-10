/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cpu/x86/tsc.h>
#include <timer.h>
#include <timestamp.h>

void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, timestamp_get());
}

uint64_t timestamp_get(void)
{
	return rdtscll() / tsc_freq_mhz();
}
