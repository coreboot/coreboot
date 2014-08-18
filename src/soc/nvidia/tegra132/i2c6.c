/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2014, NVIDIA CORPORATION.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/padconfig.h>
#include <soc/nvidia/tegra/i2c.h>
#include <soc/nvidia/tegra132/power.h>
#include <soc/nvidia/tegra132/clk_rst.h>
#include "delay.h"

#define I2C6_BUS		5
#define I2C6_PADCTL		0xC001
#define DPAUX_HYBRID_PADCTL	0x545C0124

static struct tegra_pmc_regs * const pmc = (void *)TEGRA_PMC_BASE;
static struct clk_rst_ctlr *clk_rst = (void *)TEGRA_CLK_RST_BASE;

static int partition_clamp_on(int id)
{
	return read32(&pmc->clamp_status) & (1 << id);
}

static void remove_clamps(int id)
{
	if (!partition_clamp_on(id))
		return;

	/* Remove clamp */
	write32((1 << id), &pmc->remove_clamping_cmd);

	/* Wait for clamp off */
	while (partition_clamp_on(id))
		;
}

static void enable_sor_periphs(void)
{
	u32 lclks = CLK_L_HOST1X;
	u32 hclks = CLK_H_MIPI_CAL | CLK_H_HDMI | CLK_H_DSI;
	u32 uclks = CLK_U_DSIB;
	u32 wclks = CLK_W_DP2 | CLK_W_HDA2HDMICODEC;
	u32 xclks = CLK_X_DPAUX | CLK_X_SOR0 | CLK_X_HDMI_AUDIO;

	clock_enable(lclks, hclks, uclks, 0, wclks, xclks);

	/* Give clocks time to stabilize. */
	udelay(IO_STABILIZATION_DELAY);
}

static void unreset_sor_periphs(void)
{
	u32 lclks = CLK_L_HOST1X;
	u32 hclks = CLK_H_MIPI_CAL | CLK_H_HDMI | CLK_H_DSI;
	u32 uclks = CLK_U_DSIB;
	u32 wclks = CLK_W_DP2 | CLK_W_HDA2HDMICODEC;
	u32 xclks = CLK_X_DPAUX | CLK_X_SOR0 | CLK_X_HDMI_AUDIO;

	clock_clear_reset(lclks, hclks, uclks, 0, wclks, xclks);
}

void soc_configure_i2c6pad(void)
{
	/*
	 * I2C6 on Tegra124/132 requires some special init.
	 * The SOR block must be unpowergated, and several
	 * display-based peripherals must be clocked and taken
	 * out of reset so that a DPAUX register can be
	 * configured to enable the I2C6 mux routing.
	 */
	power_ungate_partition(POWER_PARTID_SOR);
	enable_sor_periphs();
	remove_clamps(POWER_PARTID_SOR);
	unreset_sor_periphs();

	/* Host1X needs a valid clock source so DPAUX can be accessed */
	clock_configure_source(host1x, PLLP, 204000);

	/* Now we can write the I2C6 mux in DPAUX */
	write32(I2C6_PADCTL, (void *)DPAUX_HYBRID_PADCTL);
}
