/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_X1P42100_DSI_PHY_PLL_H_
#define _SOC_QUALCOMM_X1P42100_DSI_PHY_PLL_H_

#include <types.h>

/*
 * DSI PHY 2.7.0 PLL VCO rate programming.
 * @vco_rate_hz:  VCO output frequency in Hz
 * @pll_post_div: PLL post-divider ratio (must be a power of 2)
 */
void dsi_phy_pll_vco_270_set_rate(unsigned long vco_rate_hz, u32 pll_post_div);

#endif /* _SOC_QUALCOMM_X1P42100_DSI_PHY_PLL_H_ */
