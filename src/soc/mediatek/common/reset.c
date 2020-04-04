/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/mmio.h>
#include <reset.h>
#include <soc/wdt.h>

void do_board_reset(void)
{
	write32(&mtk_wdt->wdt_swrst, MTK_WDT_SWRST_KEY);
}
