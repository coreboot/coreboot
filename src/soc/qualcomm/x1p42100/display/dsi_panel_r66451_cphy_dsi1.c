/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * DSI panel driver for Visionox R66451 AMOLED (1080x2340, 120Hz,
 * DSC 1.1 command mode on DSI1 with 3 C-PHY trios).
 *
 * Panel timing, DSC config, and init commands are loaded from CBFS
 * ("panel-VIS_R66451_CPHY_DSI1"), built from
 * src/drivers/mipi/panel-VIS_R66451_CPHY_DSI1.c.
 */

#include <cbfs.h>
#include <console/console.h>
#include <mipi/panel.h>
#include <soc/display/dsi_panel_r66451_cphy_dsi1.h>
#include <soc/display/mdss_dsc_calc.h>
#include <soc/display/mdssreg.h>
#include <soc/display/panel_ops.h>
#include <types.h>

static struct panel_serializable_data *r66451_cphy_panel_data;

static const u8 *r66451_cphy_get_init_cmds(void)
{
	if (!r66451_cphy_panel_data)
		return NULL;
	return r66451_cphy_panel_data->init;
}

static struct mdss_panel_config r66451_cphy_panel_config = {
	.pipe_count      = 2,
	.dsc_ratio_x1000 = 3000,
	.intf_num        = 2,
	.dsi_count       = 1,
	.dsi_controller  = 1,
	.num_lanes       = 3,
	.cphy_mode       = true,
	.command_mode    = true,
};

static struct mdss_dsc_config r66451_cphy_dsc_cfg = {
	.dsc_version_minor = 1,
	.scr               = 0,
	.bpc               = 8,
	.bpp               = 8,
	.block_pred        = 1,
	.multiplex_mode    = 1,
	.slice_width       = 540,
	.slice_height      = 20,
	.slices_per_pkt    = 2,
	.num_interfaces    = 1,
};

static struct mdss_dsc_params r66451_cphy_dsc_params;
static bool r66451_cphy_dsc_params_ready;
static struct mdss_panel_ops r66451_cphy_ops;

const struct mdss_panel_ops *dsi_panel_r66451_cphy_dsi1_get_ops(void)
{
	if (!r66451_cphy_panel_data) {
		r66451_cphy_panel_data = cbfs_map("panel-VIS_R66451_CPHY_DSI1", NULL);
		if (!r66451_cphy_panel_data) {
			printk(BIOS_ERR,
			       "r66451_cphy: Could not find panel-VIS_R66451_CPHY_DSI1 in CBFS\n");
			return NULL;
		}
	}

	const struct edid_mode *edid_mode = &r66451_cphy_panel_data->edid.mode;

	if (!r66451_cphy_dsc_params_ready) {
		const struct dsc_config *dsc = &r66451_cphy_panel_data->dsc_config;

		r66451_cphy_panel_config.cphy_mode =
			!!(r66451_cphy_panel_data->flags & PANEL_FLAG_CPHY);
		if (dsc->bits_per_component) {
			r66451_cphy_dsc_cfg.dsc_version_minor = dsc->dsc_version_minor;
			r66451_cphy_dsc_cfg.bpc               = dsc->bits_per_component;
			r66451_cphy_dsc_cfg.bpp               = dsc->bits_per_pixel >> 4;
			r66451_cphy_dsc_cfg.block_pred        = dsc->block_pred_enable;
			r66451_cphy_dsc_cfg.slice_width       = dsc->slice_width;
			r66451_cphy_dsc_cfg.slice_height      = dsc->slice_height;
		}
		if (mdss_dsc_calc(&r66451_cphy_dsc_cfg, edid_mode,
				  &r66451_cphy_dsc_params) != CB_SUCCESS) {
			printk(BIOS_ERR, "r66451_cphy: DSC calc failed\n");
			return NULL;
		}
		r66451_cphy_dsc_params_ready = true;
	}

	r66451_cphy_ops.edid_mode              = edid_mode;
	r66451_cphy_ops.config                 = &r66451_cphy_panel_config;
	r66451_cphy_ops.dsc_params             = &r66451_cphy_dsc_params;
	r66451_cphy_ops.dsc_cfg                = &r66451_cphy_dsc_cfg;
	r66451_cphy_ops.video_compression_ctrl =
		mdss_dsc_calc_video_compression_ctrl(&r66451_cphy_dsc_cfg, edid_mode);
	r66451_cphy_ops.get_init_cmds          = r66451_cphy_get_init_cmds;
	return &r66451_cphy_ops;
}
