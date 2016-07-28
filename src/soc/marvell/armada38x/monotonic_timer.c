/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#include <arch/io.h>
#include <timer.h>
#include <delay.h>

#define TIMER_CTRL_REG 0xf1020300
#define TIMER_RELOAD_REG 0xf1020310
#define TIMER_REG 0xf1020314
#define TIMER_RELOAD_VALUE 0xffffffff

#define MHZ_NUM 25

void init_timer(void)
{
	unsigned int reg;

	/* Set the reload timer */
	write32((void *)TIMER_RELOAD_REG, TIMER_RELOAD_VALUE);
	/* Set the initial value to TIMER_RELOAD_VALUE - 1
	 * (instead of TIMER_RELOAD_VALUE) to avoid 'fake'
	 * overflow being detected in timer_monotonic_get
	 * if it's called close enough with 'this' function */
	write32((void *)TIMER_REG, TIMER_RELOAD_VALUE - 1);
	reg = read32((const void *)TIMER_CTRL_REG);
	/* Let it start counting */
	reg |= 0x3;
	write32((void *)TIMER_CTRL_REG, reg);
}

void timer_monotonic_get(struct mono_time *mt)
{
	static uint64_t total_ticks = 0;
	uint64_t overflow = 0;
	uint32_t current_ticks =
		TIMER_RELOAD_VALUE - read32((const void *)TIMER_REG);

	/* Assuming at most one overflow happened since last call */
	if (current_ticks <= total_ticks)
		overflow = 1ULL << 32;

	total_ticks = (((total_ticks + overflow) >> 32) << 32) + current_ticks;
	mono_time_set_usecs(mt, total_ticks / MHZ_NUM);
}
