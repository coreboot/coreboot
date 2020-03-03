/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_DISPLAY_DSI_PHY_H
#define _SOC_DISPLAY_DSI_PHY_H

#include <stdint.h>
#include <soc/display/dsi_phy_pll.h>

#define MAX_REGULATOR_CONFIG            7
#define MAX_BIST_CONFIG                 6
#define MAX_TIMING_CONFIG               40
#define MAX_LANE_CONFIG                 45
#define MAX_STRENGTH_CONFIG             10
#define MAX_CTRL_CONFIG                 4
#define DMA_TPG_FIFO_LEN                64

struct msm_panel_info;
struct mipi_dsi_phy_ctrl {
	uint32_t regulator[5];
	uint32_t timing[12];
	uint32_t ctrl[4];
	uint32_t strength[4];
	uint32_t pll[21];
};

enum dsi_reg_mode {
	DSI_PHY_REGULATOR_DCDC_MODE,
	DSI_PHY_REGULATOR_LDO_MODE,
};

enum {
	DSI_PLL_TYPE_10NM,
	DSI_PLL_TYPE_MAX,
};

struct msm_dsi_phy_ctrl {
	uint32_t clk_pre;
	uint32_t clk_post;
	uint32_t clk_zero;
	uint32_t clk_trail;
	uint32_t clk_prepare;
	uint32_t hs_exit;
	uint32_t hs_zero;
	uint32_t hs_prepare;
	uint32_t hs_trail;
	uint32_t hs_rqst;
	uint32_t ta_go;
	uint32_t ta_sure;
	uint32_t ta_get;
	uint32_t half_byte_clk_en;
	bool clk_pre_inc_by_2;
};

struct mdss_dsi_phy_ctrl {
	uint32_t regulator[MAX_REGULATOR_CONFIG];
	uint32_t timing[MAX_TIMING_CONFIG];
	uint32_t ctrl[MAX_CTRL_CONFIG];
	uint32_t strength[MAX_STRENGTH_CONFIG];
	char bistCtrl[MAX_BIST_CONFIG];
	char laneCfg[MAX_LANE_CONFIG];
	enum dsi_reg_mode regulator_mode;
	int pll_type;
};

enum cb_err mdss_dsi_phy_10nm_init(struct edid *edid, uint32_t num_of_lanes, uint32_t bpp);

#endif
