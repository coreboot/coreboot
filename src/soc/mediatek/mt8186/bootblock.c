/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/eint_event.h>
#include <soc/gic.h>
#include <soc/mmu_operations.h>
#include <soc/pll.h>
#include <soc/tracker_common.h>
#include <soc/wdt.h>

void bootblock_soc_init(void)
{
	mtk_mmu_init();
	bustracker_init();
	mtk_wdt_init();
	mt_pll_init();
	unmask_eint_event_mask();
	mtk_gic_preinit();
}
