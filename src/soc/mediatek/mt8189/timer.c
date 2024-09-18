/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 9.16
 */

#include <arch/lib_helpers.h>
#include <commonlib/helpers.h>
#include <delay.h>
#include <soc/timer.h>

void init_timer(void)
{
	timer_prepare();

	raw_write_cntfrq_el0(TIMER_MHZ * MHz);
}
