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
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <amd_geodelx.h>
#include <spd.h>

/* all these functions used to be in a lot of fiddly little files.  To
  * make it easier to find functions, we are merging them here. This
  * file is our first real cpu-specific support file and should serve
  * as a model for v3 cpu-specific support. So, warning, you might
  * think it makes sense to split this file up, but we've tried that,
  * and it sucks.
  */

/** 
  * start_time1 Starts Timer 1 for port 61 use. FIXME try to figure
  * out what these values mean.
  */
void start_timer1(void)
{
	outb(0x56, 0x43);
	outb(0x12, 0x41);
}

/** 
  * system_preinit Very early initialization needed for almost
  * everything else.  Currently, all we do is start timer1.
  */
void system_preinit(void)
{
	start_timer1();
}


/* cpu bug management */
/**
 *
 *	pci_deadlock Bugtool #465 and #609 PCI cache deadlock There is
 *	also fix code in cache and PCI functions. This bug is very is
 *	pervasive.
 *
 */
static void pci_deadlock(void)
{
	struct msr  msr;

	/*
	 * forces serialization of all load misses. Setting this bit prevents the 
	 * DM pipe from backing up if a read request has to be held up waiting 
	 * for PCI writes to complete.
	 */
	msr = rdmsr(CPU_DM_CONFIG0);
	msr.lo |= DM_CONFIG0_LOWER_MISSER_SET;
	wrmsr(CPU_DM_CONFIG0, msr);

	/* write serialize memory hole to PCI. Need to unWS when something is 
	 * shadowed regardless of cachablility.
	 */
	msr.lo = 0x021212121;
	msr.hi = 0x021212121;
	wrmsr(CPU_RCONF_A0_BF, msr);
	wrmsr(CPU_RCONF_C0_DF, msr);
	wrmsr(CPU_RCONF_E0_FF, msr);
}

/**	disable_memory_reorder PBZ 3659: The MC reordered transactions
  *	incorrectly and breaks coherency.  Disable reording and take a
  *	potential performance hit.  This is safe to do here and not in
  *	MC init since there is nothing to maintain coherency with and
  *	the cache is not enabled yet.
  */
/****************************************************************************/
static void disable_memory_reorder(void)
{
	struct msr  msr;

	msr = rdmsr(MC_CF8F_DATA);
	msr.hi |= CF8F_UPPER_REORDER_DIS_SET;
	wrmsr(MC_CF8F_DATA, msr);
}

/**
  * Fix up register settings to manage known CPU bugs.  For cpu
  * version C3. Should be the only released version
  */
void cpu_bug(void)
{
	pci_deadlock();
	disable_memory_reorder();
	printk(BIOS_DEBUG, "Done cpubug fixes \n");
}

/**
 * Reset the phase locked loop hardware. After power on as part of
 * this operation, we have to set the clock hardware and reboot. Thus,
 * we have to know if we have been here before. To do this, we use the
 * RSTPLL_LOWER_SWFLAGS_SHIFT flag in the msrGlcpSysRstpll. Also, the
 * clocks can either be configured via passed-in parameters or
 * hardware straps. Once set, we yank the hardware reset line and
 * hlt. We should never reach the hlt, but one never knows.
 *
 * @param manualconf If non-zero, use passed-in parameters to
 * determine how to configure pll -- manual or automagic. 
 * If manual, use passed-in parameters pll_hi and pll_lo
 * @param pll_hi value to use for the high 32 bits of the pll msr
 * @param pll_lo value to use for the low 32 bits of the pll msr
 */
