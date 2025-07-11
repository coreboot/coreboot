/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/lib_helpers.h>
#include <commonlib/helpers.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/timer.h>

void timer_set_compensation(void)
{
	SET32_BITFIELDS(&mtk_systimer->cntcr,
			COMP_FEATURE, COMP_FEATURE_CLR | COMP_FEATURE_20_EN,
			COMP_FEATURE_TIE, COMP_FEATURE_TIE_CLR | COMP_FEATURE_TIE_EN);
}

void init_timer(void)
{
	timer_prepare();

	raw_write_cntfrq_el0(TIMER_MHZ * MHz);
}
