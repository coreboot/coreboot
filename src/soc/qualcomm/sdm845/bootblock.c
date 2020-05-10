/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/mmu.h>
#include <soc/clock.h>
#include <soc/qspi.h>

void bootblock_soc_init(void)
{
	clock_init();
	sdm845_mmu_init();
	quadspi_init(25 * MHz);
}