void pll_reset(int manualconf, u32 pll_hi, u32 pll_lo)
{
	struct msr  msrGlcpSysRstpll;

	msrGlcpSysRstpll = rdmsr(GLCP_SYS_RSTPLL);

	printk(BIOS_DEBUG, 
		"_MSR GLCP_SYS_RSTPLL (%08x) value is: %08x:%08x\n", msrGlcpSysRstpll.hi, msrGlcpSysRstpll.lo);
	post_code(POST_PLL_INIT);

	if (!(msrGlcpSysRstpll.lo & (1 << RSTPLL_LOWER_SWFLAGS_SHIFT))) {
		printk(BIOS_DEBUG,"Configuring PLL\n");
		if (manualconf) {
			post_code(POST_PLL_MANUAL);
			/* CPU and GLIU mult/div (GLMC_CLK = GLIU_CLK / 2)  */
			msrGlcpSysRstpll.hi = pll_hi;

			/* Hold Count - how long we will sit in reset */
			msrGlcpSysRstpll.lo = pll_lo;
		} else {
			/*automatic configuration (straps) */
			post_code(POST_PLL_STRAP);
			msrGlcpSysRstpll.lo &=
			    ~(0xFF << RSTPPL_LOWER_HOLD_COUNT_SHIFT);
			msrGlcpSysRstpll.lo |=
			    (0xDE << RSTPPL_LOWER_HOLD_COUNT_SHIFT);
			msrGlcpSysRstpll.lo &=
			    ~(RSTPPL_LOWER_COREBYPASS_SET |
			      RSTPPL_LOWER_MBBYPASS_SET);
			msrGlcpSysRstpll.lo |=
			    RSTPPL_LOWER_COREPD_SET | RSTPPL_LOWER_CLPD_SET;
		}
		/* Use SWFLAGS to remember: "we've already been here"  */
		msrGlcpSysRstpll.lo |= (1 << RSTPLL_LOWER_SWFLAGS_SHIFT);

		/* "reset the chip" value */
		msrGlcpSysRstpll.lo |= RSTPPL_LOWER_CHIP_RESET_SET;
		wrmsr(GLCP_SYS_RSTPLL, msrGlcpSysRstpll);

		/*      You should never get here..... The chip has reset. */
		printk(BIOS_EMERG,"CONFIGURING PLL FAILURE -- HALT\n");
		post_code(POST_PLL_RESET_FAIL);
		__asm__ __volatile__("hlt\n");

	}
	printk(BIOS_DEBUG, "Done pll_reset\n");
	return;
}


/**
 * Return the CPU clock rate. Rates in this system are always returned
 * as multkiples of 33 Mhz.
 *
 */
u32 cpu_speed(void)
{
	u32 speed;
	struct msr  msr;

	msr = rdmsr(GLCP_SYS_RSTPLL);
	speed = ((((msr.hi >> RSTPLL_UPPER_CPUMULT_SHIFT) & 0x1F) + 1) * 333) / 10;
	if ((((((msr.hi >> RSTPLL_UPPER_CPUMULT_SHIFT) & 0x1F) + 1) * 333) % 10) > 5) {
		++speed;
	}
	return (speed);
}

/**
 * Return the Geode Link clock rate.  Rates in this system are always
 * returned as multkiples of 33 Mhz.
 *
 */
u32 geode_link_speed(void)
{
	unsigned int speed;
	struct msr  msr;

	msr = rdmsr(GLCP_SYS_RSTPLL);
	speed = ((((msr.hi >> RSTPLL_UPPER_GLMULT_SHIFT) & 0x1F) + 1) * 333) / 10;
	if ((((((msr.hi >> RSTPLL_UPPER_GLMULT_SHIFT) & 0x1F) + 1) * 333) % 10) > 5) {
		++speed;
	}
	return (speed);
}


/**
 * Return the PCI bus clock rate.  Rates in this system are always
 * returned as multkiples of 33 Mhz.
 *
 */
u32 pci_speed(void)
{
	struct msr  msr;

	msr = rdmsr(GLCP_SYS_RSTPLL);
	if (msr.hi & (1 << RSTPPL_LOWER_PCISPEED_SHIFT)) {
		return (66);
	} else {
		return (33);
	}
}

/**
 * set_delay_control. This is Black Magic DRAM timing
 * juju(http://www.thefreedictionary.com/juju) Dram delay depends on
 * cpu clock, memory bus clock, memory bus loading, memory bus
 * termination, your middle initial (ha! caught you!), Geode Link
 * clock rate, and dram timing specifications. From this the code
 * computes a number which is "known to work". No, hardware is not an
 * exact science. And, finally, if an FS2 (jtag debugger) is hooked
 * up, then just don't to anything. This code was written by a master
 * of the Dark Arts at AMD and should not be modified in any way.
 * 
 * @param num_banks How many banks of DRAM there are
 * @param dimm0 DIMM 0 SMBus address
 * @param dimm1 DIMM 1 SMBus address
 * @param sram_width Data width of the SDRAM
 */

