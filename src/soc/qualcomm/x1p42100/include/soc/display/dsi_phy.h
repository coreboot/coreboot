/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_X1P42100_DSI_PHY_H_
#define _SOC_QUALCOMM_X1P42100_DSI_PHY_H_

#include <edid.h>
#include <types.h>

/*
 * DSI PHY 2.7.0 timing parameters (D-PHY).
 * Matches UEFI HAL_DSI_PHY_TimingConfigType.
 */
struct msm_dsi_phy_ctrl {
	u32 clk_pre;
	u32 clk_post;
	u32 clk_zero;
	u32 clk_trail;
	u32 clk_prepare;
	u32 hs_exit;
	u32 hs_zero;
	u32 hs_prepare;
	u32 hs_trail;
	u32 hs_rqst;
	u32 ta_go;
	u32 ta_sure;
	u32 ta_get;
	u32 half_byte_clk_en;
	bool clk_pre_inc_by_2;
};

/*
 * Top-level PHY init entry point.
 * Performs: PHY reset -> power-up -> lane init -> PLL setup -> clock enable.
 */
enum cb_err mdss_dsi_phy_270_init(struct edid *edid, u32 num_of_lanes, u32 bpp);
enum cb_err mdss_dsi1_phy_270_init(struct edid *edid, u32 num_of_lanes, u32 bpp);

#endif /* _SOC_QUALCOMM_X1P42100_DSI_PHY_H_ */
