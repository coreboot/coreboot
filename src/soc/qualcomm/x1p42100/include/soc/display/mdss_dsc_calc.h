/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_X1P42100_MDSS_DSC_CALC_H_
#define _SOC_QUALCOMM_X1P42100_MDSS_DSC_CALC_H_

#include <edid.h>
#include <soc/display/panel_ops.h>
#include <types.h>

/*
 * mdss_dsc_config - panel DSC configuration inputs (from DTSI/panel XML).
 * These are the only panel-specific inputs needed to calculate all DSC
 * register values dynamically.
 */
struct mdss_dsc_config {
	u8  dsc_version_minor; /* 1 = DSC 1.1, 2 = DSC 1.2 */
	u8  scr;               /* 0 = standard, 1 = SCR variant */
	u8  bpc;               /* bits per component (8 or 10) */
	u8  bpp;               /* bits per pixel output (8, 10, 12...) */
	u8  block_pred;        /* block prediction enable (0 or 1) */
	u8  multiplex_mode;    /* 1=2-2-1 (encoders merge), 0=2-2-2 (each to own INTF) */
	u32 slice_width;       /* DSC slice width in pixels (from DTSI) */
	u32 slice_height;      /* DSC slice height in lines (from DTSI) */
	u32 slices_per_pkt;    /* DSC slices per DSI packet (from DTSI) */
	u32 num_interfaces;    /* number of DSI interfaces (1=2-2-1, 2=2-2-2) */
	/*
	 * override initial_dec_delay (0=use profile table default)
	 * Depends on slice geometry - validate from golden dump
	 */
	u32 initial_dec_delay;
};

/*
 * picture_width/height are derived internally from ha/va + topology:
 *   2-2-1 (multiplex_mode=1): picture_width = ha,               picture_height = va
 *   2-2-2 (multiplex_mode=0): picture_width = ha*num_interfaces, picture_height = va
 * ha/va come from edid_mode - no redundant fields in mdss_dsc_config.
 * Source: UEFI MDPLib.c MDPSetupDSCProperty() + hal_mdp_dsc.c
 */

/*
 * mdss_dsc_calc() - calculate all DSC register values from panel config.
 * Ported from UEFI HAL_MDP_DSC_GetProperty() + HAL_MDP_DSC_ENC_Commit().
 * Returns CB_SUCCESS on success, CB_ERR if profile not found.
 */
enum cb_err mdss_dsc_calc(const struct mdss_dsc_config *cfg,
			  const struct edid_mode *mode,
			  struct mdss_dsc_params *out);

/*
 * mdss_dsc_calc_video_compression_ctrl() - calculate DSI VIDEO_COMPRESSION_MODE_CTRL.
 * Ported from UEFI dsiDriver.c + HALdsi_Config.c.
 */
u32 mdss_dsc_calc_video_compression_ctrl(const struct mdss_dsc_config *cfg,
					 const struct edid_mode *mode);

/*
 * mdss_dsc_build_pps() - build the 128-byte DSC PPS packet.
 * Ported from UEFI HAL_MDP_DSC_Prepare_PPS() in hal_mdp_dsc.c.
 * pps_buf must be a caller-allocated 128-byte buffer.
 * Send via DSI using MIPI_DSI_PPS_LONG_WRITE (data type 0x0A).
 */
void mdss_dsc_build_pps(const struct mdss_dsc_config *cfg,
			const struct edid_mode *mode,
			const struct mdss_dsc_params *p,
			u8 *pps_buf);

#endif /* _SOC_QUALCOMM_X1P42100_MDSS_DSC_CALC_H_ */
