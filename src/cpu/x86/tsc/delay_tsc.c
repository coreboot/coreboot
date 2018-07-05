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

#include <arch/early_variables.h>
#include <console/console.h>
#include <arch/io.h>
#include <cpu/x86/tsc.h>
#include <smp/spinlock.h>
#include <delay.h>
#include <thread.h>

static unsigned long clocks_per_usec CAR_GLOBAL;

#define CLOCK_TICK_RATE	1193180U /* Underlying HZ */

/* ------ Calibrate the TSC -------
 * Too much 64-bit arithmetic here to do this cleanly in C, and for
 * accuracy's sake we want to keep the overhead on the CTC speaker (channel 2)
 * output busy loop as low as possible. We avoid reading the CTC registers
 * directly because of the awkward 8-bit access mechanism of the 82C54
 * device.
 */

#define CALIBRATE_INTERVAL ((2*CLOCK_TICK_RATE)/1000) /* 2ms */
#define CALIBRATE_DIVISOR  (2*1000) /* 2ms / 2000 == 1usec */

static unsigned long calibrate_tsc_with_pit(void)
{
	/* Set the Gate high, disable speaker */
	outb((inb(0x61) & ~0x02) | 0x01, 0x61);

	/*
	 * Now let's take care of CTC channel 2
	 *
	 * Set the Gate high, program CTC channel 2 for mode 0,
	 * (interrupt on terminal count mode), binary count,
	 * load 5 * LATCH count, (LSB and MSB) to begin countdown.
	 */
	outb(0xb0, 0x43);	/* binary, mode 0, LSB/MSB, Ch 2 */

	outb(CALIBRATE_INTERVAL	& 0xff, 0x42);	/* LSB of count */
	outb(CALIBRATE_INTERVAL	>> 8, 0x42);	/* MSB of count */

	{
		tsc_t start;
		tsc_t end;
		unsigned long count;

		start = rdtsc();
		count = 0;
		do {
			count++;
		} while ((inb(0x61) & 0x20) == 0);
		end = rdtsc();

		/* Error: ECTCNEVERSET */
		if (count <= 1)
			goto bad_ctc;

		/* 64-bit subtract - gcc just messes up with long longs */
		__asm__("subl %2,%0\n\t"
			"sbbl %3,%1"
			: "=a" (end.lo), "=d" (end.hi)
			: "g" (start.lo), "g" (start.hi),
			 "0" (end.lo), "1" (end.hi));

		/* Error: ECPUTOOFAST */
		if (end.hi)
			goto bad_ctc;


		/* Error: ECPUTOOSLOW */
		if (end.lo <= CALIBRATE_DIVISOR)
			goto bad_ctc;

		return CEIL_DIV(end.lo, CALIBRATE_DIVISOR);
	}

	/*
	 * The CTC wasn't reliable: we got a hit on the very first read,
	 * or the CPU was so fast/slow that the quotient wouldn't fit in
	 * 32 bits..
	 */
bad_ctc:
	printk(BIOS_ERR, "bad_ctc\n");
	return 0;
}

static unsigned long calibrate_tsc(void)
{
	if (IS_ENABLED(CONFIG_TSC_CONSTANT_RATE))
		return tsc_freq_mhz();
	else
		return calibrate_tsc_with_pit();
}

void init_timer(void)
{
	if (!car_get_var(clocks_per_usec))
		car_set_var(clocks_per_usec, calibrate_tsc());
}

static inline unsigned long get_clocks_per_usec(void)
{
	init_timer();
	return car_get_var(clocks_per_usec);
}

void udelay(unsigned int us)
{
	unsigned long long start;
	unsigned long long current;
	unsigned long long clocks;

	if (!thread_yield_microseconds(us))
		return;

	start = rdtscll();
	clocks = us;
	clocks *= get_clocks_per_usec();
	current = rdtscll();
	while ((current - start) < clocks) {
		cpu_relax();
		current = rdtscll();
	}
}

#if IS_ENABLED(CONFIG_TSC_MONOTONIC_TIMER)
#include <timer.h>

static struct monotonic_counter {
	int initialized;
	struct mono_time time;
	uint64_t last_value;
} mono_counter_g CAR_GLOBAL;

static inline struct monotonic_counter *get_monotonic_context(void)
{
	return car_get_var_ptr(&mono_counter_g);
}

void timer_monotonic_get(struct mono_time *mt)
{
	uint64_t current_tick;
	uint64_t ticks_elapsed;
	unsigned long ticks_per_usec;
	struct monotonic_counter *mono_counter;

	mono_counter = get_monotonic_context();
	if (!mono_counter->initialized) {
		init_timer();
		mono_counter->last_value = rdtscll();
		mono_counter->initialized = 1;
	}

	current_tick = rdtscll();
	ticks_elapsed = current_tick - mono_counter->last_value;
	ticks_per_usec = get_clocks_per_usec();

	/* Update current time and tick values only if a full tick occurred. */
	if (ticks_elapsed >= ticks_per_usec) {
		uint64_t usecs_elapsed;

		usecs_elapsed = ticks_elapsed / ticks_per_usec;
		mono_time_add_usecs(&mono_counter->time, (long)usecs_elapsed);
		mono_counter->last_value = current_tick;
	}

	/* Save result. */
	*mt = mono_counter->time;
}
#endif
