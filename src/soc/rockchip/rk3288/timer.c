/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <delay.h>
#include <soc/timer.h>
#include <stdint.h>
#include <timer.h>

static uint64_t timer_raw_value(void)
{
	uint64_t value0;
	uint64_t value1;

	value0 = (uint64_t)read32(&timer7_ptr->timer_curr_value0);
	value1 = (uint64_t)read32(&timer7_ptr->timer_curr_value1);
	value0 = value0 | value1<<32;
	return value0;
}

void timer_monotonic_get(struct mono_time *mt)
{
	mono_time_set_usecs(mt, timer_raw_value() / clocks_per_usec);
}

void init_timer(void)
{
	write32(&timer7_ptr->timer_load_count0, TIMER_LOAD_VAL);
	write32(&timer7_ptr->timer_load_count1, TIMER_LOAD_VAL);
	write32(&timer7_ptr->timer_ctrl_reg, 1);
}
