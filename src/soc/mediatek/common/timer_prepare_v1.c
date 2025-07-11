/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/timer.h>
#include <soc/timer_reg.h>

void timer_prepare(void)
{
	clrbits32(&mtk_systimer->cntcr, COMP_FEATURE_MASK);
	setbits32(&mtk_systimer->cntcr, COMP_25_MASK);
}
