/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8196_DP_DPTX_HAL_H__
#define __SOC_MEDIATEK_MT8196_DP_DPTX_HAL_H__

#include <soc/dptx_hal_common.h>

#define MTK_DP_TRAIN_VOLTAGE_LEVEL_RETRY	5
#define MTK_DP_TRAIN_DOWNSCALE_RETRY		10

enum {
	DPTX_PATTERN_TPS1 = 1,
	DPTX_PATTERN_TPS2,
	DPTX_PATTERN_TPS3,
	DPTX_PATTERN_TPS4,
	DPTX_PATTERN_UNKNOWN,
};

void dptx_hal_swing_emp_reset(struct mtk_dp *mtk_dp);
void dptx_hal_phy_init(struct mtk_dp *mtk_dp);
void dptx_hal_phy_set_lanes(struct mtk_dp *mtk_dp, u8 lane_count);
void dptx_hal_phy_wait_aux_ldo_ready(struct mtk_dp *mtk_dp);
void dptx_hal_phy_set_idle_pattern(struct mtk_dp *mtk_dp, u8 lane_count, bool enable);
void dptx_hal_phy_set_swing_preemphasis(struct mtk_dp *mtk_dp, u8 lane_count, u8 *swing_val,
					u8 *preemphasis);

#endif /* __SOC_MEDIATEK_MT8196_DP_DPTX_HAL_H__ */
