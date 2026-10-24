/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * DSI panel driver for sim dual-DSI DSC 3:1 video panel (2-2-2 topology).
 *
 * Panel timing and init commands are loaded from CBFS ("panel-SIM_DUALMIPI"),
 * built from src/drivers/mipi/panel-SIM_DUALMIPI.c.
 */

#include <cbfs.h>
#include <console/console.h>
#include <mipi/panel.h>
#include <soc/display/dsi_panel_sim_dualmipi.h>
#include <soc/display/mdss_dsc_calc.h>
#include <soc/display/mdssreg.h>
#include <soc/display/panel_ops.h>
#include <types.h>

static struct panel_serializable_data *sim_dualmipi_panel_data;

const u8 *dsi_panel_sim_dualmipi_get_init_cmds(void)
{
	if (!sim_dualmipi_panel_data)
		return NULL;
	return sim_dualmipi_panel_data->init;
}

/*
 * Topology config: 2-2-2 dual DSI
 * pipe_count=2, dsc_ratio=3, intf_num=1 (primary), dsi_count=2
 */
static const struct mdss_panel_config sim_dualmipi_panel_config = {
	.pipe_count      = 2,
	.dsc_ratio_x1000 = 3000,  /* 8bpc/8bpp: 8*3*1000/8 = 3000 */
	.intf_num        = 1,     /* DSI0 = INTF_1 (primary/master) */
	.dsi_count       = 2,     /* dual DSI: DSI0 + DSI1 */
};

static struct mdss_dsc_config sim_dualmipi_dsc_cfg = {
	.dsc_version_minor = 1,    /* DSC 1.1 */
	.scr               = 0,
	.bpc               = 8,    /* qcom,mdss-dsc-bit-per-component */
	.bpp               = 8,    /* qcom,mdss-dsc-bit-per-pixel */
	.block_pred        = 1,
	.multiplex_mode    = 0,    /* 2-2-2: each encoder to its own INTF */
	.slice_width       = 720,  /* qcom,mdss-dsc-slice-width */
	.slice_height      = 40,   /* qcom,mdss-dsc-slice-height */
	.slices_per_pkt    = 1,    /* qcom,mdss-dsc-slice-per-pkt */
	.num_interfaces    = 2,
	.initial_dec_delay = 616,  /* 0x268 from HLOS golden dump */
};

static struct mdss_dsc_params sim_dualmipi_dsc_params;
static bool sim_dualmipi_dsc_params_ready;

/*
 * PHY timings from DTSI: qcom,mdss-dsi-panel-phy-timings
 * [00 24 0a 0a 1a 24 0a 0a 09 02 04 00 1e 0f]
 */
static const u8 sim_dualmipi_phy_timings[DSI_PHY_TIMING_SIZE] = {
	0x00, /* CTRL_0:  half_byte_clk_en */
	0x24, /* CTRL_1:  clk_zero */
	0x0A, /* CTRL_2:  clk_prepare */
	0x0A, /* CTRL_3:  clk_trail */
	0x1A, /* CTRL_4:  hs_exit */
	0x24, /* CTRL_5:  hs_zero */
	0x0A, /* CTRL_6:  hs_prepare */
	0x0A, /* CTRL_7:  hs_trail */
	0x09, /* CTRL_8:  hs_rqst */
	0x02, /* CTRL_9:  ta_go|ta_sure */
	0x04, /* CTRL_10: ta_get */
	0x00, /* CTRL_11: trig3_cmd */
	0x1E, /* CTRL_12: clk_pre */
	0x0F, /* CTRL_13: clk_post */
};

static struct mdss_panel_ops sim_dualmipi_ops;

const struct mdss_panel_ops *dsi_panel_sim_dualmipi_get_ops(void)
{
	if (!sim_dualmipi_panel_data) {
		sim_dualmipi_panel_data = cbfs_map("panel-SIM_DUALMIPI", NULL);
		if (!sim_dualmipi_panel_data) {
			printk(BIOS_ERR, "sim_dualmipi: Could not find panel-SIM_DUALMIPI in CBFS\n");
			return NULL;
		}
	}

	const struct edid_mode *edid_mode = &sim_dualmipi_panel_data->edid.mode;

	if (!sim_dualmipi_dsc_params_ready) {
		const struct dsc_config *dsc = &sim_dualmipi_panel_data->dsc_config;
		if (dsc->bits_per_component) {
			sim_dualmipi_dsc_cfg.dsc_version_minor = dsc->dsc_version_minor;
			sim_dualmipi_dsc_cfg.bpc               = dsc->bits_per_component;
			sim_dualmipi_dsc_cfg.bpp               = dsc->bits_per_pixel >> 4;
			sim_dualmipi_dsc_cfg.block_pred        = dsc->block_pred_enable;
			sim_dualmipi_dsc_cfg.slice_width       = dsc->slice_width;
			sim_dualmipi_dsc_cfg.slice_height      = dsc->slice_height;
			sim_dualmipi_dsc_cfg.initial_dec_delay = dsc->initial_dec_delay;
		}
		if (mdss_dsc_calc(&sim_dualmipi_dsc_cfg, edid_mode,
				  &sim_dualmipi_dsc_params) != CB_SUCCESS) {
			printk(BIOS_ERR, "sim_dualmipi: DSC calc failed\n");
			return NULL;
		}
		sim_dualmipi_dsc_params_ready = true;
	}
	sim_dualmipi_ops.edid_mode               = edid_mode;
	sim_dualmipi_ops.config                  = &sim_dualmipi_panel_config;
	sim_dualmipi_ops.dsc_params              = &sim_dualmipi_dsc_params;
	sim_dualmipi_ops.video_compression_ctrl  =
		mdss_dsc_calc_video_compression_ctrl(&sim_dualmipi_dsc_cfg, edid_mode);
	sim_dualmipi_ops.video_compression_ctrl2 = 0x00000000;
	sim_dualmipi_ops.get_init_cmds           = dsi_panel_sim_dualmipi_get_init_cmds;
	sim_dualmipi_ops.dsc_cfg                 = &sim_dualmipi_dsc_cfg;
	sim_dualmipi_ops.phy_timings             = sim_dualmipi_phy_timings;
	return &sim_dualmipi_ops;
}
