/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/wdt.h>

#define MTK_WDT_REQ_MOD_KEY_VAL 0x33
#define MTK_WDT_REQ_IRQ_KEY_VAL 0x44

DEFINE_BITFIELD(MTK_WDT_REQ_MOD_KEY, 31, 24)
DEFINE_BITFIELD(MTK_WDT_REQ_IRQ_KEY, 31, 24)
DEFINE_BIT(MTK_WDT_THERMAL_EN, 18)
DEFINE_BIT(MTK_WDT_THERMAL_IRQ, 18)

void mtk_wdt_preinit(void)
{
	SET32_BITFIELDS(&mtk_wdt->wdt_req_mode,
			MTK_WDT_THERMAL_EN, 1,
			MTK_WDT_REQ_MOD_KEY, MTK_WDT_REQ_MOD_KEY_VAL);
	SET32_BITFIELDS(&mtk_wdt->wdt_req_irq_en,
			MTK_WDT_THERMAL_IRQ, 0,
			MTK_WDT_REQ_IRQ_KEY, MTK_WDT_REQ_IRQ_KEY_VAL);
}
