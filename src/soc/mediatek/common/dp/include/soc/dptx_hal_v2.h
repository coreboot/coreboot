/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef SOC_MEDIATEK_COMMON_DP_DPTX_HAL_V2_H
#define SOC_MEDIATEK_COMMON_DP_DPTX_HAL_V2_H

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

extern const int dptx_hal_driving_offset[];
extern const int dptx_hal_driving_offset_size;

void dptx_hal_swing_emp_reset(struct mtk_dp *mtk_dp);
void dptx_hal_phy_set_idle_pattern(struct mtk_dp *mtk_dp, u8 lane_count, bool enable);
void dptx_hal_phy_set_lanes(struct mtk_dp *mtk_dp, u8 lane_count);

#endif /* SOC_MEDIATEK_COMMON_DP_DPTX_HAL_V2_H */
