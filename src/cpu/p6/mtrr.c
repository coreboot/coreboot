/*
 * intel_mtrr.c: setting MTRR to decent values for cache initialization on P6
 *
 * Derived from intel_set_mtrr in intel_subr.c and mtrr.c in linux kernel
 *
 * Copyright 2000 Silicon Integrated System Corporation
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Reference: Intel Architecture Software Developer's Manual, Volume 3: System Programming
 *
 * $Id$
 */

#ifndef lint
static char rcsid[] = "$Id$";
#endif
#define DEBUG
#include <cpu/p6/msr.h>
#include <cpu/p6/mtrr.h>
#include <printk.h>

#define arraysize(x)   (sizeof(x)/sizeof((x)[0]))

static unsigned int mtrr_msr[] = {
	MTRRfix64K_00000_MSR, MTRRfix16K_80000_MSR, MTRRfix16K_A0000_MSR,
	MTRRfix4K_C0000_MSR, MTRRfix4K_C8000_MSR, MTRRfix4K_D0000_MSR, MTRRfix4K_D8000_MSR,
	MTRRfix4K_E0000_MSR, MTRRfix4K_E8000_MSR, MTRRfix4K_F0000_MSR, MTRRfix4K_F8000_MSR,
};

static unsigned char fixed_mtrr_values[][4] = {
	/* MTRRfix64K_00000_MSR, defines memory range from 0KB to 512 KB, each byte cover 64KB area */
	{MTRR_TYPE_WRBACK, MTRR_TYPE_WRBACK, MTRR_TYPE_WRBACK, MTRR_TYPE_WRBACK},
	{MTRR_TYPE_WRBACK, MTRR_TYPE_WRBACK, MTRR_TYPE_WRBACK, MTRR_TYPE_WRBACK},

	/* MTRRfix16K_80000_MSR, defines memory range from 512KB to 640KB, each byte cover 16KB area */
	{MTRR_TYPE_WRBACK, MTRR_TYPE_WRBACK, MTRR_TYPE_WRBACK, MTRR_TYPE_WRBACK},
	{MTRR_TYPE_WRBACK, MTRR_TYPE_WRBACK, MTRR_TYPE_WRBACK, MTRR_TYPE_WRBACK},

	/* MTRRfix16K_A0000_MSR, defines memory range from A0000 to C0000, each byte cover 16KB area */
	{MTRR_TYPE_WRCOMB, MTRR_TYPE_WRCOMB, MTRR_TYPE_WRCOMB, MTRR_TYPE_WRCOMB},
	{MTRR_TYPE_WRCOMB, MTRR_TYPE_WRCOMB, MTRR_TYPE_WRCOMB, MTRR_TYPE_WRCOMB},

	/* MTRRfix4K_C0000_MSR, defines memory range from C0000 to C8000, each byte cover 4KB area */
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},

	/* MTRRfix4K_C8000_MSR, defines memory range from C8000 to D0000, each byte cover 4KB area */
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},

	/* MTRRfix4K_D0000_MSR, defines memory range from D0000 to D8000, each byte cover 4KB area */
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},

	/* MTRRfix4K_D8000_MSR, defines memory range from D8000 to E0000, each byte cover 4KB area */
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},

	/* MTRRfix4K_E0000_MSR, defines memory range from E0000 to E8000, each byte cover 4KB area */
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},

	/* MTRRfix4K_E8000_MSR, defines memory range from E8000 to F0000, each byte cover 4KB area */
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},

	/* MTRRfix4K_F0000_MSR, defines memory range from F0000 to F8000, each byte cover 4KB area */
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},

	/* MTRRfix4K_F8000_MSR, defines memory range from F8000 to 100000, each byte cover 4KB area */
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},
	{MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH, MTRR_TYPE_WRTHROUGH},
};

void
intel_enable_fixed_mtrr()
{
	unsigned long low, high;

	rdmsr(MTRRdefType_MSR, low, high);
	low |= 0xc00;
	wrmsr(MTRRdefType_MSR, low, high);
}

void
intel_enable_var_mtrr()
{
	unsigned long low, high;

	rdmsr(MTRRdefType_MSR, low, high);
	low |= 0x800;
	wrmsr(MTRRdefType_MSR, low, high);
}

/* setting fixed mtrr, you can do some experiments with different memory type 
   defined in the table "fixed_mtrr_values" */ 
void intel_set_fixed_mtrr()
{
	unsigned int i;
	unsigned long low, high;

	for (i = 0; i < arraysize(mtrr_msr); i++) {
		low = *(unsigned long *) fixed_mtrr_values[i*2];
		high = *(unsigned long *) fixed_mtrr_values[i*2+1];
		wrmsr(mtrr_msr[i], low, high);
	}
}

