/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/mmu_operations.h>
#include <soc/mt6315.h>
#include <soc/mt6359p.h>
#include <soc/pll.h>
#include <soc/pmif.h>
#include <soc/wdt.h>

void bootblock_soc_init(void)
{
	mtk_mmu_init();
	mtk_wdt_init();
	mt_pll_init();
	mtk_pmif_init();
	mt6359p_init();
	mt6315_init();
}
