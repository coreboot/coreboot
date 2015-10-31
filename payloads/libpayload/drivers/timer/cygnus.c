/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Electronics
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
#include <libpayload.h>

struct cygnus_timer {
	u32 gtim_glob_low;
	u32 gtim_glob_hi;
	u32 gtim_glob_ctrl;
};

static struct cygnus_timer * const timer_ptr =
		(void *)CONFIG_LP_IPROC_PERIPH_GLB_TIM_REG_BASE;

uint64_t timer_hz(void)
{
	/*
	 * this is set up by coreboot as follows:
	 *
	 * PERIPH_CLOCK /
	 * (((TIMER_GLB_TIM_CTRL_PRESC & TIMER_GLB_TIM_CTRL_PRESC_MASK)>>8) + 1)
	 *
	 * where PERIPH_CLOCK is typically 500000000.
	 */
	return 500000000;
}

uint64_t timer_raw_value(void)
{
	uint64_t cur_tick;
	uint32_t count_h;
	uint32_t count_l;

	do {
		count_h = readl(&timer_ptr->gtim_glob_hi);
		count_l = readl(&timer_ptr->gtim_glob_low);
		cur_tick = readl(&timer_ptr->gtim_glob_hi);
	} while (cur_tick != count_h);

	return (cur_tick << 32) + count_l;
}