/* setting variable mtrr, comes from linux kernel source */
void intel_set_var_mtrr(unsigned int reg, unsigned long base, unsigned long size, unsigned char type)
{
	unsigned int tmp;

	if (reg >= 8)
		return;

	// it is recommended that we disable and enable cache when we 
	// do this. 
	/* Disable cache */
	/* Write back the cache and flush TLB */
	asm volatile ("movl  %%cr0, %0\n\t"
		      "orl  $0x40000000, %0\n\t"
		      "wbinvd\n\t"
		      "movl  %0, %%cr0\n\t"
		      "wbinvd\n\t":"=r" (tmp)::"memory");

	if (size == 0) {
		/* The invalid bit is kept in the mask, so we simply clear the
		   relevant mask register to disable a range. */
		wrmsr (MTRRphysMask_MSR (reg), 0, 0);
	} else {
		/* Bit 32-35 of MTRRphysMask should be set to 1 */
		wrmsr (MTRRphysBase_MSR (reg), base | type, 0);
		wrmsr (MTRRphysMask_MSR (reg), ~(size - 1) | 0x800, 0x0F);
	}

	// turn cache back on. 
	asm volatile ("movl  %%cr0, %0\n\t"
		      "andl  $0x9fffffff, %0\n\t"
		      "movl  %0, %%cr0\n\t":"=r" (tmp)::"memory");

}

/* fms: find most sigificant bit set, stolen from Linux Kernel Source. */
static __inline__ unsigned int fms(unsigned int x)
{
	int r;

	__asm__("bsrl %1,%0\n\t"
	        "jnz 1f\n\t"
	        "movl $0,%0\n"
	        "1:" : "=r" (r) : "g" (x));
	return r;
}

/* setting up variable and fixed mtrr
 *
 * From Intel Vol. III Section 9.12.4, the Range Size and Base Alignment has some kind of requirement:
 *	1. The range size must be 2^N byte for N >= 12 (i.e 4KB minimum).
 *	2. The base address must be 2^N aligned, where the N here is equal to the N in previous
 *	   requirement. So a 8K range must be 8K aligned not 4K aligned.
 *
 * These requirement is meet by "decompositing" the ramsize into Sum(Cn * 2^n, n = [0..N], Cn = [0, 1]).
 * For Cm = 1, there is a WB range of 2^m size at base address Sum(Cm * 2^m, m = [N..n]).
 * A 124MB (128MB - 4MB SMA) example:
 * 	ramsize = 124MB == 64MB (at 0MB) + 32MB (at 64MB) + 16MB (at 96MB ) + 8MB (at 112MB) + 4MB (120MB).
 * But this wastes a lot of MTRR registers so we use another more "aggresive" way with Uncacheable Regions.
 *
 * In the Uncacheable Region scheme, we try to cover the whole ramsize by one WB region as possible,
 * If (an only if) this can not be done we will try to decomposite the ramesize, the mathematical formula
 * whould be ramsize = Sum(Cn * 2^n, n = [0..N], Cn = [-1, 0, 1]). For Cn = -1, a Uncachable Region is used.
 * The same 124MB example:
 * 	ramsize = 124MB == 128MB WB (at 0MB) + 4MB UC (at 124MB)
 * or a 156MB (128MB + 32MB - 4MB SMA) example:
 *	ramsize = 156MB == 128MB WB (at 0MB) + 32MB WB (at 128MB) + 4MB UC (at 156MB)
 */
#ifdef INTEL_PPRO_MTRR
#ifdef ENABLE_FIXED_AND_VARIABLE_MTRRS
void intel_set_mtrr(unsigned long rambase, unsigned long ramsizeK)
{
	unsigned int reg = 0;
	unsigned long range_wb, range_uc;

	DBG("\n");

	while (ramsizeK != 0 && reg <= 6) {
		intel_post(0x60 + reg);

		range_wb = 1 << (fms(ramsizeK) + 1);
		range_uc = range_wb - ramsizeK;

		if ((ramsizeK % range_uc) == 0) {
			DBG("Setting variable MTRR %d, base: %4dMB, range: %4dMB, type: WB\n",
			    reg, rambase >> 10, range_wb >> 10);
			intel_set_var_mtrr(reg++, rambase * 1024, range_wb * 1024,
					   MTRR_TYPE_WRBACK);
			rambase += ramsizeK;

			DBG("Setting variable MTRR %d, base: %4dMB, range: %4dMB, type: UC\n",
			    reg, rambase >> 10, range_uc >> 10);
			intel_set_var_mtrr(reg++, rambase * 1024, range_uc * 1024,
					   MTRR_TYPE_UNCACHABLE);
			ramsizeK = 0; /* effectivly a break */
		} else {
			range_wb >>= 1;

			DBG("Setting variable MTRR %d, base: %4dMB, range: %4dMB, type: WB\n",
			    reg, rambase >> 10, range_wb >> 10);
			intel_set_var_mtrr(reg++, rambase * 1024, range_wb * 1024,
					   MTRR_TYPE_WRBACK);

			rambase += range_wb;
			ramsizeK -= range_wb;
		}
	}

	intel_set_fixed_mtrr();

	/* enable fixed MTRR */
	intel_enable_fixed_mtrr();
	intel_enable_var_mtrr();
}
#else /* ENABLE_FIXED_AND_VARIABLE_MTRRS */
void intel_set_mtrr(unsigned long rambase, unsigned long ramsizeK)
{
	intel_set_var_mtrr(0, 0, ramsizeK * 1024, MTRR_TYPE_WRBACK);
	intel_enable_var_mtrr();
}
#endif /* ENABLE_FIXED_AND_VARIABLE_MTRRS */
#endif /* INTEL_PPRO_MTRR */
