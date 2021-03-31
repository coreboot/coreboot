/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/mmu.h>

void bootblock_soc_init(void)
{
	sc7280_mmu_init();
}
