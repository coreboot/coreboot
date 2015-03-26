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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <timer.h>

#define TIMER_GLB_TIM_CTRL_PRESC_MASK	0x0000FF00
#define TIMER_GLB_TIM_CTRL_TIM_EN	0x00000001
#define TIMER_GLB_TIM_CTRL_PRESC	0x0
/*
 * arm clk is 1GHz, periph_clk=arm_clk/2, tick per usec.
 * arm clk is set by the bootrom. See util/broadcom/unauth.cfg for details.
 */
#define PERIPH_CLOCK			500
#define CLOCKS_PER_USEC	(PERIPH_CLOCK / \
	(((TIMER_GLB_TIM_CTRL_PRESC & TIMER_GLB_TIM_CTRL_PRESC_MASK) >> 8) + 1))

struct cygnus_timer {
	u32 gtim_glob_low;
	u32 gtim_glob_hi;
	u32 gtim_glob_ctrl;
};

static struct cygnus_timer * const timer_ptr =
		(void *)IPROC_PERIPH_GLB_TIM_REG_BASE;

static inline uint64_t timer_raw_value(void)
{
	uint64_t cur_tick;
	uint32_t count_h;
	uint32_t count_l;

	do {
		count_h = read32(&timer_ptr->gtim_glob_hi);
		count_l = read32(&timer_ptr->gtim_glob_low);
		cur_tick = read32(&timer_ptr->gtim_glob_hi);
	} while (cur_tick != count_h);

	return (cur_tick << 32) + count_l;
}

void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, timer_raw_value() / CLOCKS_PER_USEC);
}

void init_timer(void)
{
	write32(&timer_ptr->gtim_glob_ctrl, TIMER_GLB_TIM_CTRL_PRESC);
	write32(&timer_ptr->gtim_glob_low, 0);
	write32(&timer_ptr->gtim_glob_hi, 0);
	write32(&timer_ptr->gtim_glob_ctrl, TIMER_GLB_TIM_CTRL_TIM_EN);
}
