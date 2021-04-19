/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/eint_event.h>

void unmask_eint_event_mask(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(mtk_eint_event->eint_event_mask_clr); i++)
		write32(&mtk_eint_event->eint_event_mask_clr[i], 0xffffffff);
}
