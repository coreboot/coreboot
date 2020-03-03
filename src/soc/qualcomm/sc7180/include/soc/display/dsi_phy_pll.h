/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_DISPLAY_DSI_PHY_PLL_H
#define _SOC_DISPLAY_DSI_PHY_PLL_H

#include <types.h>

enum {
	MDSS_DSI_PLL_10NM,
	MDSS_UNKNOWN_PLL,
};

struct mdss_pll_vco_calc {
	s32 div_frac_start1;
	s32 div_frac_start2;
	s32 div_frac_start3;
	s64 dec_start1;
	s64 dec_start2;
	s64 pll_plllock_cmp1;
	s64 pll_plllock_cmp2;
	s64 pll_plllock_cmp3;
};

void dsi_phy_pll_vco_10nm_set_rate(unsigned long rate);

#endif
