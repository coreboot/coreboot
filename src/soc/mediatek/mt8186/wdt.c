/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 3.4
 */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/wdt.h>

#define MTK_WDT_CLR_STATUS 0x22000000

void mtk_wdt_clr_status(uint32_t wdt_sta)
{
	write32(&mtk_wdt->wdt_mode, wdt_sta | MTK_WDT_CLR_STATUS);
}
