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

enum {
	MEM_TIMINGS_MSR_COUNT	= 4,
};

/* These are the ratio's for configuring ARM clock */
struct arm_clk_ratios {
	unsigned arm_freq_mhz;		/* Frequency of ARM core in MHz */

	unsigned apll_mdiv;
	unsigned apll_pdiv;
	unsigned apll_sdiv;

	unsigned arm2_ratio;
	unsigned apll_ratio;
	unsigned pclk_dbg_ratio;
	unsigned atb_ratio;
	unsigned periph_ratio;
	unsigned acp_ratio;
	unsigned cpud_ratio;
	unsigned arm_ratio;
};

/* These are the memory timings for a particular memory type and speed */
struct mem_timings {
	enum mem_manuf mem_manuf;	/* Memory manufacturer */
	enum ddr_mode mem_type;		/* Memory type */
	unsigned frequency_mhz;		/* Frequency of memory in MHz */

	/* Here follow the timing parameters for the selected memory */
	uint8_t apll_mdiv;
	uint8_t apll_pdiv;
	uint8_t apll_sdiv;
	uint8_t mpll_mdiv;
	uint8_t mpll_pdiv;
	uint8_t mpll_sdiv;
	uint8_t cpll_mdiv;
	uint8_t cpll_pdiv;
	uint8_t cpll_sdiv;
	uint8_t gpll_pdiv;
	uint16_t gpll_mdiv;
	uint8_t gpll_sdiv;
	uint8_t epll_mdiv;
	uint8_t epll_pdiv;
	uint8_t epll_sdiv;
	uint8_t vpll_mdiv;
	uint8_t vpll_pdiv;
	uint8_t vpll_sdiv;
	uint8_t bpll_mdiv;
	uint8_t bpll_pdiv;
	uint8_t bpll_sdiv;
	uint8_t use_bpll;       /* 1 to use BPLL for cdrex, 0 to use MPLL */
	uint8_t pclk_cdrex_ratio;
	unsigned direct_cmd_msr[MEM_TIMINGS_MSR_COUNT];

	unsigned timing_ref;
	unsigned timing_row;
	unsigned timing_data;
	unsigned timing_power;

	/* DQS, DQ, DEBUG offsets */
	unsigned phy0_dqs;
	unsigned phy1_dqs;
	unsigned phy0_dq;
	unsigned phy1_dq;
	uint8_t phy0_tFS;
	uint8_t phy1_tFS;
	uint8_t phy0_pulld_dqs;
	uint8_t phy1_pulld_dqs;

	uint8_t lpddr3_ctrl_phy_reset;
	uint8_t ctrl_start_point;
	uint8_t ctrl_inc;
	uint8_t ctrl_start;
	uint8_t ctrl_dll_on;
	uint8_t ctrl_ref;

	uint8_t ctrl_force;
	uint8_t ctrl_rdlat;
	uint8_t ctrl_bstlen;

	uint8_t fp_resync;
	uint8_t iv_size;
	uint8_t dfi_init_start;
	uint8_t aref_en;

	uint8_t rd_fetch;

	uint8_t zq_mode_dds;
	uint8_t zq_mode_term;
	uint8_t zq_mode_noterm;	/* 1 to allow termination disable */

	unsigned memcontrol;
	unsigned memconfig;

	unsigned membaseconfig0;
	unsigned membaseconfig1;
	unsigned prechconfig_tp_cnt;
	unsigned dpwrdn_cyc;
	unsigned dsref_cyc;
	unsigned concontrol;
	/* Channel and Chip Selection */
	uint8_t dmc_channels;		/* number of memory channels */
	uint8_t chips_per_channel;	/* number of chips per channel */
	uint8_t chips_to_configure;	/* number of chips to configure */
	uint8_t send_zq_init;		/* 1 to send this command */
	unsigned impedance;		/* drive strength impedeance */
	uint8_t gate_leveling_enable;	/* check gate leveling is enabled */
};

/**
 * Get the correct memory timings for our selected memory type and speed.
 *
 * This function can be called from SPL or the main U-Boot.
 *
 * @return pointer to the memory timings that we should use
 */
struct mem_timings *clock_get_mem_timings(void);

/*
 * Initialize clock for the device
 */
void system_clock_init(void);
#endif
