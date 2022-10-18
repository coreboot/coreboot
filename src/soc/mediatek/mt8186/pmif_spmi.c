/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/pll.h>
#include <soc/pmif_spmi.h>
#include <console/console.h>

/* IOCFG_LT, DRV_CFG2 */
DEFINE_BITFIELD(SPMI_SCL, 14, 12)
DEFINE_BITFIELD(SPMI_SDA, 17, 15)
DEFINE_BIT(SPMI_SCL_IN, 27)
DEFINE_BIT(SPMI_SDA_IN, 28)
DEFINE_BIT(SPMI_SCL_PU, 11)
DEFINE_BIT(SPMI_SDA_PD, 12)
DEFINE_BIT(SPMI_SCL_SMT, 28)
DEFINE_BIT(SPMI_SDA_SMT, 28)
DEFINE_BITFIELD(SPMI_TD, 19, 16)
DEFINE_BITFIELD(SPMI_RD, 15, 14)
DEFINE_BITFIELD(SPMI_DRI, 5, 3)

/* TOPRGU, WDT_SWSYSRST2 */
DEFINE_BIT(SPMI_MST_RST, 23)
DEFINE_BITFIELD(UNLOCK_KEY, 31, 24)

/* TOPCKGEN, CLK_CFG_17 */
DEFINE_BITFIELD(CLK_SPMI_MST_SEL, 10, 8)
DEFINE_BIT(CLK_SPMI_MST_INT, 12)
DEFINE_BIT(PDN_SPMI_MST, 15)

/* TOPCKGEN, CLK_CFG_UPDATE2 */
DEFINE_BIT(SPMI_MST_CK_UPDATE, 5)

const struct spmi_device spmi_dev[] = {
	{
		.slvid = SPMI_SLAVE_6,
		.type = BUCK_CPU,
		.type_id = BUCK_CPU_ID,
	},
};

const size_t spmi_dev_cnt = ARRAY_SIZE(spmi_dev);

int spmi_config_master(void)
{
	/* Software reset */
	SET32_BITFIELDS(&mtk_rug->wdt_swsysrst2, SPMI_MST_RST, 1, UNLOCK_KEY, 0x88);

	SET32_BITFIELDS(&mtk_topckgen->clk_cfg_11,
			CLK_SPMI_MST_SEL, 0x3,
			CLK_SPMI_MST_INT, 0,
			PDN_SPMI_MST, 0);
	SET32_BITFIELDS(&mtk_topckgen->clk_cfg_update2, SPMI_MST_CK_UPDATE, 1);

	/* Software reset */
	SET32_BITFIELDS(&mtk_rug->wdt_swsysrst2, SPMI_MST_RST, 0, UNLOCK_KEY, 0x88);

	/* Enable SPMI */
	write32(&mtk_spmi_mst->mst_req_en, 1);
	write32(&mtk_spmi_mst->rcs_ctrl, 0x15);

	return 0;
}
