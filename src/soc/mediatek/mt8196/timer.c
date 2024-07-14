/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/lib_helpers.h>
#include <commonlib/helpers.h>
#include <delay.h>
#include <soc/timer.h>

void init_timer(void)
{
	timer_prepare();

	raw_write_cntfrq_el0(GPT_MHZ * MHz);
}
