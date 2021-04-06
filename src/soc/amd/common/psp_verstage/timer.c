/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl_uapp/bl_syscall_public.h>
#include <stdint.h>
#include <timer.h>

void timer_monotonic_get(struct mono_time *mt)
{
	/* Chrono timer is based on a 25MHz clock */
	uint64_t clk;

	svc_read_timer_val(PSP_TIMER_TYPE_CHRONO, &clk);

	mt->microseconds = clk / 25;
}
