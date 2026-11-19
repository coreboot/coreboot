/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * DSI panel driver for Tianma GA1 ILITEK79900A (1600x2560, 120Hz,
 * DSC 1.2 video burst mode on DSI1 with 3 C-PHY trios).
 *
 * Panel timing, DSC config, init, and power-off commands are loaded from
 * CBFS ("panel-TIANMA_ILITEK79900A"), built from
 * src/drivers/mipi/panel-TIANMA_ILITEK79900A.c.
 */

#include <cbfs.h>
#include <console/console.h>
#include <mipi/panel.h>
#include <soc/display/dsi_panel_ilitek79900a.h>
#include <soc/display/mdss_dsc_calc.h>
#include <soc/display/mdssreg.h>
#include <soc/display/panel_ops.h>
#include <types.h>

static struct panel_serializable_data *ilitek79900a_panel_data;

static const u8 *ilitek79900a_get_init_cmds(void)
{
	if (!ilitek79900a_panel_data)
		return NULL;
	return ilitek79900a_panel_data->init;
}

static struct mdss_panel_config ilitek79900a_config = {
	.pipe_count      = 2,
	.dsc_ratio_x1000 = 3000,
	.intf_num        = 2,
	.dsi_count       = 1,
	.dsi_controller  = 1,
	.num_lanes       = 3,
	.cphy_mode       = true,
	.cphy_clock_lane = true,
	.command_mode    = false,
	.prefetch_lines  = 1,
};

/* DSI1 C-PHY TIMING_CTRL_0..13 for 120 Hz mode. */
static const u8 ilitek79900a_phy_timings[DSI_PHY_TIMING_SIZE] = {
	0x00, /* CTRL_0 */
	0x01, /* CTRL_1 */
	0x01, /* CTRL_2 */
	0x02, /* CTRL_3 */
	0x22, /* CTRL_4 */
	0x21, /* CTRL_5 */
	0x07, /* CTRL_6 */
	0x19, /* CTRL_7 */
	0x07, /* CTRL_8 */
	0x02, /* CTRL_9 */
	0x04, /* CTRL_10 */
	0x00, /* CTRL_11 */
	0x03, /* CTRL_12 */
	0x2e, /* CTRL_13 */
};

/* DSC 1.2, 8 bpc / 8 bpp, two 800-byte slices in one DSI packet. */
static struct mdss_dsc_config ilitek79900a_dsc_config = {
	.dsc_version_minor = 2,
	.scr               = 0,
	.bpc               = 8,
	.bpp               = 8,
	.block_pred        = 1,
	.multiplex_mode    = 1,
	.slice_width       = 800,
	.slice_height      = 8,
	.slices_per_pkt    = 2,
	.num_interfaces    = 1,
	.initial_lines     = 3,
};

static struct mdss_dsc_params ilitek79900a_dsc_params;
static bool ilitek79900a_dsc_params_ready;
static struct mdss_panel_ops ilitek79900a_ops;

const struct mdss_panel_ops *dsi_panel_ilitek79900a_get_ops(void)
{
	if (!ilitek79900a_panel_data) {
		ilitek79900a_panel_data = cbfs_map("panel-TIANMA_ILITEK79900A", NULL);
		if (!ilitek79900a_panel_data) {
			printk(BIOS_ERR,
			       "ilitek79900a: Could not find panel-TIANMA_ILITEK79900A in CBFS\n");
			return NULL;
		}
	}

	const struct edid_mode *edid_mode = &ilitek79900a_panel_data->edid.mode;

	if (!ilitek79900a_dsc_params_ready) {
		const struct dsc_config *dsc = &ilitek79900a_panel_data->dsc_config;

		ilitek79900a_config.cphy_mode =
			!!(ilitek79900a_panel_data->flags & PANEL_FLAG_CPHY);
		if (dsc->bits_per_component) {
			ilitek79900a_dsc_config.dsc_version_minor = dsc->dsc_version_minor;
			ilitek79900a_dsc_config.bpc               = dsc->bits_per_component;
			ilitek79900a_dsc_config.bpp               = dsc->bits_per_pixel >> 4;
			ilitek79900a_dsc_config.block_pred        = dsc->block_pred_enable;
			ilitek79900a_dsc_config.slice_width       = dsc->slice_width;
			ilitek79900a_dsc_config.slice_height      = dsc->slice_height;
		}
		if (mdss_dsc_calc(&ilitek79900a_dsc_config, edid_mode,
				  &ilitek79900a_dsc_params) != CB_SUCCESS) {
			printk(BIOS_ERR, "ilitek79900a: DSC calc failed\n");
			return NULL;
		}
		ilitek79900a_dsc_params_ready = true;
	}

	ilitek79900a_ops.edid_mode              = edid_mode;
	ilitek79900a_ops.config                 = &ilitek79900a_config;
	ilitek79900a_ops.dsc_cfg                = &ilitek79900a_dsc_config;
	ilitek79900a_ops.dsc_params             = &ilitek79900a_dsc_params;
	ilitek79900a_ops.video_compression_ctrl =
		mdss_dsc_calc_video_compression_ctrl(&ilitek79900a_dsc_config, edid_mode);
	ilitek79900a_ops.get_init_cmds          = ilitek79900a_get_init_cmds;
	if (ilitek79900a_panel_data->poweroff[0] != PANEL_CMD_END)
		ilitek79900a_ops.off_cmds = ilitek79900a_panel_data->poweroff;
	ilitek79900a_ops.phy_timings            = ilitek79900a_phy_timings;
	ilitek79900a_ops.phy_timings_size       = sizeof(ilitek79900a_phy_timings);
	ilitek79900a_ops.clk_cfg1_override      = 0x33; /* CLK_EN, CLK_EN_SEL, DSICLK_SEL=3 */
	ilitek79900a_ops.traffic_mode           = 2;
	ilitek79900a_ops.reset_pre_ms           = 2;
	ilitek79900a_ops.reset_low_ms           = 10;
	ilitek79900a_ops.reset_post_ms          = 24;
	return &ilitek79900a_ops;
}
