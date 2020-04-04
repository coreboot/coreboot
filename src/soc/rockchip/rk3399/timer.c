/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/mmio.h>
#include <delay.h>
#include <soc/timer.h>
#include <stdint.h>
#include <timer.h>

static uint64_t timer_raw_value(void)
{
	uint64_t value0;
	uint64_t value1;

	value0 = (uint64_t)read32(&timer0_ptr->timer_cur_value0);
	value1 = (uint64_t)read32(&timer0_ptr->timer_cur_value1);

	return value0 | value1<<32;
}

void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, timer_raw_value() / clocks_per_usec);
}

void init_timer(void)
{
	write32(&timer0_ptr->timer_load_count0, TIMER_LOAD_VAL);
	write32(&timer0_ptr->timer_load_count1, TIMER_LOAD_VAL);
	write32(&timer0_ptr->timer_load_count2, 0);
	write32(&timer0_ptr->timer_load_count3, 0);
	write32(&timer0_ptr->timer_ctrl_reg, 1);
}
