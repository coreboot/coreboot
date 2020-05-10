/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/mmu.h>
#include <soc/clock.h>

void bootblock_soc_early_init(void)
{
	clock_init();
}

void bootblock_soc_init(void)
{
	qcs405_mmu_init();
}
