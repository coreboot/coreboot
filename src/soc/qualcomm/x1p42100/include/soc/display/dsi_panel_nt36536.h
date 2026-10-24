/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_X1P42100_DSI_PANEL_NT36536_H_
#define _SOC_QUALCOMM_X1P42100_DSI_PANEL_NT36536_H_

#include <soc/display/panel_ops.h>
#include <types.h>

/* Novatek NT36536 BOE NS130070-M00 AMOLED (3504x2190, 120Hz, 2-2-2 DSI) */
const struct mdss_panel_ops *dsi_panel_nt36536_get_ops(void);
const u8 *dsi_panel_nt36536_get_init_cmds(void);

#endif /* _SOC_QUALCOMM_X1P42100_DSI_PANEL_NT36536_H_ */
