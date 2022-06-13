/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/mmio.h>
#include <soc/spi.h>

void bootblock_mainboard_init(void)
{
	mtk_snfc_init();
}
