/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <delay.h>
#include <soc/dptx.h>
#include <soc/dptx_hal.h>
#include <soc/dptx_reg.h>

void dptx_hal_reset_swing_preemphasis(struct mtk_dp *mtk_dp)
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
