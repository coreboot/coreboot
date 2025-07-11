/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/timer.h>

static void clear_systimer(void)
{
	int id;

	for (id = 0; id < SYSTIMER_CNT; id++) {
		u32 *cnttval_con = &mtk_systimer->cnttval[id].con;
		WRITE32_BITFIELDS(cnttval_con, SYST_CON, SYST_CON_EN);
		SET32_BITFIELDS(cnttval_con, SYST_CON, SYST_CON_IRQ_CLR);
		WRITE32_BITFIELDS(cnttval_con, SYST_CON, SYST_CON_CLR);
	}

	SET32_BITFIELDS(&mtk_systimer->cntcr, REV_SET, REV_CLR_EN);
}

void timer_prepare(void)
{
	timer_set_compensation();
	clear_systimer();
}
