/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/mmu_operations.h>
#include <soc/mt6391.h>
#include <soc/pll.h>
#include <soc/wdt.h>

void bootblock_soc_init(void)
{
	mt_pll_init();

	/* init pmic wrap SPI interface and pmic */
	mt6391_init();

	/* post init pll */
	mt_pll_post_init();

	mtk_mmu_init();

	/* init watch dog, will disable AP watch dog */
	mtk_wdt_init();
}
