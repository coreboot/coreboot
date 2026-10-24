/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_X1P42100_PANEL_OPS_H_
#define _SOC_QUALCOMM_X1P42100_PANEL_OPS_H_

#include <edid.h>
#include <soc/display/mdssreg.h>
#include <types.h>

/*
 * mdss_dsc_params - pre-calculated DSC register values for one panel config.
 * All values are panel-specific constants derived from the DSC profile table.
 * Source: UEFI hal_mdp_dsc.h + HAL_MDP_DSC_ENC_Commit() register layout.
 */
struct mdss_dsc_params {
	u32 enc_main_conf;       /* DSC_CMN: SPLIT_PANEL, MULTIPLEX_MODE, NUM_OF_SS */
	u32 enc_df_ctrl;         /* ENC_DF_CTRL: INITIAL_LINES, VIDEO_MODE, OB_MAX_ADDR */
	u32 dsc_main_conf;       /* BPC, BPP, BLOCK_PRED, DSC_MINOR */
	u32 dsc_picture_size;    /* HEIGHT[31:16] | WIDTH[15:0] */
	u32 dsc_slice_size;      /* SLICE_HEIGHT[31:16] | SLICE_WIDTH[15:0] */
	u32 dsc_misc_size;       /* CHUNK_SIZE[15:0] */
	u32 dsc_hrd_delays;      /* INITIAL_DEC_DELAY[31:16] | INITIAL_XMIT_DELAY[9:0] */
	u32 dsc_rc_scale;
	u32 dsc_rc_scale_inc_dec;
	u32 dsc_rc_offsets_1;
	u32 dsc_rc_offsets_2;
	u32 dsc_rc_offsets_3;
	u32 dsc_flatness_qp;
	u32 dsc_rc_model_size;
	u32 dsc_rc_config;
	u32 dsc_rc_buf_thresh_0;
	u32 dsc_rc_buf_thresh_1;
	u32 dsc_rc_buf_thresh_2;
	u32 dsc_rc_buf_thresh_3;
	u32 dsc_rc_min_qp_0;
	u32 dsc_rc_min_qp_1;
	u32 dsc_rc_min_qp_2;
	u32 dsc_rc_max_qp_0;
	u32 dsc_rc_max_qp_1;
	u32 dsc_rc_max_qp_2;
	u32 dsc_rc_range_bpg_offsets_0;
	u32 dsc_rc_range_bpg_offsets_1;
	u32 dsc_rc_range_bpg_offsets_2;
};

/*
 * mdss_panel_ops - panel-agnostic interface for DSI display bringup.
 *
 * Each panel driver (dsi_panel_r66451.c, dsi_panel_sim_dualmipi.c, ...)
 * fills in this struct. disp.c calls through these ops without knowing
 * which panel is active - guaranteeing that adding a new panel cannot
 * break an existing one.
 *
 * edid_mode    : panel timing (ha, va, hbl, vbl, pixel_clock, etc.)
 * config       : MDP topology (pipe_count, dsc_ratio_x1000, intf_num, dsi_count)
 * dsc_params   : pre-calculated DSC RC register values
 * video_compression_ctrl  : DSI VIDEO_COMPRESSION_MODE_CTRL value
 * video_compression_ctrl2 : DSI VIDEO_COMPRESSION_MODE_CTRL2 value
 * get_init_cmds: returns PANEL_DCS/PANEL_DELAY/PANEL_END on-command array
 * power_on     : GPIO reset sequence + regulator enable
 * power_off    : GPIO reset low + regulator disable
 */
/*
 * PHY timing override: 14 bytes matching TIMING_CTRL_0..13.
 * If non-NULL, these values override the calculated timings.
 * Source: qcom,mdss-dsi-panel-phy-timings in panel DTSI.
 */
#define DSI_PHY_TIMING_SIZE 14

struct mdss_panel_ops {
	const struct edid_mode        *edid_mode;
	const struct mdss_panel_config *config;
	const struct mdss_dsc_params  *dsc_params;
	const struct mdss_dsc_config  *dsc_cfg;    /* DSC input config (for PPS build) */
	u32                            video_compression_ctrl;
	u32                            video_compression_ctrl2;
	const u8                      *(*get_init_cmds)(void);
	const u8                      *phy_timings; /* optional override, NULL = use calculated */
	u8                             clk_cfg0_override; /* 0 = use calculated; non-zero = use this value */
	u8                             clk_cfg1_override; /* 0 = use calculated; non-zero = use this value */
	u8                             traffic_mode;  /* DSI video traffic mode: 1=non-burst sync pulse (default), 2=burst */
	u8                             eotp_disabled; /* 0 = append EOT packet (default), 1 = disable EOT */
};

/* Returns the active panel ops - selected at compile time via Kconfig */
const struct mdss_panel_ops *mdss_get_panel_ops(void);

#endif /* _SOC_QUALCOMM_X1P42100_PANEL_OPS_H_ */
