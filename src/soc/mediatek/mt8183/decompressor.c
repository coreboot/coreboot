/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <soc/mmu_operations.h>

void decompressor_soc_init(void)
{
	mtk_mmu_init();
}
