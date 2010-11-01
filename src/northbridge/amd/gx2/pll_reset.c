/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2010 Nils Jacobs
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <cpu/x86/tsc.h>

#define CLOCK_TICK_RATE	1193180U /* Underlying HZ */
#define CALIBRATE_INTERVAL ((20*CLOCK_TICK_RATE)/1000) /* 20ms */
#define CALIBRATE_DIVISOR  (20*1000) /* 20ms / 20000 == 1usec */

#if 0
static unsigned int calibrate_tsc(void)
{
	/* Set the Gate high, disable speaker */
	outb((inb(0x61) & ~0x02) | 0x01, 0x61);

	/*
	 * Now let's take care of CTC channel 2
	 *
	 * Set the Gate high, program CTC channel 2 for mode 0,
	 * (interrupt on terminal count mode), binary count,
	 * load 5 * LATCH count, (LSB and MSB) to begin countdown.
	 */
	outb(0xb0, 0x43);			/* binary, mode 0, LSB/MSB, Ch 2 */
	outb(CALIBRATE_INTERVAL	& 0xff, 0x42);	/* LSB of count */
	outb(CALIBRATE_INTERVAL	>> 8, 0x42);	/* MSB of count */

	{
		tsc_t start;
		tsc_t end;
		unsigned long count;

		start = rdtsc();
		count = 0;
		do {
			count++;
		} while ((inb(0x61) & 0x20) == 0);
		end = rdtsc();

		/* Error: ECTCNEVERSET */
		if (count <= 1)
			goto bad_ctc;

		/* 64-bit subtract - gcc just messes up with long longs */
		__asm__("subl %2,%0\n\t"
			"sbbl %3,%1"
			:"=a" (end.lo), "=d" (end.hi)
			:"g" (start.lo), "g" (start.hi),
			 "0" (end.lo), "1" (end.hi));

		/* Error: ECPUTOOFAST */
		if (end.hi)
			goto bad_ctc;


		/* Error: ECPUTOOSLOW */
		if (end.lo <= CALIBRATE_DIVISOR)
			goto bad_ctc;

		return (end.lo + CALIBRATE_DIVISOR -1)/CALIBRATE_DIVISOR;
	}

	/*
	 * The CTC wasn't reliable: we got a hit on the very first read,
	 * or the CPU was so fast/slow that the quotient wouldn't fit in
	 * 32 bits..
	 */
bad_ctc:
	print_err("bad_ctc\n");
	return 0;
}
#endif

/* spll_raw_clk = SYSREF * FbDIV,
 * GLIU Clock   = spll_raw_clk / MDIV
 * CPU Clock    = spll_raw_clk / VDIV
 */

/* table for Feedback divisor to FbDiv register value */
static const unsigned char plldiv2fbdiv[] = {
	 0,  0,  0,  0,  0,  0, 15,  7,  3,  1,  0, 32, 16, 40, 20, 42, /* pll div  0 - 15 */
	21, 10, 37, 50, 25, 12, 38, 19,  9,  4, 34, 17,  8, 36, 18, 41, /* pll div 16 - 31 */
	52, 26, 45, 54, 27, 13,  6, 35, 49, 56, 28, 46, 23, 11, 05, 02, /* pll div 32 - 47 */
	33, 48, 24, 44, 22, 43, 53, 58, 29, 14, 39, 51, 57, 60, 30, 47, /* pll div 48 - 63 */
};

/* table for FbDiv register value to Feedback divisor */
static const unsigned char fbdiv2plldiv[] = {
	10,  9, 47,  8, 25, 46, 38,  7, 28, 24, 17, 45, 21, 37, 57,  6,
	12, 27, 30, 23, 14, 16, 52, 44, 50, 20, 33, 36, 42, 56,  0,  0,
	11, 48, 26, 39, 29, 18, 22, 58, 13, 31, 15, 53, 51, 34, 43,  0,
	49, 40, 19, 59, 32, 54, 35,  0, 41, 60, 55,  0, 61,  0,  0,  0
};

static const unsigned char pci33_ddr_crt [] = {
	/* FbDIV, VDIV, MDIV		   CPU/GeodeLink */
	     12,    2,    3,		// 200/133
	     16,    2,    3,		// 266/177
             18,    2,    3,		// 300/200
             20,    2,    3,		// 333/222
             22,    2,    3,		// 366/244
             24,    2,    3,		// 400/266
             26,    2,    3             // 433/289
};

/*	FbDIV	VDIV	MDIV	CPU/GeodeLink */
/*	12	2	3	200/133 */
/*	16	2	3	266/177 */
/*	18	2	3	300/200 */
/*	20	2	3	333/222 */
/*	22	2	3	366/244 */
/*	24	2	3	400/266 */
/*	26	2	3	433/289 */

#if 0
static unsigned int get_memory_speed(void)
{
	unsigned char val, hi, lo;

	val = spd_read_byte(0xA0, 9);
	hi = (val >> 4) & 0x0f;
	lo = val & 0x0f;

	return 20000/(hi*10 + lo);
}
#endif

