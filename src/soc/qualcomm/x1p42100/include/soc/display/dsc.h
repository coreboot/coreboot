/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_X1P42100_DSC_H_
#define _SOC_QUALCOMM_X1P42100_DSC_H_

#include <soc/display/panel_ops.h>

/*
 * Configure DSC engine using panel-specific DSC params.
 * Must be called after merge_3d is enabled and before DSI video mode starts.
 */
void mdss_dsc_config(const struct mdss_dsc_params *p);

#endif /* _SOC_QUALCOMM_X1P42100_DSC_H_ */
