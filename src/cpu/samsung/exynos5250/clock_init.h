/*
 * Clock initialization routines
 *
 * Copyright (c) 2011 The Chromium OS Authors.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __EXYNOS_CLOCK_INIT_H
#define __EXYNOS_CLOCK_INIT_H
#include "dmc.h"

/* These are the ratio's for configuring ARM clock */
struct arm_clk_ratios {
	unsigned int arm_freq_mhz;	/* Frequency of ARM core in MHz */

	unsigned int apll_mdiv;
	unsigned int apll_pdiv;
	unsigned int apll_sdiv;

	unsigned int arm2_ratio;
	unsigned int apll_ratio;
	unsigned int pclk_dbg_ratio;
	unsigned int atb_ratio;
	unsigned int periph_ratio;
	unsigned int acp_ratio;
	unsigned int cpud_ratio;
	unsigned int arm_ratio;
};

/**
 * Get the clock ratios for CPU configuration
 *
 * @return pointer to the clock ratios that we should use
 */
struct arm_clk_ratios *get_arm_clk_ratios(void);

/*
 * Initialize clock for the device
 */
struct mem_timings;
void system_clock_init(struct mem_timings *mem,
		struct arm_clk_ratios *arm_clk_ratio);
#endif
