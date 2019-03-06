/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2009 coresystems GmbH
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

#include <stdint.h>
#include <console/console.h>
#include <delay.h>
#include <thread.h>
#include <arch/cpu.h>
#include <arch/early_variables.h>
#include <cpu/intel/fsb.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/speedstep.h>

/* NOTE: This code uses global variables, so it can not be used during
 * memory init.
 */

#if CONFIG_UDELAY_LAPIC_FIXED_FSB != 0
static inline u32 get_timer_fsb(void)
{
	return CONFIG_UDELAY_LAPIC_FIXED_FSB;
}

static int set_timer_fsb(void)
{
	return 0;
}
#else
static u32 g_timer_fsb CAR_GLOBAL;

static int set_timer_fsb(void)
{
	int ia32_fsb = get_ia32_fsb();

	if (ia32_fsb > 0) {
		car_set_var(g_timer_fsb, ia32_fsb);
		return 0;
	}
	return -1;
}

static inline u32 get_timer_fsb(void)
{
	return car_get_var(g_timer_fsb);
}
#endif

void init_timer(void)
{
	/* Set the APIC timer to no interrupts and periodic mode */
	lapic_write(LAPIC_LVTT, (LAPIC_LVT_TIMER_PERIODIC | LAPIC_LVT_MASKED));

	/* Set the divider to 1, no divider */
	lapic_write(LAPIC_TDCR, LAPIC_TDR_DIV_1);

	/* Set the initial counter to 0xffffffff */
	lapic_write(LAPIC_TMICT, 0xffffffff);

	/* Set FSB frequency to a reasonable value */
	set_timer_fsb();
}

void udelay(u32 usecs)
{
	u32 start, value, ticks, timer_fsb;

	if (!thread_yield_microseconds(usecs))
		return;

	timer_fsb = get_timer_fsb();
	if (!timer_fsb || (lapic_read(LAPIC_LVTT) &
		(LAPIC_LVT_TIMER_PERIODIC | LAPIC_LVT_MASKED)) !=
		(LAPIC_LVT_TIMER_PERIODIC | LAPIC_LVT_MASKED)) {
		init_timer();
		timer_fsb = get_timer_fsb();
	}

	/* Calculate the number of ticks to run, our FSB runs at timer_fsb Mhz
	 */
	ticks = usecs * timer_fsb;
	start = lapic_read(LAPIC_TMCCT);
	do {
		value = lapic_read(LAPIC_TMCCT);
	} while ((start - value) < ticks);
}

#if CONFIG(LAPIC_MONOTONIC_TIMER)
#include <timer.h>

static struct monotonic_counter {
	int initialized;
	struct mono_time time;
	uint32_t last_value;
} mono_counter_g CAR_GLOBAL;

void timer_monotonic_get(struct mono_time *mt)
{
	uint32_t current_tick;
	uint32_t usecs_elapsed;
	uint32_t timer_fsb;
	struct monotonic_counter *mono_counter;

	mono_counter = car_get_var_ptr(&mono_counter_g);

	if (!mono_counter->initialized) {
		init_timer();
		timer_fsb = get_timer_fsb();
		/* An FSB frequency of 200Mhz provides a 20 second polling
		 * interval between timer_monotonic_get() calls before wrap
		 * around occurs. */
		if (timer_fsb > 200)
			printk(BIOS_WARNING,
			       "apic timer freq (%d) may be too fast.\n",
			       timer_fsb);
		mono_counter->last_value = lapic_read(LAPIC_TMCCT);
		mono_counter->initialized = 1;
	}

	timer_fsb = get_timer_fsb();
	current_tick = lapic_read(LAPIC_TMCCT);
	/* Note that the APIC timer counts down. */
	usecs_elapsed = (mono_counter->last_value - current_tick) / timer_fsb;

	/* Update current time and tick values only if a full tick occurred. */
	if (usecs_elapsed) {
		mono_time_add_usecs(&mono_counter->time, usecs_elapsed);
		mono_counter->last_value = current_tick;
	}

	/* Save result. */
	*mt = mono_counter->time;
}
#endif
