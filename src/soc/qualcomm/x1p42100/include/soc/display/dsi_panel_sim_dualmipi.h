/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_X1P42100_DSI_PANEL_SIM_DUALMIPI_H_
#define _SOC_QUALCOMM_X1P42100_DSI_PANEL_SIM_DUALMIPI_H_

#include <soc/display/panel_ops.h>
#include <types.h>

/* Sim dual-DSI DSC video panel (2-2-2 topology, 2880x2560 144Hz) */
const struct mdss_panel_ops *dsi_panel_sim_dualmipi_get_ops(void);
const u8 *dsi_panel_sim_dualmipi_get_init_cmds(void);

#endif /* _SOC_QUALCOMM_X1P42100_DSI_PANEL_SIM_DUALMIPI_H_ */
