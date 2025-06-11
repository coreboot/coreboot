/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 10.28
 */

#include <console/console.h>
#include <delay.h>
#include <soc/dptx.h>
#include <soc/dptx_hal.h>
#include <soc/dptx_reg.h>

void dptx_hal_reset_swing_preemphasis(struct mtk_dp *mtk_dp)
{
	u32 mask = EDP_TX_LN_VOLT_SWING_VAL_MASK | EDP_TX_LN_PRE_EMPH_VAL_MASK |
		   EDP_TX_LN_VOLT_SWING_EN_MASK | EDP_TX_LN_PRE_EMPH_EN_MASK;
	u32 value = EDP_TX_LN_VOLT_SWING_EN_MASK | EDP_TX_LN_PRE_EMPH_EN_MASK;

	for (int i = 0; i < dptx_hal_driving_offset_size; i++)
		mtk_dp_phy_mask(mtk_dp, dptx_hal_driving_offset[i], value, mask);
}

void dptx_hal_phy_set_lanes(struct mtk_dp *mtk_dp, u8 lane_count)
{
	mtk_dp_phy_mask(mtk_dp, PHYD_DIG_GLB_OFFSET + 0x44,
			GENMASK(4 + lane_count - 1, 4), GENMASK(7, 4));
}

void dptx_hal_phyd_reset(struct mtk_dp *mtk_dp)
{
	mtk_dp_phy_mask(mtk_dp, DP_PHY_DIG_SW_RST, 0, BIT(0));
	udelay(10);
	mtk_dp_phy_mask(mtk_dp, DP_PHY_DIG_SW_RST, BIT(0), BIT(0));

	dptx_hal_reset_swing_preemphasis(mtk_dp);
}