void set_delay_control(u8 dimm0, u8 dimm1)
{
	u32 msrnum, glspeed;
	u8 spdbyte0, spdbyte1;
	int numdimms = 0;
	struct msr  msr;

	glspeed = geode_link_speed();

	/* fix delay controls for DM and IM arrays */
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

	/* Enable setting */
	msrnum = CPU_BC_MSS_ARRAY_CTL_ENA;
	msr.hi = 0;
	msr.lo = 0x00000001;
	wrmsr(msrnum, msr);

	/* Debug Delay Control Setup Check
	  * 
	  *  Leave it alone if it has been setup. FS2 or something is here. 
	  */
	msrnum = GLCP_DELAY_CONTROLS;
	msr = rdmsr(msrnum);
	if (msr.lo & ~(0x7C0)) {
		return;
	}

	/*
	 * Delay Controls based on DIMM loading. UGH!
	 * # of Devices = Module Width (SPD6) / Device Width(SPD13) * Physical Banks(SPD5)
	 * Note - We only support module width of 64.
	 */
	spdbyte0 = smbus_read_byte(dimm0, SPD_PRIMARY_SDRAM_WIDTH);
	if (spdbyte0 != 0xFF) {
		numdimms++;
		spdbyte0 = (unsigned char)64 / spdbyte0 *
		    (unsigned char)(smbus_read_byte(dimm0, SPD_NUM_DIMM_BANKS));
	} else {
		spdbyte0 = 0;
	}

	spdbyte1 = smbus_read_byte(dimm1, SPD_PRIMARY_SDRAM_WIDTH);
	if (spdbyte1 != 0xFF) {
		numdimms++;
		spdbyte1 = (unsigned char)64 / spdbyte1 *
		    (unsigned char)(smbus_read_byte(dimm1, SPD_NUM_DIMM_BANKS));
	} else {
		spdbyte1 = 0;
	}

/* The current thinking. Subject to change...

;								   "FUTURE ROBUSTNESS" PROPOSAL
;								   ----------------------------
;		DIMM	 Max MBUS					   MC 0x2000001A bits 26:24
;DIMMs	devices	 Frequency	 MCP 0x4C00000F Setting		 vvv
;-----	-------	 ---------	 ----------------------	 ----------
;1		 4		 400MHz		 0x82*100FF 0x56960004		  4
;1		 8		 400MHz		 0x82*100AA 0x56960004		  4
;1		 16		 400MHz		 0x82*10055 0x56960004		  4
;
;2		 4,4	 400MHz		 0x82710000 0x56960004		  4
;2		 8,8	 400MHz		 0xC27100A5 0x56960004		  4	*** OUT OF PUBLISHED ENVELOPE ***
;
;2		16,4	 >333		 0xB27100A5 0x56960004		  4	*** OUT OF PUBLISHED ENVELOPE ***
;2		16,8	 >333		 0xB27100A5 0x56960004		  4	*** OUT OF PUBLISHED ENVELOPE ***
;2		16,16	 >333		 0xB2710000 0x56960004		  4	*** OUT OF PUBLISHED ENVELOPE ***
;
;1		 4		 <=333MHz	 0x83*100FF 0x56960004		  3
;1		 8		 <=333MHz	 0x83*100AA 0x56960004		  3
;1		 16		 <=333MHz	 0x83*100AA 0x56960004		  3
;
;2		 4,4	 <=333MHz	 0x837100A5 0x56960004		  3
;2		 8,8	 <=333MHz	 0x937100A5 0x56960004		  3
;
;2		16,4	 <=333MHz	 0xB37100A5 0x56960004		  3	*** OUT OF PUBLISHED ENVELOPE ***
;2		16,8	 <=333MHz	 0xB37100A5 0x56960004		  3	*** OUT OF PUBLISHED ENVELOPE ***
;2		16,16	 <=333MHz	 0xB37100A5 0x56960004		  3	*** OUT OF PUBLISHED ENVELOPE ***
;=========================================================================
;* - Bit 55 (disable SDCLK 1,3,5) should be set if there is a single DIMM in slot 0,
;	 but it should be clear for all 2 DIMM settings and if a single DIMM is in slot 1.
;	 Bits 54:52 should always be set to '111'.

;No VTT termination
;-------------------------------------
;ADDR/CTL have 22 ohm series R
;DQ/DQM/DQS have 33 ohm series R
;
;		DIMM	 Max MBUS
;DIMMs	devices	 Frequency	 MCP 0x4C00000F Setting
;-----	-------	 ---------	 ----------------------
;1		 4		 400MHz		 0xF2F100FF 0x56960004		  4			The MC changes improve Salsa.
;1		 8		 400MHz		 0xF2F100FF 0x56960004		  4			Delay controls no real change,
;1		 4		 <=333MHz	 0xF2F100FF 0x56960004		  3			just fixing typo in left side.
;1		 8		 <=333MHz	 0xF2F100FF 0x56960004		  3
;1		 16		 <=333MHz	 0xF2F100FF 0x56960004		  3
*/
	msr.hi = msr.lo = 0;

	if (spdbyte0 == 0 || spdbyte1 == 0) {
		/* one dimm solution */
		if (spdbyte1 == 0) {
			msr.hi |= 0x000800000;
		}
		spdbyte0 += spdbyte1;
		if (spdbyte0 > 8) {
			/* large dimm */
			if (glspeed < 334) {
				msr.hi |= 0x0837100AA;
				msr.lo |= 0x056960004;
			} else {
				msr.hi |= 0x082710055;
				msr.lo |= 0x056960004;
			}
		} else if (spdbyte0 > 4) {
			/* medium dimm */
			if (glspeed < 334) {
				msr.hi |= 0x0837100AA;
				msr.lo |= 0x056960004;
			} else {
				msr.hi |= 0x0827100AA;
				msr.lo |= 0x056960004;
			}
		} else {
			/* small dimm */
			if (glspeed < 334) {
				msr.hi |= 0x0837100FF;
				msr.lo |= 0x056960004;
			} else {
				msr.hi |= 0x0827100FF;
				msr.lo |= 0x056960004;
			}
		}
	} else {
		/* two dimm solution */
		spdbyte0 += spdbyte1;
		if (spdbyte0 > 24) {
			/* huge dimms */
			if (glspeed < 334) {
				msr.hi |= 0x0B37100A5;
				msr.lo |= 0x056960004;
			} else {
				msr.hi |= 0x0B2710000;
				msr.lo |= 0x056960004;
			}
		} else if (spdbyte0 > 16) {
			/* large dimms */
			if (glspeed < 334) {
				msr.hi |= 0x0B37100A5;
				msr.lo |= 0x056960004;
			} else {
				msr.hi |= 0x0B27100A5;
				msr.lo |= 0x056960004;
			}
		} else if (spdbyte0 >= 8) {
			/* medium dimms */
			if (glspeed < 334) {
				msr.hi |= 0x0937100A5;
				msr.lo |= 0x056960004;
			} else {
				msr.hi |= 0x0C27100A5;
				msr.lo |= 0x056960004;
			}
		} else {
			/* small dimms */
			if (glspeed < 334) {
				msr.hi |= 0x0837100A5;
				msr.lo |= 0x056960004;
			} else {
				msr.hi |= 0x082710000;
				msr.lo |= 0x056960004;
			}
		}
	}
	wrmsr(GLCP_DELAY_CONTROLS, msr);
	return;
}

