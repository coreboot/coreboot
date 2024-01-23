/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8196 Functional Specification
 * Chapter number: 7.3.4.4
 */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/wdt.h>

#define MTK_WDT_CLR_STATUS 0x230001FF

void mtk_wdt_clr_status(void)
{
	write32(&mtk_wdt->wdt_mode, MTK_WDT_CLR_STATUS);
}
