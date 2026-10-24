/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * DSI panel driver for Visionox r66451 AMOLED (FHD+ 1080x2340, 120Hz, DSC, 2-2-1).
 *
 * Panel timing and init commands are loaded from CBFS ("panel-VIS_R66451"),
 * built from src/drivers/mipi/panel-VIS_R66451.c.
 */

#include <cbfs.h>
#include <console/console.h>
#include <mipi/panel.h>
#include <soc/display/dsi_panel_r66451.h>
#include <soc/display/mdss_dsc_calc.h>
#include <soc/display/mdssreg.h>
#include <soc/display/panel_ops.h>
#include <types.h>

static struct panel_serializable_data *r66451_panel_data;

const u8 *dsi_panel_r66451_get_init_cmds(void)
{
	if (!r66451_panel_data)
		return NULL;
	return r66451_panel_data->init;
}

const u8 *dsi_panel_r66451_get_poweroff_cmds(void)
{
	if (!r66451_panel_data)
		return NULL;
	return r66451_panel_data->poweroff;
}

/*
 * Topology config for r66451:
 *   2-2-1 (2 LM, 2 DSC encoders, 1 DSI interface)
 *   DSC 3:1 compression, DSI uses INTF_1 on x1p42100
 */
static const struct mdss_panel_config r66451_panel_config = {
	.pipe_count      = 2,
	.dsc_ratio_x1000 = 3000,
	.intf_num        = 1,  /* DSI0 uses INTF_1 on x1p42100 */
	.dsi_count       = 1,  /* single DSI (2-2-1 topology) */
};

const struct mdss_panel_config *dsi_panel_r66451_get_config(void)
{
	return &r66451_panel_config;
}

static struct mdss_dsc_config r66451_dsc_cfg = {
	.dsc_version_minor = 1,    /* DSC 1.1 */
	.scr               = 0,
	.bpc               = 8,
	.bpp               = 8,
	.block_pred        = 1,
	.multiplex_mode    = 1,    /* 2-2-1: both encoders merge to 1 INTF */
	.slice_width       = 540,
	.slice_height      = 20,
	.slices_per_pkt    = 2,
	.num_interfaces    = 1,
};

static struct mdss_dsc_params r66451_dsc_params;
static bool r66451_dsc_params_ready;

/*
 * PHY timings from qcom,mdss-dsi-panel-phy-timings:
 * [00 1d 08 07 17 22 07 07 08 02 04 00 19 0c]
 */
static const u8 r66451_phy_timings[DSI_PHY_TIMING_SIZE] = {
	0x00, /* CTRL_0:  half_byte_clk_en */
	0x1D, /* CTRL_1:  clk_zero */
	0x08, /* CTRL_2:  clk_prepare */
	0x07, /* CTRL_3:  clk_trail */
	0x17, /* CTRL_4:  hs_exit */
	0x22, /* CTRL_5:  hs_zero */
	0x07, /* CTRL_6:  hs_prepare */
	0x07, /* CTRL_7:  hs_trail */
	0x08, /* CTRL_8:  hs_rqst */
	0x02, /* CTRL_9:  ta_go|ta_sure */
	0x04, /* CTRL_10: ta_get */
	0x00, /* CTRL_11: trig3_cmd */
	0x19, /* CTRL_12: clk_pre */
	0x0C, /* CTRL_13: clk_post */
};

static struct mdss_panel_ops r66451_ops;

const struct mdss_panel_ops *dsi_panel_r66451_get_ops(void)
{
	if (!r66451_panel_data) {
		r66451_panel_data = cbfs_map("panel-VIS_R66451", NULL);
		if (!r66451_panel_data) {
			printk(BIOS_ERR, "r66451: Could not find panel-VIS_R66451 in CBFS\n");
			return NULL;
		}
	}

	const struct edid_mode *edid_mode = &r66451_panel_data->edid.mode;

	if (!r66451_dsc_params_ready) {
		const struct dsc_config *dsc = &r66451_panel_data->dsc_config;
		if (dsc->bits_per_component) {
			r66451_dsc_cfg.dsc_version_minor = dsc->dsc_version_minor;
			r66451_dsc_cfg.bpc               = dsc->bits_per_component;
			r66451_dsc_cfg.bpp               = dsc->bits_per_pixel >> 4;
			r66451_dsc_cfg.block_pred        = dsc->block_pred_enable;
			r66451_dsc_cfg.slice_width       = dsc->slice_width;
			r66451_dsc_cfg.slice_height      = dsc->slice_height;
		}
		if (mdss_dsc_calc(&r66451_dsc_cfg, edid_mode, &r66451_dsc_params) != CB_SUCCESS) {
			printk(BIOS_ERR, "r66451: DSC calc failed\n");
			return NULL;
		}
		r66451_dsc_params_ready = true;
	}
	r66451_ops.edid_mode               = edid_mode;
	r66451_ops.config                  = &r66451_panel_config;
	r66451_ops.dsc_params              = &r66451_dsc_params;
	r66451_ops.video_compression_ctrl  =
		mdss_dsc_calc_video_compression_ctrl(&r66451_dsc_cfg, edid_mode);
	r66451_ops.video_compression_ctrl2 = 0x00000000;
	r66451_ops.get_init_cmds           = dsi_panel_r66451_get_init_cmds;
	r66451_ops.dsc_cfg                 = &r66451_dsc_cfg;
	r66451_ops.phy_timings             = r66451_phy_timings;
	return &r66451_ops;
}
