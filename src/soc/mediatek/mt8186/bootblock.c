/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/mmu_operations.h>

void bootblock_soc_init(void)
{
	mtk_mmu_init();
}
