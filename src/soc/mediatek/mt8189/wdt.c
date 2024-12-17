/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 8.2
 */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/efuse.h>
#include <soc/wdt.h>

#define MTK_WDT_CLR_STATUS		0x230001FF
#define MTK_WDT_CLR_EFUSE_ECC_ERR	0x2300000A
#define MTK_WDT_STA_EFUSE_ECC_ERR	BIT(23)

void mtk_wdt_clr_status(void)
{
	write32(&mtk_wdt->wdt_mode, MTK_WDT_CLR_STATUS);
}

void mtk_wdt_clear_efuse_ecc(void)
{
	if ((read32(&mtk_wdt->wdt_status) & MTK_WDT_STA_EFUSE_ECC_ERR) &&
	    !mtk_efuse_ecc_has_error())
		write32(&mtk_wdt->wdt_mode, MTK_WDT_CLR_EFUSE_ECC_ERR);
}
