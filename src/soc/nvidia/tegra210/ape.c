/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <delay.h>
#include <soc/addressmap.h>
#include <soc/clk_rst.h>
#include <soc/clock.h>
#include <soc/padconfig.h>
#include <soc/power.h>

static void enable_ape_periph_clocks(void)
{
	clock_enable(0, 0, 0, CLK_V_APB2APE, 0, 0, CLK_Y_APE);

	/* Give clocks time to stabilize. */
	udelay(IO_STABILIZATION_DELAY);
}

static void unreset_ape_periphs(void)
{
	clock_clr_reset(0, 0, 0, CLK_V_APB2APE, 0, 0, CLK_Y_APE);
}

/*
 * Audio on Tegra210 requires some special init.
 * The APE block must be unpowergated, and a couple of
 * audio-based peripherals must be clocked and taken
 * out of reset so that I2S/AXBAR/APB2APE registers can
 * be configured to enable audio flow.
 */

void soc_configure_ape(void)
{
	power_ungate_partition(POWER_PARTID_APE);

	enable_ape_periph_clocks();
	remove_clamps(POWER_PARTID_APE);
	unreset_ape_periphs();
}
