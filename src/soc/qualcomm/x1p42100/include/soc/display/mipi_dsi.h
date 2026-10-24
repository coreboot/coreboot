/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_X1P42100_MIPI_DSI_H_
#define _SOC_QUALCOMM_X1P42100_MIPI_DSI_H_

#include <edid.h>
#include <types.h>

/* Forward declarations for PPS types (defined in mdss_dsc_calc.h) */
struct mdss_dsc_config;
struct mdss_dsc_params;

#define DSI_VIDEO_DST_FORMAT_RGB565      0
#define DSI_VIDEO_DST_FORMAT_RGB666      1
#define DSI_VIDEO_DST_FORMAT_RGB666_LOOSE 2
#define DSI_VIDEO_DST_FORMAT_RGB888      3

/*
 * DSI PPS data type (DSI_DATATYPE_PPSPACKET = 0x0A).
 * Source: UEFI HALdsi.h DSI_DATATYPE_PPSPACKET.
 * Used to send the 128-byte DSC Picture Parameter Set to the panel.
 */
#define MIPI_DSI_PPS_LONG_WRITE  0x0A

/* DSI0 (primary) */
enum cb_err mdss_dsi_config(struct edid *edid, u32 num_of_lanes, u32 bpp);
void mdss_dsi_clock_config(void);
void mdss_dsi_video_mode_config(struct edid *edid, u32 bpp);
enum cb_err mdss_dsi_panel_initialize(const u8 *init_cmds);

/* DSI1 (secondary - dual DSI 2-2-2 topology) */
enum cb_err mdss_dsi1_config(struct edid *edid, u32 num_of_lanes, u32 bpp);
void mdss_dsi1_video_mode_config(struct edid *edid, u32 bpp);

/* Full DSI+DSC bring-up */
enum cb_err mdss_dsi_display_init(struct edid *edid_out, void (*power_on)(void));
void mdss_dsi_display_stop(void);

/*
 * mdss_dsi_send_pps() - send DSC PPS command to panel.
 * Builds 128-byte PPS from DSC params and sends via DSI DMA (type 0x0A).
 * Must be called after mdss_dsc_config() and before mdss_dsi_panel_initialize().
 */
enum cb_err mdss_dsi_send_pps(const struct mdss_dsc_config *cfg,
			      const struct edid_mode *mode,
			      const struct mdss_dsc_params *params);

#endif /* _SOC_QUALCOMM_X1P42100_MIPI_DSI_H_ */
