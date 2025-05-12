/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/mmu.h>
#include <soc/qspi_common.h>
#include <soc/qupv3_config_common.h>

#define SPI_BUS_CLOCK_FREQ (50 * MHz)

void bootblock_soc_init(void)
{
	if (!CONFIG(COMPRESS_BOOTBLOCK))
		soc_mmu_init();

	quadspi_init(SPI_BUS_CLOCK_FREQ);
	qupv3_fw_init();
}
