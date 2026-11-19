/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_X1P42100_DISP_H_
#define _SOC_QUALCOMM_X1P42100_DISP_H_

#include <soc/display/panel_ops.h>
#include <stddef.h>

#if CONFIG(MIPI_PANEL_TIANMA_ILITEK79900A)
#include <soc/display/dsi_panel_ilitek79900a.h>
static inline const struct mdss_panel_ops *mdss_get_active_panel_ops(void)
{
	return dsi_panel_ilitek79900a_get_ops();
}
#elif CONFIG(MIPI_PANEL_VIS_R66451_CPHY_DSI1)
#include <soc/display/dsi_panel_r66451_cphy_dsi1.h>
static inline const struct mdss_panel_ops *mdss_get_active_panel_ops(void)
{
	return dsi_panel_r66451_cphy_dsi1_get_ops();
}
#elif CONFIG(MIPI_PANEL_BOE_NS130070_M00)
#include <soc/display/dsi_panel_nt36536.h>
static inline const struct mdss_panel_ops *mdss_get_active_panel_ops(void)
{
	return dsi_panel_nt36536_get_ops();
}
#elif CONFIG(MIPI_PANEL_VIS_R66451)
#include <soc/display/dsi_panel_r66451.h>
static inline const struct mdss_panel_ops *mdss_get_active_panel_ops(void)
{
	return dsi_panel_r66451_get_ops();
}
#elif CONFIG(MIPI_PANEL_SIM_DUALMIPI)
#include <soc/display/dsi_panel_sim_dualmipi.h>
static inline const struct mdss_panel_ops *mdss_get_active_panel_ops(void)
{
	return dsi_panel_sim_dualmipi_get_ops();
}
#else
static inline const struct mdss_panel_ops *mdss_get_active_panel_ops(void)
{
	return NULL;
}
#endif

#endif /* _SOC_QUALCOMM_X1P42100_DISP_H_ */
