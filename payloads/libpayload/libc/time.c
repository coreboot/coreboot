/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

/**
 * @file libc/time.c
 * General time functions
 */

#include <libpayload-config.h>
#include <libpayload.h>
#ifdef CONFIG_ARCH_X86
#include <arch/rdtsc.h>
#endif

extern u32 cpu_khz;

static struct {
	u64 ticks;
	time_t secs;
	suseconds_t usecs;
} clock;

static void update_clock(void)
{
	u64 delta = timer_raw_value() - clock.ticks;
	int secs;
	static uint64_t ticks_per_sec = 0;
	static uint64_t ticks_per_usec = 0;
	if (!ticks_per_sec) {
		ticks_per_sec = timer_hz();
		ticks_per_usec = timer_hz() / 1000000;
	}

	clock.ticks += delta;

	secs = (int) (delta / ticks_per_sec);
	clock.secs += secs;
	delta -= (secs * ticks_per_sec);
	clock.usecs += (int)(delta / ticks_per_usec);

	if (clock.usecs > 1000000) {
		clock.usecs -= 1000000;
		clock.secs++;
	}
}

#ifdef CONFIG_NVRAM

static unsigned int day_of_year(int mon, int day, int year)
{
	static u8 mdays[12] = { 31, 28, 31, 30, 31, 30,
				31, 31, 30, 31, 30, 31 };

	int i, ret = 0;

	for(i = 0; i < mon; i++) {
		ret += mdays[i];

		if (i == 1 && (year % 4))
			ret++;
	}

	return (ret + day);
}

static void gettimeofday_init(void)
{
	int days, delta;
	struct tm tm;

	rtc_read_clock(&tm);
	clock.ticks = rdtsc();

	/* Calculate the number of days in the year so far */
	days = day_of_year(tm.tm_mon, tm.tm_mday, tm.tm_year + 1900);

	delta = tm.tm_year - 70;

	days += (delta * 365);

	/* Figure leap years */

	if (delta > 2)
	  days += (delta - 2) / 4;

	clock.secs = (days * 86400) + (tm.tm_hour * 3600) +
		(tm.tm_min * 60) + tm.tm_sec;
}
#else
static void gettimeofday_init(void)
{
	/* Record the number of ticks */
	clock.ticks = timer_raw_value();
}
#endif

/**
 * Return the current time broken into a timeval structure.
 *
 * @param tv A pointer to a timeval structure.
 * @param tz Added for compatability - not used.
 * @return 0 for success (this function cannot return non-zero currently).
 */
int gettimeofday(struct timeval *tv, void *tz)
{
	/*
	 * Call the gtod init when we need it - this keeps the code from
	 * being included in the binary if we don't need it.
	 */
	if (!clock.ticks)
		gettimeofday_init();

	update_clock();

	tv->tv_sec = clock.secs;
	tv->tv_usec = clock.usecs;

	return 0;
}

static inline void _delay(uint64_t delta)
{
	uint64_t start = timer_raw_value();
	while (timer_raw_value() - start < delta) ;
}

/**
 * Delay for a specified number of nanoseconds.
 *
 * @param n Number of nanoseconds to delay for.
 */
void ndelay(unsigned int n)
{
	_delay((uint64_t)n * timer_hz() / 1000000000);
}

/**
 * Delay for a specified number of microseconds.
 *
 * @param n Number of microseconds to delay for.
 */
void udelay(unsigned int n)
{
	_delay((uint64_t)n * timer_hz() / 1000000);
}

/**
 * Delay for a specified number of milliseconds.
 *
 * @param m Number of milliseconds to delay for.
 */
void mdelay(unsigned int m)
{
	_delay((uint64_t)m * timer_hz() / 1000);
}

/**
 * Delay for a specified number of seconds.
 *
 * @param s Number of seconds to delay for.
 */
void delay(unsigned int s)
{
	_delay((uint64_t)s * timer_hz());
}
