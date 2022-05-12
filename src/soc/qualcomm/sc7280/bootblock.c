/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/clock.h>
#include <soc/qspi_common.h>
#include <soc/qupv3_config_common.h>

void bootblock_soc_init(void)
{
	clock_init();
	/*
	 * Through experimentation, we have determined
	 * that a delay of 1/8 cycle is best for herobrine.
	 * See b/190231148
	 */
	quadspi_init(75000 * KHz, 1);
	qupv3_fw_init();
}
