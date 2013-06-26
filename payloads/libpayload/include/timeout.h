/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2013 secunet Security Networks AG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _LIBPAYLOAD_TIMEOUT_H
#define _LIBPAYLOAD_TIMEOUT_H

#include <libpayload.h>
#include <stdint.h>

#define NSECS_PER_SEC 1000000000L
#define USECS_PER_SEC 1000000L
#define MSECS_PER_SEC 1000

typedef const uint64_t deadline_t;

/* For a TSC, 0 won't be reached within years. How about non-x86 platforms? */
#define NEVER_TIMEOUT ((deadline_t)0)

/**
 * Starts a new timeout from now
 *
 * @param ns nanoseconds to timeout after
 */
static inline deadline_t new_timeout_ns(const unsigned long ns)
{
	return timer_raw_value() +
		DIV_ROUND_UP((uint64_t)ns * timer_hz(), NSECS_PER_SEC);
}

/**
 * Starts a new timeout from now
 *
 * @param us microseconds to timeout after
 */
static inline deadline_t new_timeout_us(const unsigned long us)
{
	return timer_raw_value() +
		DIV_ROUND_UP((uint64_t)us * timer_hz(), USECS_PER_SEC);
}

/**
 * Starts a new timeout from now
 *
 * @param ms milliseconds to timeout after
 */
static inline deadline_t new_timeout_ms(const unsigned long ms)
{
	return timer_raw_value() +
		DIV_ROUND_UP((uint64_t)ms * timer_hz(), MSECS_PER_SEC);
}

/**
 * Starts a new timeout from now
 *
 * @param s seconds to timeout after
 */
static inline deadline_t new_timeout_s(const unsigned long s)
{
	return timer_raw_value() + ((uint64_t)s * timer_hz());
}

/**
 * Checks if a deadline has expired
 *
 * @param dl the deadline to check
 * @return 1 if dl has expired, 0 if not
 */
static inline int timed_out(deadline_t dl)
{
	return timer_raw_value() >= dl;
}

int delay_until(deadline_t delay_end, deadline_t);

/**
 * Delay for a specified time but break at a deadline
 *
 * @param dl deadline to satisfy
 * @param ns number of nanoseconds to delay for
 * @return -1 if dl has already expired, 0 if not
 */
static inline int timeout_ndelay(deadline_t dl, unsigned long ns)
{
	return delay_until(new_timeout_ns(ns), dl);
}

/**
 * Delay for a specified time but break at a deadline
 *
 * @param dl deadline to satisfy
 * @param us number of microseconds to delay for
 * @return -1 if dl has already expired, 0 if not
 */
static inline int timeout_udelay(deadline_t dl, unsigned long us)
{
	return delay_until(new_timeout_us(us), dl);
}

/**
 * Delay for a specified time but break at a deadline
 *
 * @param dl deadline to satisfy
 * @param ms number of milliseconds to delay for
 * @return -1 if dl has already expired, 0 if not
 */
static inline int timeout_mdelay(deadline_t dl, const unsigned long ms)
{
	return delay_until(new_timeout_ms(ms), dl);
}

/**
 * Delay for a specified time but break at a deadline
 *
 * @param dl deadline to satisfy
 * @param s number of seconds to delay for
 * @return -1 if dl has already expired, 0 if not
 */
static inline int timeout_delay(deadline_t dl, const unsigned long s)
{
	return delay_until(new_timeout_s(s), dl);
}

#endif
