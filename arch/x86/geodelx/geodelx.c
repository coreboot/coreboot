/*
 * This file is part of the LinuxBIOS project.
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <lib.h>
#include <console.h>
#include <msr.h>
#include <io.h>
#include <cpu.h>
#include <amd_geodelx.h>
#include <spd.h>
#include <legacy.h>

/* All these functions used to be in a lot of fiddly little files. To make it
 * easier to find functions, we are merging them here. This file is our first
 * real CPU-specific support file and should serve as a model for v3
 * CPU-specific support.
 *
 * So, warning, you might think it makes sense to split this file up, but
 * we've tried that, and it sucks.
 */

/**
 * Starts Timer 1 for port 61 use.
 *
 * The command 0x56 means write counter 1 lower 8 bits in next I/O, set the
 * counter mode to square wave generator (count down to 0 from programmed
 * value twice in a row, alternating the output signal) counting in 16-bit
 * binary mode.
 *
 * 0x12 is counter/timer 1 and signals the PIT to do a RAM refresh
 * approximately every 15us.
 *
 * The PIT typically is generating 1.19318 MHz.
 *
 * Timer 1 was used for RAM refresh on XT/AT and can be read on port 61.
 * Port 61 is used by many timing loops for calibration.
 */
void start_timer1(void)
{
	outb(0x56, I82C54_CONTROL_WORD_REGISTER);
	outb(0x12, I82C54_COUNTER1);
}

/**
 * Very early initialization needed for almost everything else.
 * Currently, all we do is start timer1.
 */
void system_preinit(void)
{
	start_timer1();
}

/* CPU bug management */

/**
 * Bugtool #465 and #609 PCI cache deadlock.
 * TODO: URL?
 *
 * There is also fix code in cache and PCI functions. This bug is very is
 * pervasive.
 */
static void pci_deadlock(void)
{
	struct msr msr;

	/* Forces serialization of all load misses. Setting this bit prevents
	 * the DM pipe from backing up if a read request has to be held up
	 * waiting for PCI writes to complete.
	 */
	msr = rdmsr(CPU_DM_CONFIG0);
	msr.lo |= DM_CONFIG0_LOWER_MISSER_SET;
	wrmsr(CPU_DM_CONFIG0, msr);

	/* Write serialize memory hole to PCI. Need to unWS when something is
	 * shadowed regardless of cachablility.
	 */
	msr.lo = 0x021212121;
	msr.hi = 0x021212121;
	wrmsr(CPU_RCONF_A0_BF, msr);
	wrmsr(CPU_RCONF_C0_DF, msr);
	wrmsr(CPU_RCONF_E0_FF, msr);
}

/**
 * PBZ 3659: The MC reordered transactions incorrectly and breaks coherency.
 *
 * Disable reording and take a potential performance hit. This is safe to do
 * here and not in MC init, since there is nothing to maintain coherency with
 * and the cache is not enabled yet.
 */
static void disable_memory_reorder(void)
{
	struct msr msr;

	msr = rdmsr(MC_CF8F_DATA);
	msr.hi |= CF8F_UPPER_REORDER_DIS_SET;
	wrmsr(MC_CF8F_DATA, msr);
}

/**
 * Fix up register settings to manage known CPU bugs.
 *
 * For CPU version C3. Should be the only released version.
 */
void cpu_bug(void)
{
	pci_deadlock();
	disable_memory_reorder();
	printk(BIOS_DEBUG, "Done cpubug fixes\n");
}

/**
 * Reset the phase locked loop (PLL) hardware.
 *
 * After power on as part of this operation, we have to set the clock
 * hardware and reboot. Thus, we have to know if we have been here before.
 *
 * To do this, we use the RSTPLL_LOWER_SWFLAGS_SHIFT flag in the
 * msrGlcpSysRstpll. Also, the clocks can either be configured via passed-in
 * parameters or hardware straps. Once set, we yank the hardware reset line
 * and hlt. We should never reach the hlt, but one never knows.
 *
 * @param manualconf If non-zero, use passed-in parameters to determine how
 *                   to configure PLL -- manual or automagic. If manual, use
 *                   passed-in parameters pll_hi and pll_lo.
 * @param pll_hi Value to use for the high 32 bits of the PLL msr.
 * @param pll_lo Value to use for the low 32 bits of the PLL msr.
 */
