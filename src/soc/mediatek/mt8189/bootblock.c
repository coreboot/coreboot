/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <bootblock_common.h>
#include <soc/lastbus_v2.h>
#include <soc/mmu_operations.h>
#include <soc/pll.h>
#include <soc/spm_mtcmos.h>
#include <soc/tracker.h>
#include <soc/wdt.h>

void bootblock_soc_init(void)
{
	mtk_mmu_init();
	bustracker_init();
	lastbus_init();
	mtk_wdt_init();
	mt_pll_init();
	mtcmos_init();
	mt_pll_post_init();
}