#define USE_GOODRICH_VERSION 1

#if USE_GOODRICH_VERSION
///////////////////////////////////////////////////////////////////////////////
// Goodrich Version of pll_reset

/* PLLCHECK_COMPLETED is the "we've already done this" flag */
#define PLLCHECK_COMPLETED (1 << RSTPLL_LOWER_SWFLAGS_SHIFT)

#ifndef RSTPPL_LOWER_BYPASS_SET
#define RSTPPL_LOWER_BYPASS_SET (1 << GLCP_SYS_RSTPLL_BYPASS)
#endif /* RSTPPL_LOWER_BYPASS_SET */

#define DEFAULT_MDIV	3
#define DEFAULT_VDIV	2
#define DEFAULT_FBDIV	22	// 366/244 ; 24 400/266 018 ;300/200

static void pll_reset(void)
{
	msr_t msrGlcpSysRstpll;
	unsigned MDIV_VDIV_FBDIV;
	unsigned SyncBits;			/* store the sync bits in up ebx */

	/* clear the Bypass bit */

	/* If the straps say we are in bypass and the syspll is not AND there are no software */
	/* bits set then FS2 or something set up the PLL and we should not change it. */

	msrGlcpSysRstpll = rdmsr(GLCP_SYS_RSTPLL);
	msrGlcpSysRstpll.lo &= ~RSTPPL_LOWER_BYPASS_SET;
	wrmsr(GLCP_SYS_RSTPLL, msrGlcpSysRstpll);

	/* If the "we've already been here" flag is set, don't reconfigure the pll */
	if ( !(msrGlcpSysRstpll.lo & PLLCHECK_COMPLETED ) )
	{ /* we haven't configured the PLL; do it now */

		/* Store PCI33(0)/66(1), SDR(0)/DDR(1), and CRT(0)/TFT(1) in upper esi to get to the */
		/* correct Strap Table. */
		post_code(POST_PLL_INIT);

		/* configure for DDR */
		msrGlcpSysRstpll.lo &= ~(1 << RSTPPL_LOWER_SDRMODE_SHIFT);
		wrmsr(GLCP_SYS_RSTPLL, msrGlcpSysRstpll);

		/* Use Manual settings */
		/*	UseManual: */
		post_code(POST_PLL_MANUAL);

		/* DIV settings manually entered. */
		/* ax = VDIV, upper eax = MDIV, upper ecx = FbDIV */
		/* use gs and fs since we don't need them. */

		/*	ProgramClocks: */
		/* ax = VDIV, upper eax = MDIV, upper ecx = FbDIV */
		/* move everything into ebx */
		/* VDIV */
		MDIV_VDIV_FBDIV = ((DEFAULT_VDIV - 2) << RSTPLL_UPPER_VDIV_SHIFT);

		/* MDIV */
		MDIV_VDIV_FBDIV |= ((DEFAULT_MDIV - 2) << RSTPLL_UPPER_MDIV_SHIFT);

		/* FbDIV */
		MDIV_VDIV_FBDIV |= (plldiv2fbdiv[DEFAULT_FBDIV] << RSTPLL_UPPER_FBDIV_SHIFT);

		/* write GLCP_SYS_RSTPPL (GLCP reg 0x14) with clock values */
		msrGlcpSysRstpll.lo &= ~(1 << RSTPPL_LOWER_SDRMODE_SHIFT);
		wrmsr(GLCP_SYS_RSTPLL, msrGlcpSysRstpll);

		msrGlcpSysRstpll.hi = MDIV_VDIV_FBDIV;
		wrmsr(GLCP_SYS_RSTPLL, msrGlcpSysRstpll);

		/* Set Reset, LockWait, and SW flag */
		/*	DoReset: */

		/* CheckSemiSync proc */
		/* Check for Semi-Sync in GeodeLink and CPU. */
		/* We need to do this here since the strap settings don't account for these bits. */
		SyncBits = 0;			// store the sync bits in up ebx

		/* Check for Bypass mode. */
		if (msrGlcpSysRstpll.lo & RSTPPL_LOWER_BYPASS_SET)
		{
			/* If we are in BYPASS PCI may or may not be sync'd but CPU and GeodeLink will. */
			SyncBits |= RSTPPL_LOWER_CPU_SEMI_SYNC_SET;
		}
		else
		{
			/*	CheckPCIsync: */
			/* If FBdiv/Mdiv is evenly divisible then set the PCI semi-sync. FB is always greater */
			/* look up the real divider... if we get a 0 we have serious problems */
			if ( !(fbdiv2plldiv[((msrGlcpSysRstpll.hi >> RSTPLL_UPPER_FBDIV_SHIFT) & 0x3f)] %
				(((msrGlcpSysRstpll.hi >> RSTPLL_UPPER_MDIV_SHIFT) & 0x0F) + 2)) )
			{
				SyncBits |= RSTPPL_LOWER_PCI_SEMI_SYNC_SET;
			}

			/*	CheckCPUSync: */
			/* If Vdiv/Mdiv is evenly divisible then set the CPU semi-sync. */
			/* CPU is always greater or equal. */
			if (!((((msrGlcpSysRstpll.hi >> RSTPLL_UPPER_MDIV_SHIFT) & 0x07) + 2) %
				(((msrGlcpSysRstpll.hi >> RSTPLL_UPPER_VDIV_SHIFT) & 0x0F) + 2)))
			{
				SyncBits |= RSTPPL_LOWER_CPU_SEMI_SYNC_SET;
			}
		}


		/*	SetSync: */
		msrGlcpSysRstpll.lo &= ~(RSTPPL_LOWER_PCI_SEMI_SYNC_SET | RSTPPL_LOWER_CPU_SEMI_SYNC_SET);
		msrGlcpSysRstpll.lo |= SyncBits;
		wrmsr(GLCP_SYS_RSTPLL, msrGlcpSysRstpll);
		/* CheckSemiSync endp */

		/* now we do the reset */
		/* Set hold count to 99 (063h) */
		msrGlcpSysRstpll.lo &= ~(0x0FF << RSTPPL_LOWER_HOLD_COUNT_SHIFT);
		msrGlcpSysRstpll.lo |=  (0x0DE << RSTPPL_LOWER_HOLD_COUNT_SHIFT);
		msrGlcpSysRstpll.lo |=  PLLCHECK_COMPLETED;		// Say we are done
		wrmsr(GLCP_SYS_RSTPLL, msrGlcpSysRstpll);

		/* Don't want to use LOCKWAIT */
		msrGlcpSysRstpll.lo |= (RSTPPL_LOWER_PLL_RESET_SET + RSTPPL_LOWER_PD_SET);
		msrGlcpSysRstpll.lo |= RSTPPL_LOWER_CHIP_RESET_SET;
		wrmsr(GLCP_SYS_RSTPLL, msrGlcpSysRstpll);

		/* You should never get here..... The chip has reset. */
		post_code(POST_PLL_RESET_FAIL);
		while (1);

	} /* we haven't configured the PLL; do it now */

}
// End of Goodrich version of pll_reset
///////////////////////////////////////////////////////////////////////////////

