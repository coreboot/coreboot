/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/lib_helpers.h>
#include <commonlib/bsd/gcd.h>
#include <timer.h>

void timer_monotonic_get(struct mono_time *mt)
{
	uint64_t tvalue = raw_read_cntpct_el0();
	static uint32_t tfreq, mult;
	uint32_t div;

	/*
	 * The value from raw_read_cntfrq_el0() could be large enough to
	 * cause overflow when multiplied by USECS_PER_SEC. To prevent this,
	 * both USECS_PER_SEC. and tfreq can be reduced by dividing them by
	 * their GCD.
	 */
	if (tfreq == 0) {
		tfreq = raw_read_cntfrq_el0();
		mult = USECS_PER_SEC;
		div = gcd(tfreq, mult);
		tfreq /= div;
		mult /= div;
	}

	long usecs = (tvalue * mult) / tfreq;
	mono_time_set_usecs(mt, usecs);
}
