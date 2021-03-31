/* SPDX-License-Identifier: GPL-2.0-only */

#include <delay.h>
#include <arch/lib_helpers.h>
#include <commonlib/helpers.h>

void init_timer(void)
{
	raw_write_cntfrq_el0(19200*KHz);
}
