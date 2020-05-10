/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <reset.h>
#include <soc/wdt.h>

void do_board_reset(void)
{
	write32(&mtk_wdt->wdt_swrst, MTK_WDT_SWRST_KEY);
}
