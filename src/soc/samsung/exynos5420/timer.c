/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
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
#include <delay.h>
#include <soc/clk.h>
#include <stdint.h>
#include <timer.h>

static const uint32_t clocks_per_usec = MCT_HZ/1000000;

static uint64_t mct_raw_value(void)
{
	uint64_t upper = read32(&exynos_mct->g_cnt_u);
	uint64_t lower = read32(&exynos_mct->g_cnt_l);

	return (upper << 32) | lower;
}

void init_timer(void)
{
	write32(&exynos_mct->g_tcon, read32(&exynos_mct->g_tcon) | (0x1 << 8));
}

void timer_monotonic_get(struct mono_time *mt)
{
	/* We don't have to call mct_start() here
	 * because it was already called in the bootblock
	 */

	mono_time_set_usecs(mt, mct_raw_value() / clocks_per_usec);
}
