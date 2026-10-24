/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_X1P42100_DSI_PANEL_R66451_H_
#define _SOC_QUALCOMM_X1P42100_DSI_PANEL_R66451_H_

#include <soc/display/mdssreg.h>
#include <soc/display/panel_ops.h>
#include <types.h>

/* Visionox r66451 AMOLED panel (1080x2340, 120Hz, 2-2-1 DSI, DSC 8bpc/8bpp) */

/* Returns pointer to PANEL_DCS/PANEL_DELAY/PANEL_END on-command array */
const u8 *dsi_panel_r66451_get_init_cmds(void);

/* Returns pointer to PANEL_DCS/PANEL_DELAY/PANEL_END off-command array */
const u8 *dsi_panel_r66451_get_poweroff_cmds(void);

/* Topology config for this panel (2-2-1 DSI, DSC 3:1) */
const struct mdss_panel_config *dsi_panel_r66451_get_config(void);

/* Full panel ops (edid_mode, config, dsc_params, init_cmds) */
const struct mdss_panel_ops *dsi_panel_r66451_get_ops(void);

#endif /* _SOC_QUALCOMM_X1P42100_DSI_PANEL_R66451_H_ */