/**
 * cpu_reg_init. All cpu register settings, here in one place, and
 * done in the proper order.
 *
 * @param debug_clock_disable Disable the debug clock to save power. Currently ignored, but we need to 
 * pick this up from a CMOS setting in future. 
 * @param dimm0 SMBus address of dimm0 (mainboard dependent)
 * @param dimm1 SMBus address of dimm1 (mainboard dependent)
 */
void cpu_reg_init(int debug_clock_disable, u8 dimm0, u8 dimm1)
{
	int msrnum;
	struct msr  msr;

	/* Castle 2.0 BTM periodic sync period. */
	/*      [40:37] 1 sync record per 256 bytes */
	msrnum = CPU_PF_CONF;
	msr = rdmsr(msrnum);
	msr.hi |= (0x8 << 5);
	wrmsr(msrnum, msr);

	/*
	   ; Castle performance setting.
	   ; Enable Quack for fewer re-RAS on the MC
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

	/*      GLIU port active enable, limit south pole masters (AES and PCI) to one outstanding transaction. */
	msrnum = GLIU1_PORT_ACTIVE;
	msr = rdmsr(msrnum);
	msr.lo &= ~0x880;
	wrmsr(msrnum, msr);

	/* Set the Delay Control in GLCP */
	set_delay_control(dimm0, dimm1);

	/*  Enable RSDC */
	msrnum = CPU_AC_SMM_CTL;
	msr = rdmsr(msrnum);
	msr.lo |= SMM_INST_EN_SET;
	wrmsr(msrnum, msr);

	/* FPU imprecise exceptions bit */
	msrnum = CPU_FPU_MSR_MODE;
	msr = rdmsr(msrnum);
	msr.lo |= FPU_IE_SET;
	wrmsr(msrnum, msr);

	/* Power Savers (Do after BIST) */
	/* Enable Suspend on HLT & PAUSE instructions */
	msrnum = CPU_XC_CONFIG;
	msr = rdmsr(msrnum);
	msr.lo |= XC_CONFIG_SUSP_ON_HLT | XC_CONFIG_SUSP_ON_PAUSE;
	wrmsr(msrnum, msr);

	/* Enable SUSP and allow TSC to run in Suspend (keep speed detection happy) */
	msrnum = CPU_BC_CONF_0;
	msr = rdmsr(msrnum);
	msr.lo |= TSC_SUSP_SET | SUSP_EN_SET;
	msr.lo &= 0x0F0FFFFFF;
	msr.lo |= 0x002000000;	/* PBZ213: Set PAUSEDLY = 2 */
	wrmsr(msrnum, msr);

	/* Disable the debug clock to save power. */
	/* NOTE: leave it enabled for fs2 debug */
	if (debug_clock_disable && 0){
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
	/* fix cpu bugs */
#warning testing fixing bugs in initram
	cpu_bug();
}