#else // #if USE_GOODRICH_VERSION

static void pll_reset(void)
{
	msr_t msr;
	unsigned int sysref, spll_raw, cpu_core, gliu;
	unsigned mdiv, vdiv, fbdiv;

	/* get CPU core clock in MHZ */
	cpu_core = calibrate_tsc();
	print_debug("Cpu core is ");
	print_debug_hex32(cpu_core);
	print_debug("\n");

	msr = rdmsr(GLCP_SYS_RSTPLL);
	if (msr.lo & (1 << GLCP_SYS_RSTPLL_BYPASS)) {
#if 0
		print_debug("MSR ");
		print_debug_hex32(GLCP_SYS_RSTPLL);
		print_debug("is ");
		print_debug_hex32(msr.hi);
		print_debug(":");
		print_debug_hex32(msr.lo);

		msr.hi = PLLMSRhi;
		msr.lo = PLLMSRlo;
		wrmsr(GLCP_SYS_RSTPLL, msr);
		msr.lo |= PLLMSRlo1;
		wrmsr(GLCP_SYS_RSTPLL, msr);

		print_debug("Reset PLL\n");

		msr.lo |= PLLMSRlo2;
		wrmsr(GLCP_SYS_RSTPLL,msr);
		print_debug("should not be here\n");
#endif
		print_err("shit");
		while (1)
			;
	}

	if (msr.lo & GLCP_SYS_RSTPLL_SWFLAGS_MASK) {
		/* PLL is already set and we are reboot from PLL reset */
		print_debug("reboot from BIOS reset\n");
		return;
	}

	/* get the sysref clock rate */
	vdiv  = (msr.hi >> GLCP_SYS_RSTPLL_VDIV_SHIFT) & 0x07;
	vdiv += 2;
	fbdiv = (msr.hi >> GLCP_SYS_RSTPLL_FBDIV_SHIFT) & 0x3f;
	fbdiv = fbdiv2plldiv[fbdiv];
	spll_raw = cpu_core * vdiv;
	sysref   = spll_raw / fbdiv;

	/* get target memory rate by SPD */
	//gliu = get_memory_speed();

	msr.hi = 0x00000019;
	msr.lo = 0x06de0378;
	wrmsr(0x4c000014, msr);
	msr.lo |= ((0xde << 16) | (1 << 26) | (1 << 24));
	wrmsr(0x4c000014, msr);

	print_debug("Reset PLL\n");

	msr.lo |= ((1<<14) |(1<<13) | (1<<0));
	wrmsr(0x4c000014,msr);

	print_debug("should not be here\n");
}
#endif // #if USE_GOODRICH_VERSION
