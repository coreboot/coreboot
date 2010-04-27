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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/**************************************************************************
;*
;*	SetDelayControl
;*
;*************************************************************************/
static void SetDelayControl(void)
{
	unsigned int msrnum, glspeed;
	unsigned char spdbyte0, spdbyte1;
	msr_t msr;

	glspeed = GeodeLinkSpeed();

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
	   Leave it alone if it has been setup. FS2 or something is here. */
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
	spdbyte0 = spd_read_byte(DIMM0, SPD_PRIMARY_SDRAM_WIDTH);
	if (spdbyte0 != 0xFF) {
		spdbyte0 = (unsigned char)64 / spdbyte0 *
		    (unsigned char)(spd_read_byte(DIMM0, SPD_NUM_DIMM_BANKS));
	} else {
		spdbyte0 = 0;
	}

	spdbyte1 = spd_read_byte(DIMM1, SPD_PRIMARY_SDRAM_WIDTH);
	if (spdbyte1 != 0xFF) {
		spdbyte1 = (unsigned char)64 / spdbyte1 *
		    (unsigned char)(spd_read_byte(DIMM1, SPD_NUM_DIMM_BANKS));
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
;
;1		 4		 <=333MHz	 0x83*100FF 0x56960004		  3
;1		 8		 <=333MHz	 0x83*100AA 0x56960004		  3
;1		 16		 <=333MHz	 0x83*100AA 0x56960004		  3
;
;2		 4,4	 <=333MHz	 0x837100A5 0x56960004		  3
;2		 8,8	 <=333MHz	 0x937100A5 0x56960004		  3
;
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
;1		 4		 400MHz		 0xF2F100FF 0x56960004		  4			The No VTT changes improve timing.
;1		 8		 400MHz		 0xF2F100FF 0x56960004		  4
;1		 4		 <=333MHz	 0xF2F100FF 0x56960004		  3
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
	print_debug("Try to write GLCP_DELAY_CONTROLS: hi ");
	print_debug_hex32(msr.hi);
	print_debug(" and lo ");
	print_debug_hex32(msr.lo);
	print_debug("\n");
	wrmsr(GLCP_DELAY_CONTROLS, msr);
	print_debug("SetDelayControl done\n");
	return;
}

/* ***************************************************************************/
/* *	cpuRegInit*/
/* ***************************************************************************/
void cpuRegInit(void)
{
	int msrnum;
	msr_t msr;

	/* Castle 2.0 BTM periodic sync period. */
	/*      [40:37] 1 sync record per 256 bytes */
	print_debug("Castle 2.0 BTM periodic sync period.\n");
	msrnum = CPU_PF_CONF;
	msr = rdmsr(msrnum);
	msr.hi |= (0x8 << 5);
	wrmsr(msrnum, msr);

	/*
	 * LX performance setting.
	 * Enable Quack for fewer re-RAS on the MC
	 */
	print_debug("Enable Quack for fewer re-RAS on the MC\n");
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
	print_debug(" GLIU port active enable\n");
	msrnum = GLIU1_PORT_ACTIVE;
	msr = rdmsr(msrnum);
	msr.lo &= ~0x880;
	wrmsr(msrnum, msr);

	/* Set the Delay Control in GLCP */
	print_debug("Set the Delay Control in GLCP\n");
	SetDelayControl();

	/*  Enable RSDC */
	print_debug("Enable RSDC\n");
	msrnum = CPU_AC_SMM_CTL;
	msr = rdmsr(msrnum);
	msr.lo |= SMM_INST_EN_SET;
	wrmsr(msrnum, msr);

	/* FPU imprecise exceptions bit */
	print_debug("FPU imprecise exceptions bit\n");
	msrnum = CPU_FPU_MSR_MODE;
	msr = rdmsr(msrnum);
	msr.lo |= FPU_IE_SET;
	wrmsr(msrnum, msr);

	/* Power Savers (Do after BIST) */
	/* Enable Suspend on HLT & PAUSE instructions */
	print_debug("Enable Suspend on HLT & PAUSE instructions\n");
	msrnum = CPU_XC_CONFIG;
	msr = rdmsr(msrnum);
	msr.lo |= XC_CONFIG_SUSP_ON_HLT | XC_CONFIG_SUSP_ON_PAUSE;
	wrmsr(msrnum, msr);

	/* Enable SUSP and allow TSC to run in Suspend (keep speed detection happy) */
	print_debug("Enable SUSP and allow TSC to run in Suspend\n");
	msrnum = CPU_BC_CONF_0;
	msr = rdmsr(msrnum);
	msr.lo |= TSC_SUSP_SET | SUSP_EN_SET;
	msr.lo &= 0x0F0FFFFFF;
	msr.lo |= 0x002000000;	/* PBZ213: Set PAUSEDLY = 2 */
	wrmsr(msrnum, msr);

	/* Disable the debug clock to save power. */
	/* NOTE: leave it enabled for fs2 debug */
#if 0
	msrnum = GLCP_DBGCLKCTL;
	msr.hi = 0;
	msr.lo = 0;
	wrmsr(msrnum, msr);
#endif

	/* Setup throttling delays to proper mode if it is ever enabled. */
	print_debug("Setup throttling delays to proper mode\n");
	msrnum = GLCP_TH_OD;
	msr.hi = 0;
	msr.lo = 0x00000603C;
	wrmsr(msrnum, msr);
	print_debug("Done cpuRegInit\n");
}
