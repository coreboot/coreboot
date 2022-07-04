/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/wdt.h>

void mtk_wdt_set_req(void)
{
	SET32_BITFIELDS(&mtk_wdt->wdt_req_mode,
			MTK_WDT_SPM_THERMAL_EN, MTK_WDT_SPM_THERMAL_VAL,
			MTK_WDT_THERMAL_EN, 1,
			MTK_WDT_REQ_MOD_KEY, MTK_WDT_REQ_MOD_KEY_VAL);
	SET32_BITFIELDS(&mtk_wdt->wdt_req_irq_en,
			MTK_WDT_THERMAL_IRQ, 0,
			MTK_WDT_REQ_IRQ_KEY, MTK_WDT_REQ_IRQ_KEY_VAL);
}
