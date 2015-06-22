/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */
#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <lib.h>
#include <stdlib.h>

#include <soc/addressmap.h>
#include <soc/clock.h>
#include <device/device.h>
#include <soc/nvidia/tegra/types.h>
#include <soc/display.h>
#include <soc/mipi_dsi.h>
#include <soc/mipi_display.h>
#include <soc/tegra_dsi.h>
#include <soc/mipi-phy.h>

int mipi_dphy_set_timing(struct tegra_dsi *dsi)
{

	u32 freq = (dsi->clk_rate * 2) / 1000000;

	u32 thsdexit = (DSI_PHY_TIMING_DIV(120, (freq)));
	u32 thstrial = (((3) + (DSI_PHY_TIMING_DIV((DSI_THSTRAIL_VAL(freq)),
			freq))));
	u32 tdatzero = DSI_PHY_TIMING_DIV(((145) + (5 * (DSI_TBIT(freq)))),
			(freq));
	u32 thsprepare = DSI_PHY_TIMING_DIV((65 + (5*(DSI_TBIT(freq)))), freq);
	u32 tclktrial = (DSI_PHY_TIMING_DIV(80, freq));
	u32 tclkpost = ((DSI_PHY_TIMING_DIV(((70) + ((52) * (DSI_TBIT(freq)))),
			freq)));
	u32 tclkzero = (DSI_PHY_TIMING_DIV(260, freq));
	u32 ttlpx = (DSI_PHY_TIMING_DIV(60, freq)) ;
	u32 tclkprepare = (DSI_PHY_TIMING_DIV(60, freq));
	u32 tclkpre = 1; //min = 8*UI per mipi spec, tclk_pre=0 should be ok, but using 1 value
	u32 twakeup = 0x7F; //min = 1ms

	u32 ttaget;
	u32 ttassure;
	u32 ttago;
	u32 value;

	if (!ttlpx) {
		ttaget = 5;
		ttassure = 2;
		ttago = 4;
	} else {
		ttaget = 5 * ttlpx;
		ttassure = 2 * ttlpx;
		ttago = 4 * ttlpx;
	}

	value = (thsdexit << 24) |
		(thstrial << 16) |
		(tdatzero << 8) |
		(thsprepare << 0);
	tegra_dsi_writel(dsi, value, DSI_PHY_TIMING_0);

	value = (tclktrial << 24) |
		(tclkpost << 16)  |
		(tclkzero << 8) |
		(ttlpx << 0);
	tegra_dsi_writel(dsi, value, DSI_PHY_TIMING_1);

	value = (tclkprepare << 16) |
		(tclkpre << 8) |
		(twakeup << 0);
	tegra_dsi_writel(dsi, value, DSI_PHY_TIMING_2);

	value = (ttaget << 16) |
		(ttassure << 8) |
		(ttago << 0),
	tegra_dsi_writel(dsi, value, DSI_BTA_TIMING);
	return 0;
}
