/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#include <assert.h>
#include <libpayload.h>

static uint32_t *low_reg = (void *)CONFIG_LP_TIMER_GENERIC_REG;
static uint32_t *high_reg = (void *)CONFIG_LP_TIMER_GENERIC_HIGH_REG;

uint64_t timer_hz(void)
{
	/* libc/time.c currently requires all timers to be at least 1MHz. */
	assert(CONFIG_LP_TIMER_GENERIC_HZ >= 1000000);
	return CONFIG_LP_TIMER_GENERIC_HZ;
}

uint64_t timer_raw_value(void)
{
	uint64_t cur_tick;
	uint32_t count_h;
	uint32_t count_l;

	if (!high_reg)
		return readl(low_reg);

	do {
		count_h = readl(high_reg);
		count_l = readl(low_reg);
		cur_tick = readl(high_reg);
	} while (cur_tick != count_h);

	return (cur_tick << 32) + count_l;
}
