/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <delay.h>
#include <soc/dptx.h>
#include <soc/dptx_hal.h>
#include <soc/dptx_reg.h>

static void dptx_hal_phy_reset_swing_preemphasis(struct mtk_dp *mtk_dp)
{
	for (int i = 0; i < dptx_hal_driving_offset_size; i++)
		mtk_dp_phy_mask(mtk_dp, dptx_hal_driving_offset[i], 0,
				EDP_TX_LN_VOLT_SWING_VAL_MASK | EDP_TX_LN_PRE_EMPH_VAL_MASK);
}

void dptx_hal_phy_set_lanes(struct mtk_dp *mtk_dp, u8 lane_count)
{
	for (int i = 0; i < lane_count; i++)
		mtk_dp_phy_mask(mtk_dp, PHYD_DIG_GLB_OFFSET + 0x74, BIT(i), BIT(i));
}

void dptx_hal_phyd_reset(struct mtk_dp *mtk_dp)
{
	u32 val;

	mtk_dp_phy_mask(mtk_dp, DP_PHY_DIG_SW_RST, 0, BIT(0));
	udelay(10);
	mtk_dp_phy_mask(mtk_dp, DP_PHY_DIG_SW_RST, BIT(0), BIT(0));
	val = mtk_dp_phy_read(mtk_dp, DP_PHY_DIG_TX_CTL_0) & 0xF;
	printk(BIOS_DEBUG, "[eDPTX] DP_PHY_DIG_TX_CTL_0:%#x\n", val);

	while (val > 0) {
		val >>= 1;
		mtk_dp_phy_mask(mtk_dp, DP_PHY_DIG_TX_CTL_0, val, 0xF);
		printk(BIOS_DEBUG, "[eDPTX] DP_PHY_DIG_TX_CTL_0:%#x\n", val);
	}
	printk(BIOS_DEBUG, "[eDPTX] DP_PHY_DIG_TX_CTL_0:%#x\n",
	       mtk_dp_phy_read(mtk_dp, DP_PHY_DIG_TX_CTL_0));

	dptx_hal_phy_reset_swing_preemphasis(mtk_dp);
}