void pll_reset(int manualconf, u32 pll_hi, u32 pll_lo)
{
	struct msr msr_glcp_sys_pll;	/* GeodeLink PLL control MSR */

	msr_glcp_sys_pll = rdmsr(GLCP_SYS_RSTPLL);

	printk(BIOS_DEBUG,
	       "_MSR GLCP_SYS_RSTPLL (%08x) value is: %08x:%08x\n",
	       msr_glcp_sys_pll.hi, msr_glcp_sys_pll.lo);
	post_code(POST_PLL_INIT);

	if (!(msr_glcp_sys_pll.lo & (1 << RSTPLL_LOWER_SWFLAGS_SHIFT))) {
		printk(BIOS_DEBUG, "Configuring PLL\n");
		if (manualconf) {
			post_code(POST_PLL_MANUAL);
			/* CPU and GLIU mult/div (GLMC_CLK = GLIU_CLK / 2) */
			msr_glcp_sys_pll.hi = pll_hi;

			/* Hold Count - how long we will sit in reset */
			msr_glcp_sys_pll.lo = pll_lo;
		} else {
			/* Automatic configuration (straps) */
			post_code(POST_PLL_STRAP);

			/* Hold 0xDE * 16 clocks during reset. AMD recomended
			 * value for PLL reset from silicon validation.
			 */
			msr_glcp_sys_pll.lo &=
			    ~(0xFF << RSTPPL_LOWER_HOLD_COUNT_SHIFT);
			msr_glcp_sys_pll.lo |=
			    (0xDE << RSTPPL_LOWER_HOLD_COUNT_SHIFT);
			msr_glcp_sys_pll.lo &=
			    ~(RSTPPL_LOWER_COREBYPASS_SET |
			      RSTPPL_LOWER_MBBYPASS_SET);
			msr_glcp_sys_pll.lo |=
			    RSTPPL_LOWER_COREPD_SET | RSTPPL_LOWER_CLPD_SET;
		}

		/* Use SWFLAGS to remember: "we've already been here". */
		msr_glcp_sys_pll.lo |= (1 << RSTPLL_LOWER_SWFLAGS_SHIFT);

		/* "Reset the chip" value */
		msr_glcp_sys_pll.lo |= RSTPPL_LOWER_CHIP_RESET_SET;
		wrmsr(GLCP_SYS_RSTPLL, msr_glcp_sys_pll);

		/* You should never get here... the chip has reset. */
		printk(BIOS_EMERG, "CONFIGURING PLL FAILURE -- HALT\n");
		post_code(POST_PLL_RESET_FAIL);
		hlt();
	}

	printk(BIOS_DEBUG, "Done pll_reset\n");
	return;
}

/**
 * Return the CPU clock rate from the PLL MSR.
 *
 * @return CPU speed in MHz.
 */
u32 cpu_speed(void)
{
	u32 speed;
	struct msr msr;

	msr = rdmsr(GLCP_SYS_RSTPLL);
	speed = ((((msr.hi >> RSTPLL_UPPER_CPUMULT_SHIFT)
		  & RSTPLL_UPPER_CPUMULT_MASK) + 1) * 333) / 10;
	if ((((((msr.hi >> RSTPLL_UPPER_CPUMULT_SHIFT)
		& RSTPLL_UPPER_CPUMULT_MASK) + 1) * 333) % 10) > 5) {
		++speed;
	}
	return speed;
}

/**
 * Return the GeodeLink clock rate from the PLL MSR.
 *
 * @return GeodeLink speed in MHz.
 */
