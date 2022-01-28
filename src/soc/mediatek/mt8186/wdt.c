/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 3.4
 */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/wdt.h>

#define MTK_WDT_CLR_STATUS_VAL 0x22

DEFINE_BITFIELD(MTK_WDT_CLR_STATUS, 31, 24)

void mtk_wdt_clr_status(void)
{
	SET32_BITFIELDS(&mtk_wdt->wdt_mode,
			MTK_WDT_CLR_STATUS, MTK_WDT_CLR_STATUS_VAL);
}
