/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/clock.h>
#include <soc/mmu.h>
#include <soc/qspi.h>
#include <soc/qupv3_config.h>

void bootblock_soc_init(void)
{
	sc7180_mmu_init();
	clock_init();
	quadspi_init(37500 * KHz);
	qupv3_fw_init();
}
