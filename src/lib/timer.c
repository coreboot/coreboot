/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <timer.h>
#include <delay.h>
#include <thread.h>

__weak void init_timer(void) { /* do nothing */ }

void udelay(unsigned int usec)
{
	struct stopwatch sw;

	/*
	 * As the timer granularity is in microseconds pad the
	 * requested delay by one to get at least >= requested usec delay.
	 */
	usec += 1;

	if (!thread_yield_microseconds(usec))
		return;

	stopwatch_init_usecs_expire(&sw, usec);
	stopwatch_wait_until_expired(&sw);
}
