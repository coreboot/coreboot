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
	u32 saved_cntcv_l;
	u32 saved_cntcv_h;

	saved_cntcv_h = read32(&mtk_systimer->cntcv_h);
	saved_cntcv_l = read32(&mtk_systimer->cntcv_l);

	timer_prepare();

	write32(&mtk_systimer->cntcv_h, saved_cntcv_h);
	write32(&mtk_systimer->cntcv_l, saved_cntcv_l);

	raw_write_cntfrq_el0(TIMER_MHZ * MHz);
	udelay(100);
}
