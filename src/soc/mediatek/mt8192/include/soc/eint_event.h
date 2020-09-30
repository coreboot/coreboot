/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_MEDIATEK_MT8192_EINT_EVENT_H
#define SOC_MEDIATEK_MT8192_EINT_EVENT_H

#include <device/mmio.h>
#include <soc/addressmap.h>

/* eint event mask cler register */
struct eint_event_reg {
	uint32_t eint_event_mask_clr[7];
};

/* eint_base + 0x880 is eint_event_mask_clr register with access type W1C. */
static struct eint_event_reg *const mtk_eint_event = (void *)(EINT_BASE + 0x880);

/* unmask eint event, eint can wakeup by spm */
void unmask_eint_event_mask(void);

#endif
