/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8196 Functional Specification
 * Chapter number: 1.2 2.2
 */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/timer.h>

static void clear_systimer(struct systimer *const mtk_systimer)
{
	unsigned int id = 0;

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
	struct systimer *mtk_systimer = (void *)SYSTIMER_BASE;

	SET32_BITFIELDS(&mtk_systimer->cntcr,
			COMP_FEATURE, COMP_FEATURE_CLR | COMP_FEATURE_20_EN,
			COMP_FEATURE_TIE, COMP_FEATURE_TIE_CLR | COMP_FEATURE_TIE_EN);
	clear_systimer(mtk_systimer);
}
