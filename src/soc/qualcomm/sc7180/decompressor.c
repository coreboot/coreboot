/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/mmu.h>

void decompressor_soc_init(void)
{
	sc7180_mmu_init();
}