u32 geode_link_speed(void)
{
	u32 speed;
	struct msr msr;

	msr = rdmsr(GLCP_SYS_RSTPLL);
	speed = ((((msr.hi >> RSTPLL_UPPER_GLMULT_SHIFT)
		   & RSTPLL_UPPER_GLMULT_MASK) + 1) * 333) / 10;
	if ((((((msr.hi >> RSTPLL_UPPER_GLMULT_SHIFT)
		& RSTPLL_UPPER_GLMULT_MASK) + 1) * 333) % 10) > 5) {
		++speed;
	}
	return speed;
}

/**
 * Return the PCI bus clock rate from the PLL MSR.
 *
 * @return PCI speed in MHz.
 */
u32 pci_speed(void)
{
	struct msr msr = rdmsr(GLCP_SYS_RSTPLL);

	if (msr.hi & (1 << RSTPPL_LOWER_PCISPEED_SHIFT))
		return 66;
	else
		return 33;
}

/**
 * Delay Control Settings table from AMD (MCP 0x4C00000F).
 */
const struct delay_controls {
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
 */
void set_delay_control(u8 dimm0, u8 dimm1)
{
	u32 msrnum, glspeed;
	u8 spdbyte0, spdbyte1, dimms, i;
	struct msr msr;

	glspeed = geode_link_speed();

	/* Fix delay controls for DM and IM arrays. */
	msrnum = CPU_BC_MSS_ARRAY_CTL0;
	msr.hi = 0;
	msr.lo = 0x2814D352;
	wrmsr(msrnum, msr);

	msrnum = CPU_BC_MSS_ARRAY_CTL1;
	msr.hi = 0;
	msr.lo = 0x1068334D;
	wrmsr(msrnum, msr);

	msrnum = CPU_BC_MSS_ARRAY_CTL2;
	msr.hi = 0x00000106;
	msr.lo = 0x83104104;
	wrmsr(msrnum, msr);

	msrnum = GLCP_FIFOCTL;
	msr = rdmsr(msrnum);
	msr.hi = 0x00000005;
	wrmsr(msrnum, msr);

	/* Enable setting. */
	msrnum = CPU_BC_MSS_ARRAY_CTL_ENA;
	msr.hi = 0;
	msr.lo = 0x00000001;
	wrmsr(msrnum, msr);

	/* Debug Delay Control setup check.
	 * Leave it alone if it has been setup. FS2 or something is here.
	 */
	msrnum = GLCP_DELAY_CONTROLS;
	msr = rdmsr(msrnum);
	if (msr.lo & ~(DELAY_LOWER_STATUS_MASK))
		return;

	/* Delay Controls based on DIMM loading. UGH!
	 * Number of devices = module width (SPD 6) / device width (SPD 13)
	 *                     * physical banks (SPD 5)
	 *
	 * Note: We only support a module width of 64.
	 */
	dimms = 0;
	spdbyte0 = smbus_read_byte(dimm0, SPD_PRIMARY_SDRAM_WIDTH);
	if (spdbyte0 != 0xFF) {
		dimms++;
		spdbyte0 = (u8)64 / spdbyte0 *
			   (u8)(smbus_read_byte(dimm0, SPD_NUM_DIMM_BANKS));
	} else {
		spdbyte0 = 0;
	}

	spdbyte1 = smbus_read_byte(dimm1, SPD_PRIMARY_SDRAM_WIDTH);
	if (spdbyte1 != 0xFF) {
		dimms++;
		spdbyte1 = (u8)64 / spdbyte1 *
			   (u8)(smbus_read_byte(dimm1, SPD_NUM_DIMM_BANKS));
	} else {
		spdbyte1 = 0;
	}

	/* Zero GLCP_DELAY_CONTROLS MSR */
	msr.hi = msr.lo = 0;

	/* Save some power, disable clock to second DIMM if it is empty. */
	if (spdbyte1 == 0)
		msr.hi |= DELAY_UPPER_DISABLE_CLK135;

	spdbyte0 += spdbyte1;

	for (i = 0; i < ARRAY_SIZE(delay_control_table); i++) {
		if ((dimms == delay_control_table[i].dimms) &&
		    (spdbyte0 <= delay_control_table[i].devices)) {
			if (glspeed < 334) {
				msr.hi |= delay_control_table[i].slow_hi;
				msr.lo |= delay_control_table[i].slow_low;
			} else {
				msr.hi |= delay_control_table[i].fast_hi;
				msr.lo |= delay_control_table[i].fast_low;
			}
		}
	}
	wrmsr(GLCP_DELAY_CONTROLS, msr);
	return;
}

/**
 * All CPU register settings, here in one place, and done in the proper order.
 *
 * @param debug_clock_disable Disable the debug clock to save power. Currently
 *                            ignored, but we need to pick this up from a CMOS
 *                            setting in future.
 * @param dimm0 SMBus address of DIMM 0 (mainboard dependent).
 * @param dimm1 SMBus address of DIMM 1 (mainboard dependent).
 */
void cpu_reg_init(int debug_clock_disable, u8 dimm0, u8 dimm1)
{
	int msrnum;
	struct msr msr;

	/* Castle 2.0 BTM periodic sync period. */
	/* [40:37] 1 sync record per 256 bytes. */
	msrnum = CPU_PF_CONF;
	msr = rdmsr(msrnum);
	msr.hi |= (0x8 << 5);
	wrmsr(msrnum, msr);

	/* Castle performance setting.
	 * Enable Quack for fewer re-RAS on the MC.
	 */
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

	/* GLIU port active enable, limit south pole masters (AES and PCI) to
	 * one outstanding transaction.
	 */
	msrnum = GLIU1_PORT_ACTIVE;
	msr = rdmsr(msrnum);
	msr.lo &= ~0x880;
	wrmsr(msrnum, msr);

	/* Set the Delay Control in GLCP. */
	set_delay_control(dimm0, dimm1);

	/* Enable RSDC. */
	msrnum = CPU_AC_SMM_CTL;
	msr = rdmsr(msrnum);
	msr.lo |= SMM_INST_EN_SET;
	wrmsr(msrnum, msr);

	/* FPU imprecise exceptions bit. */
	msrnum = CPU_FPU_MSR_MODE;
	msr = rdmsr(msrnum);
	msr.lo |= FPU_IE_SET;
	wrmsr(msrnum, msr);

	/* Power savers (do after BIST). */
	/* Enable Suspend on HLT & PAUSE instructions. */
	msrnum = CPU_XC_CONFIG;
	msr = rdmsr(msrnum);
	msr.lo |= XC_CONFIG_SUSP_ON_HLT | XC_CONFIG_SUSP_ON_PAUSE;
	wrmsr(msrnum, msr);

	/* Enable SUSP and allow TSC to run in Suspend (keep speed
	 * detection happy).
	 */
	msrnum = CPU_BC_CONF_0;
	msr = rdmsr(msrnum);
	msr.lo |= TSC_SUSP_SET | SUSP_EN_SET;
	msr.lo &= 0x0F0FFFFFF;
	msr.lo |= 0x002000000;	/* PBZ213: Set PAUSEDLY = 2. */
	wrmsr(msrnum, msr);

	/* Disable the debug clock to save power. */
	/* Note: Leave it enabled for FS2 debug. */
	if (debug_clock_disable && 0) {
		msrnum = GLCP_DBGCLKCTL;
		msr.hi = 0;
		msr.lo = 0;
		wrmsr(msrnum, msr);
	}

	/* Setup throttling delays to proper mode if it is ever enabled. */
	msrnum = GLCP_TH_OD;
	msr.hi = 0;
	msr.lo = 0x00000603C;
	wrmsr(msrnum, msr);

	/* Fix CPU bugs. */
#warning testing fixing bugs in initram
	cpu_bug();
}
