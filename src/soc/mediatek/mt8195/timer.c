/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/timer.h>

void timer_prepare(void)
{
	clrbits32((void *)SYSTIMER_BASE, COMP_FEATURE_MASK);
	setbits32((void *)SYSTIMER_BASE, COMP_25_MASK);
}
