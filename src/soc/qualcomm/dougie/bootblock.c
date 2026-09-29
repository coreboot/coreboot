/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/mmu.h>

void bootblock_soc_early_init(void)
{
	if (!CONFIG(COMPRESS_BOOTBLOCK))
		soc_mmu_init();
}
