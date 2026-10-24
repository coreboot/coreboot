/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * DSI panel driver for Novatek NT36536 BOE NS130070-M00 AMOLED
 * (2-2-2 topology, dual DSI, DSC v1.1 8bit).
 *
 * Panel timing and init commands are loaded from CBFS ("panel-BOE_NS130070_M00"),
 * built from src/drivers/mipi/panel-BOE_NS130070_M00.c.
 */

#include <cbfs.h>
#include <console/console.h>
#include <mipi/panel.h>
#include <soc/display/dsi_panel_nt36536.h>
#include <soc/display/mdss_dsc_calc.h>
#include <soc/display/mdssreg.h>
#include <soc/display/panel_ops.h>
#include <types.h>

static struct panel_serializable_data *nt36536_panel_data;

const u8 *dsi_panel_nt36536_get_init_cmds(void)
{
	if (!nt36536_panel_data)
		return NULL;
	return nt36536_panel_data->init;
}

/* Topology config: 2-2-2 dual DSI */
static const struct mdss_panel_config nt36536_panel_config = {
	.pipe_count      = 2,
	.dsc_ratio_x1000 = 3000, /* 8bpc/8bpp: 8*3*1000/8 = 3000 (3:1) */
	.intf_num        = 1,    /* DSI0 = INTF_1 (primary/master) */
	.dsi_count       = 2,    /* dual DSI: DSI0 + DSI1 (qcom,dsi-ctrl-num=<0 1>) */
};

static struct mdss_dsc_config nt36536_dsc_cfg = {
	.dsc_version_minor = 1,    /* DSC 1.1 */
	.scr               = 0,
	.bpc               = 8,    /* qcom,mdss-dsc-bit-per-component */
	.bpp               = 8,    /* qcom,mdss-dsc-bit-per-pixel */
	.block_pred        = 1,
	.multiplex_mode    = 0,    /* 2-2-2: each encoder -> own INTF */
	.slice_width       = 876,  /* qcom,mdss-dsc-slice-width */
	.slice_height      = 30,   /* qcom,mdss-dsc-slice-height */
	.slices_per_pkt    = 1,    /* qcom,mdss-dsc-slice-per-pkt */
	.num_interfaces    = 2,
	.initial_dec_delay = 0,    /* override only if HLOS golden dump gives one */
};

static struct mdss_dsc_params nt36536_dsc_params;
static bool nt36536_dsc_params_ready;

/*
 * DSI PHY 2.7.0 timing values from customer HLOS dump
 * dsi0_phy TIMING_CTRL[0..13] - calibrated for bit_clk=1372.9 MHz
 * Updated to match HLOS golden (9 values corrected vs calculated).
 */
static const u8 nt36536_phy_timings[DSI_PHY_TIMING_SIZE] = {
	0x00, /* [ 0] half_byte_clk_en */
	0x2C, /* [ 1] clk_zero         */
	0x0C, /* [ 2] clk_prepare      */
	0x0C, /* [ 3] clk_trail        */
	0x29, /* [ 4] hs_exit          */
	0x27, /* [ 5] hs_zero          */
	0x0C, /* [ 6] hs_prepare       */
	0x0C, /* [ 7] hs_trail         */
	0x08, /* [ 8] hs_rqst          */
	0x02, /* [ 9] ta_go            */
	0x04, /* [10] ta_get           */
	0x00, /* [11] trig3_cmd        */
	0x26, /* [12] clk_pre          */
	0x1A, /* [13] clk_post         */
};

static struct mdss_panel_ops nt36536_ops;

const struct mdss_panel_ops *dsi_panel_nt36536_get_ops(void)
{
	if (!nt36536_panel_data) {
		nt36536_panel_data = cbfs_map("panel-BOE_NS130070_M00", NULL);
		if (!nt36536_panel_data) {
			printk(BIOS_ERR, "nt36536: Could not find panel-BOE_NS130070_M00 in CBFS\n");
			return NULL;
		}
	}

	const struct edid_mode *edid_mode = &nt36536_panel_data->edid.mode;

	if (!nt36536_dsc_params_ready) {
		const struct dsc_config *dsc = &nt36536_panel_data->dsc_config;
		if (dsc->bits_per_component) {
			nt36536_dsc_cfg.dsc_version_minor = dsc->dsc_version_minor;
			nt36536_dsc_cfg.bpc               = dsc->bits_per_component;
			nt36536_dsc_cfg.bpp               = dsc->bits_per_pixel >> 4;
			nt36536_dsc_cfg.block_pred        = dsc->block_pred_enable;
			nt36536_dsc_cfg.slice_width       = dsc->slice_width;
			nt36536_dsc_cfg.slice_height      = dsc->slice_height;
		}
		if (mdss_dsc_calc(&nt36536_dsc_cfg, edid_mode,
				  &nt36536_dsc_params) != CB_SUCCESS) {
			printk(BIOS_ERR, "nt36536: DSC calc failed\n");
			return NULL;
		}
		nt36536_dsc_params_ready = true;
	}
	nt36536_ops.edid_mode               = edid_mode;
	nt36536_ops.config                  = &nt36536_panel_config;
	nt36536_ops.dsc_params              = &nt36536_dsc_params;
	nt36536_ops.video_compression_ctrl  =
		mdss_dsc_calc_video_compression_ctrl(&nt36536_dsc_cfg,
						      edid_mode);
	nt36536_ops.video_compression_ctrl2 = 0x0;
	nt36536_ops.get_init_cmds           = dsi_panel_nt36536_get_init_cmds;
	nt36536_ops.phy_timings             = nt36536_phy_timings;
	nt36536_ops.traffic_mode            = 2; /* burst mode - HLOS golden VIDEO_MODE_CTRL=0x02009230 */
	nt36536_ops.eotp_disabled           = 0; /* EOT packet enabled - TX_EOT_APPEND=1 */
	return &nt36536_ops;
}
