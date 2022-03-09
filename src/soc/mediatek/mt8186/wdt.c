/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MT8186 Functional Specification
 * Chapter number: 3.4
 */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/wdt.h>

#define MTK_WDT_CLR_STATUS_VAL 0x22
#define MTK_WDT_REQ_MOD_KEY_VAL 0x33

DEFINE_BITFIELD(MTK_WDT_CLR_STATUS, 31, 24)
DEFINE_BITFIELD(MTK_WDT_REQ_MOD_KEY, 31, 24)
DEFINE_BIT(MTK_WDT_SPM_THERMAL_EN, 0)

void mtk_wdt_preinit(void)
{
	SET32_BITFIELDS(&mtk_wdt->wdt_req_mode,
			MTK_WDT_SPM_THERMAL_EN, 0,
			MTK_WDT_REQ_MOD_KEY, MTK_WDT_REQ_MOD_KEY_VAL);
}

void mtk_wdt_clr_status(void)
{
	SET32_BITFIELDS(&mtk_wdt->wdt_mode,
			MTK_WDT_CLR_STATUS, MTK_WDT_CLR_STATUS_VAL);
}
