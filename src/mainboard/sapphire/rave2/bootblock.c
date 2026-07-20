/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/espi.h>

void bootblock_mainboard_early_init(void)
{
	espi_switch_to_spi1_pads();
}
