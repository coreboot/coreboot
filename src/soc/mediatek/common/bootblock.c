/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/mmu_operations.h>

void bootblock_soc_early_init(void)
{
	if (CONFIG(EARLY_MMU_INIT))
		mtk_mmu_init();
}
