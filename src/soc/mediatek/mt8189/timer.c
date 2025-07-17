/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 9.16
 */

#include <delay.h>
#include <soc/timer.h>

void timer_set_compensation(void)
{
	SET32_BITFIELDS(&mtk_systimer->cntcr,
			COMP_FEATURE, COMP_FEATURE_CLR | COMP_FEATURE_25_EN);
}

void init_timer(void)
{
	timer_prepare();

	raw_write_cntfrq_el0(TIMER_MHZ * MHz);
}
