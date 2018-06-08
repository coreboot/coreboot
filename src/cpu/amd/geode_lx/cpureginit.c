/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Indrek Kruusa <indrek.kruusa@artecdesign.ee>
 * Copyright (C) 2006 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <spd.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/lxdef.h>
#include <northbridge/amd/lx/raminit.h>
#include <northbridge/amd/lx/northbridge.h>

/**
 * Delay Control Settings table from AMD (MCP 0x4C00000F).
 */
static const msrinit_t delay_msr_table[] = {
	{CPU_BC_MSS_ARRAY_CTL0, {.hi = 0x00000000, .lo = 0x2814D352}},
	{CPU_BC_MSS_ARRAY_CTL1, {.hi = 0x00000000, .lo = 0x1068334D}},
	{CPU_BC_MSS_ARRAY_CTL2, {.hi = 0x00000106, .lo = 0x83104104}},
};

static const struct delay_controls {
	u8 dimms;
	u8 devices;
	u32 slow_hi;
	u32 slow_low;
	u32 fast_hi;
	u32 fast_low;
} delay_control_table[] = {
	/* DIMMs Devs Slow (<=333MHz)            Fast (>334MHz) */
	{   1,     4, 0x0837100FF, 0x056960004,  0x0827100FF, 0x056960004 },
	{   1,     8, 0x0837100AA, 0x056960004,  0x0827100AA, 0x056960004 },
	{   1,    16, 0x0837100AA, 0x056960004,  0x082710055, 0x056960004 },
	{   2,     8, 0x0837100A5, 0x056960004,  0x082710000, 0x056960004 },
	{   2,    16, 0x0937100A5, 0x056960004,  0x0C27100A5, 0x056960004 },
	{   2,    20, 0x0B37100A5, 0x056960004,  0x0B27100A5, 0x056960004 },
	{   2,    24, 0x0B37100A5, 0x056960004,  0x0B27100A5, 0x056960004 },
	{   2,    32, 0x0B37100A5, 0x056960004,  0x0B2710000, 0x056960004 },
};

/*
 * Bit 55 (disable SDCLK 1,3,5) should be set if there is a single DIMM
 * in slot 0, but it should be clear for all 2 DIMM settings and if a
 * single DIMM is in slot 1. Bits 54:52 should always be set to '111'.
 *
 * Settings for single DIMM and no VTT termination (like DB800 platform)
 * 0xF2F100FF 0x56960004
 * -------------------------------------
 * ADDR/CTL have 22 ohm series R
 * DQ/DQM/DQS have 33 ohm series R
 */

/**
 * This is Black Magic DRAM timing juju[1].
 *
 * DRAM delay depends on CPU clock, memory bus clock, memory bus loading,
 * memory bus termination, your middle initial (ha! caught you!), GeodeLink
 * clock rate, and DRAM timing specifications.
 *
 * From this the code computes a number which is "known to work". No,
 * hardware is not an exact science. And, finally, if an FS2 (JTAG debugger)
 * is hooked up, then just don't do anything. This code was written by a master
 * of the Dark Arts at AMD and should not be modified in any way.
 *
 * [1] (http://www.thefreedictionary.com/juju)
 *
 * @param dimm0 The SMBus address of DIMM 0 (mainboard dependent).
 * @param dimm1 The SMBus address of DIMM 1 (mainboard dependent).
 * @param terminated The bus is terminated. (mainboard dependent).
 */
static void SetDelayControl(u8 dimm0, u8 dimm1, int terminated)
{
	u32 glspeed;
	u8 spdbyte0, spdbyte1, dimms, i;
	msr_t msr;

	glspeed = GeodeLinkSpeed();

	/* Fix delay controls for DM and IM arrays. */
	for (i = 0; i < ARRAY_SIZE(delay_msr_table); i++)
		wrmsr(delay_msr_table[i].index, delay_msr_table[i].msr);

	msr = rdmsr(GLCP_FIFOCTL);
	msr.hi = 0x00000005;
	wrmsr(GLCP_FIFOCTL, msr);

	/* Enable setting. */
	msr.hi = 0;
	msr.lo = 0x00000001;
	wrmsr(CPU_BC_MSS_ARRAY_CTL_ENA, msr);

	/* Debug Delay Control setup check.
	 * Leave it alone if it has been setup. FS2 or something is here.
	 */
	msr = rdmsr(GLCP_DELAY_CONTROLS);
	if (msr.lo & ~(DELAY_LOWER_STATUS_MASK))
		return;

	/* Delay Controls based on DIMM loading. UGH!
	 * Number of devices = module width (SPD 6) / device width (SPD 13)
	 *                     * physical banks (SPD 5)
	 *
	 * Note: We only support a module width of 64.
	 */
	dimms = 0;
	spdbyte0 = spd_read_byte(dimm0, SPD_PRIMARY_SDRAM_WIDTH);
	if (spdbyte0 != 0xFF) {
		dimms++;
		spdbyte0 = (u8)64 / spdbyte0 *
			   (u8)(spd_read_byte(dimm0, SPD_NUM_DIMM_BANKS));
	} else {
		spdbyte0 = 0;
	}

	spdbyte1 = spd_read_byte(dimm1, SPD_PRIMARY_SDRAM_WIDTH);
	if (spdbyte1 != 0xFF) {
		dimms++;
		spdbyte1 = (u8)64 / spdbyte1 *
			   (u8)(spd_read_byte(dimm1, SPD_NUM_DIMM_BANKS));
	} else {
		spdbyte1 = 0;
	}

	/* Zero GLCP_DELAY_CONTROLS MSR */
	msr.hi = msr.lo = 0;

	/* Save some power, disable clock to second DIMM if it is empty. */
	if (spdbyte1 == 0)
		msr.hi |= DELAY_UPPER_DISABLE_CLK135;

	spdbyte0 += spdbyte1;

	if ((dimms == 1) && (terminated == DRAM_TERMINATED)) {
		msr.hi = 0xF2F100FF;
		msr.lo = 0x56960004;
	} else for (i = 0; i < ARRAY_SIZE(delay_control_table); i++) {
		if ((dimms == delay_control_table[i].dimms) &&
		    (spdbyte0 <= delay_control_table[i].devices)) {
			if (glspeed < 334) {
				msr.hi |= delay_control_table[i].slow_hi;
				msr.lo |= delay_control_table[i].slow_low;
			} else {
				msr.hi |= delay_control_table[i].fast_hi;
				msr.lo |= delay_control_table[i].fast_low;
			}
			break;
		}
	}
	wrmsr(GLCP_DELAY_CONTROLS, msr);
}

