/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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
