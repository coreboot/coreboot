/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/dsi.h>

#define PHY_TIMING_DIV(x) (((x) / 8000) + 1)

void mtk_dsi_dphy_timing_calculation(u32 data_rate_mhz, struct mtk_phy_timing *timing)
{
	u32 temp;
	timing->lpx = ALIGN_UP(PHY_TIMING_DIV(80 * data_rate_mhz), 2);
	timing->da_hs_prepare = ALIGN_UP(PHY_TIMING_DIV(59 * data_rate_mhz + 4 * 1000), 2);
	timing->da_hs_zero = PHY_TIMING_DIV(163 * data_rate_mhz + 11 * 1000) -
			     timing->da_hs_prepare;

	temp = data_rate_mhz < 740 ? 443 : 420;
	timing->da_hs_trail = PHY_TIMING_DIV(66 * data_rate_mhz + temp * 100);
	timing->ta_go = 4 * timing->lpx;
	timing->ta_sure = 3 * timing->lpx / 2;
	timing->ta_get = 5 * timing->lpx;
	timing->da_hs_exit = ALIGN_UP(PHY_TIMING_DIV(118 * data_rate_mhz), 2);
	timing->da_hs_sync = 1;

	timing->clk_hs_prepare = ALIGN_UP(PHY_TIMING_DIV(57 * data_rate_mhz), 2);
	timing->clk_hs_post = PHY_TIMING_DIV(65 * data_rate_mhz + 53 * 1000);
	timing->clk_hs_trail = PHY_TIMING_DIV(65 * data_rate_mhz + 52 * 1000);
	timing->clk_hs_zero = PHY_TIMING_DIV(330 * data_rate_mhz) - timing->clk_hs_prepare;
	timing->clk_hs_exit = PHY_TIMING_DIV(118 * data_rate_mhz);
}