void cpuRegInit(int debug_clock_disable, u8 dimm0, u8 dimm1, int terminated)
{
	int msrnum;
	msr_t msr;

	/* Castle 2.0 BTM periodic sync period. */
	/*      [40:37] 1 sync record per 256 bytes */
	printk(BIOS_DEBUG, "Castle 2.0 BTM periodic sync period.\n");
	msrnum = CPU_PF_CONF;
	msr = rdmsr(msrnum);
	msr.hi |= (0x8 << 5);
	wrmsr(msrnum, msr);

	/*
	 * LX performance setting.
	 * Enable Quack for fewer re-RAS on the MC
	 */
	printk(BIOS_DEBUG, "Enable Quack for fewer re-RAS on the MC\n");
	msrnum = GLIU0_ARB;
	msr = rdmsr(msrnum);
	msr.hi &= ~ARB_UPPER_DACK_EN_SET;
	msr.hi |= ARB_UPPER_QUACK_EN_SET;
	wrmsr(msrnum, msr);

	msrnum = GLIU1_ARB;
	msr = rdmsr(msrnum);
	msr.hi &= ~ARB_UPPER_DACK_EN_SET;
	msr.hi |= ARB_UPPER_QUACK_EN_SET;
	wrmsr(msrnum, msr);

	/* GLIU port active enable, limit south pole masters
	 * (AES and PCI) to one outstanding transaction.
	 */
	printk(BIOS_DEBUG, " GLIU port active enable\n");
	msrnum = GLIU1_PORT_ACTIVE;
	msr = rdmsr(msrnum);
	msr.lo &= ~0x880;
	wrmsr(msrnum, msr);

	/* Set the Delay Control in GLCP */
	printk(BIOS_DEBUG, "Set the Delay Control in GLCP\n");
	SetDelayControl(dimm0, dimm1, terminated);

	/*  Enable RSDC */
	printk(BIOS_DEBUG, "Enable RSDC\n");
	msrnum = CPU_AC_SMM_CTL;
	msr = rdmsr(msrnum);
	msr.lo |= SMM_INST_EN_SET;
	wrmsr(msrnum, msr);

	/* FPU imprecise exceptions bit */
	printk(BIOS_DEBUG, "FPU imprecise exceptions bit\n");
	msrnum = CPU_FPU_MSR_MODE;
	msr = rdmsr(msrnum);
	msr.lo |= FPU_IE_SET;
	wrmsr(msrnum, msr);

	/* Power Savers (Do after BIST) */
	/* Enable Suspend on HLT & PAUSE instructions */
	printk(BIOS_DEBUG, "Enable Suspend on HLT & PAUSE instructions\n");
	msrnum = CPU_XC_CONFIG;
	msr = rdmsr(msrnum);
	msr.lo |= XC_CONFIG_SUSP_ON_HLT | XC_CONFIG_SUSP_ON_PAUSE;
	wrmsr(msrnum, msr);

	/* Enable SUSP and allow TSC to run in Suspend (keep speed detection happy) */
	printk(BIOS_DEBUG, "Enable SUSP and allow TSC to run in Suspend\n");
	msrnum = CPU_BC_CONF_0;
	msr = rdmsr(msrnum);
	msr.lo |= TSC_SUSP_SET | SUSP_EN_SET;
	msr.lo &= 0x0F0FFFFFF;
	msr.lo |= 0x002000000;	/* PBZ213: Set PAUSEDLY = 2 */
	wrmsr(msrnum, msr);

	/* Disable the debug clock to save power. */
	/* NOTE: leave it enabled for fs2 debug */
	if (debug_clock_disable && 0) {
		msrnum = GLCP_DBGCLKCTL;
		msr.hi = 0;
		msr.lo = 0;
		wrmsr(msrnum, msr);
	}

	/* Setup throttling delays to proper mode if it is ever enabled. */
	printk(BIOS_DEBUG, "Setup throttling delays to proper mode\n");
	msrnum = GLCP_TH_OD;
	msr.hi = 0;
	msr.lo = 0x00000603C;
	wrmsr(msrnum, msr);
	printk(BIOS_DEBUG, "Done cpuRegInit\n");
}
