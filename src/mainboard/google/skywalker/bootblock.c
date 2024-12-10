/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <bootblock_common.h>
#include <soc/spi.h>

void bootblock_mainboard_init(void)
{
	mtk_snfc_init();
}
