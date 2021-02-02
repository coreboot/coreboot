/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/iocfg.h>
#include <soc/pll.h>
#include <soc/pmif_spmi.h>

/* IOCFG_BM, DRV_CFG2 */
DEFINE_BITFIELD(SPMI_SCL, 5, 3)
DEFINE_BITFIELD(SPMI_SDA, 8, 6)

/* TOPRGU, WDT_SWSYSRST2 */
DEFINE_BIT(SPMI_MST_RST, 4)
DEFINE_BITFIELD(UNLOCK_KEY, 31, 24)

/* TOPCKGEN, CLK_CFG_15 */
DEFINE_BITFIELD(CLK_SPMI_MST_SEL, 10, 8)
DEFINE_BIT(CLK_SPMI_MST_INT, 12)
DEFINE_BIT(PDN_SPMI_MST, 15)

/* TOPCKGEN, CLK_CFG_UPDATE2 */
DEFINE_BIT(SPMI_MST_CK_UPDATE, 30)

int spmi_config_master(void)
{
	/* Software reset */
	SET32_BITFIELDS(&mtk_rug->wdt_swsysrst2, SPMI_MST_RST, 1, UNLOCK_KEY, 0x85);

	SET32_BITFIELDS(&mtk_topckgen->clk_cfg_15_clr,
			CLK_SPMI_MST_SEL, 0x7,
			CLK_SPMI_MST_INT, 1,
			PDN_SPMI_MST, 1);
	SET32_BITFIELDS(&mtk_topckgen->clk_cfg_update2, SPMI_MST_CK_UPDATE, 1);

	/* Software reset */
	SET32_BITFIELDS(&mtk_rug->wdt_swsysrst2, SPMI_MST_RST, 0, UNLOCK_KEY, 0x85);

	/* Enable SPMI */
	write32(&mtk_spmi_mst->mst_req_en, 1);

	return 0;
}

void pmif_spmi_iocfg(void)
{
	SET32_BITFIELDS(&mtk_iocfg_bm->drv_cfg2, SPMI_SCL, 0x2, SPMI_SDA, 0x2);
}
