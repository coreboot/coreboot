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

#include <mem.h>
#include <cpu/p6/msr.h>
#include <cpu/p6/mtrr.h>
#include <cpu/k7/mtrr.h>
#include <printk.h>
#include <subr.h>

#define arraysize(x)   (sizeof(x)/sizeof((x)[0]))

static unsigned int mtrr_msr[] = {
	MTRRfix64K_00000_MSR, MTRRfix16K_80000_MSR, MTRRfix16K_A0000_MSR,
	MTRRfix4K_C0000_MSR, MTRRfix4K_C8000_MSR, MTRRfix4K_D0000_MSR, MTRRfix4K_D8000_MSR,
	MTRRfix4K_E0000_MSR, MTRRfix4K_E8000_MSR, MTRRfix4K_F0000_MSR, MTRRfix4K_F8000_MSR,
};


static void intel_enable_fixed_mtrr(void)
{
	unsigned long low, high;

	rdmsr(MTRRdefType_MSR, low, high);
	low |= 0xc00;
	wrmsr(MTRRdefType_MSR, low, high);
}

static void intel_enable_var_mtrr(void)
{
	unsigned long low, high;

	rdmsr(MTRRdefType_MSR, low, high);
	low |= 0x800;
	wrmsr(MTRRdefType_MSR, low, high);
}


/* setting variable mtrr, comes from linux kernel source */
static void intel_set_var_mtrr(unsigned int reg, unsigned long basek, unsigned long sizek, unsigned char type)
{
	unsigned int tmp;
	unsigned long base_high, base_low;
	unsigned long  mask_high, mask_low;

	base_high = basek >> 22;
	base_low  = basek << 10;

	if (sizek < 4*1024*1024) {
		mask_high = 0x0F;
		mask_low = ~((sizek << 10) -1);
	}
	else {
		mask_high = 0x0F & (~((sizek >> 22) -1));
		mask_low = 0;
	}

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

	if (sizek == 0) {
		/* The invalid bit is kept in the mask, so we simply clear the
		   relevant mask register to disable a range. */
		wrmsr (MTRRphysMask_MSR (reg), 0, 0);
	} else {
		/* Bit 32-35 of MTRRphysMask should be set to 1 */
		wrmsr (MTRRphysBase_MSR(reg), base_low | type, base_high);
		wrmsr (MTRRphysMask_MSR(reg), mask_low | 0x800, mask_high);
	}

	// turn cache back on. 
	asm volatile ("movl  %%cr0, %0\n\t"
		      "andl  $0x9fffffff, %0\n\t"
		      "movl  %0, %%cr0\n\t":"=r" (tmp)::"memory");

}

/* setting variable mtrr, comes from linux kernel source */
void set_var_mtrr(unsigned int reg, unsigned long base, unsigned long size, unsigned char type)
{
	unsigned int tmp;

	if (reg >= 8)
		return;

	// it is recommended that we disable and enable cache when we 
	// do this. 
	/* Disable cache */
	/* Write back the cache and flush TLB */
	asm volatile (
		"movl  %%cr0, %0\n\t"
		"orl  $0x40000000, %0\n\t"
		"movl  %0, %%cr0\n\t"
		:"=r" (tmp)
		::"memory");

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
static inline unsigned int fms(unsigned int x)
{
	int r;

	__asm__("bsrl %1,%0\n\t"
	        "jnz 1f\n\t"
	        "movl $0,%0\n"
	        "1:" : "=r" (r) : "g" (x));
	return r;
}

/* fms: find least sigificant bit set */
static inline unsigned int fls(unsigned int x)
{
	int r;

	__asm__("bsfl %1,%0\n\t"
	        "jnz 1f\n\t"
	        "movl $32,%0\n"
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
/* 2 MTRRS are reserved for the operating system */
#define BIOS_MTRRS 6
#define OS_MTRRS   2
#define MTRRS        (BIOS_MTRRS + OS_MTRRS)


static void set_fixed_mtrrs(unsigned int first, unsigned int last, unsigned char type)
{
	unsigned int i;
	unsigned int fixed_msr = NUM_FIXED_RANGES >> 3;
	unsigned long low, high;
	low = high = 0; /* Shut up gcc */
	for(i = first; i < last; i++) {
		/* When I switch to a new msr read it in */
		if (fixed_msr != i >> 3) {
			/* But first write out the old msr */
			if (fixed_msr < (NUM_FIXED_RANGES >> 3)) {
				wrmsr(mtrr_msr[fixed_msr], low, high);
			}
			fixed_msr = i>>3;
			rdmsr(mtrr_msr[fixed_msr], low, high);
		}
		if ((i & 7) < 4) {
			low &= ~(0xff << ((i&3)*8));
			low |= type << ((i&3)*8);
		} else {
			high &= ~(0xff << ((i&3)*8));
			high |= type << ((i&3)*8);
		}
	}
	/* Write out the final msr */
	if (fixed_msr < (NUM_FIXED_RANGES >> 3)) {
		wrmsr(mtrr_msr[fixed_msr], low, high);
	}
}

static unsigned fixed_mtrr_index(unsigned long addrk)
{
	unsigned index;
	index = (addrk - 0) >> 6;
	if (index >= 8) {
		index = ((addrk - 8*64) >> 4) + 8;
	}
	if (index >= 24) {
		index = ((addrk - (8*64 + 16*16)) >> 2) + 24;
	}
	if (index > NUM_FIXED_RANGES) {
		index = NUM_FIXED_RANGES;
	}
	return index;
}

static unsigned int range_to_mtrr(unsigned int reg, 
	unsigned long range_startk, unsigned long range_sizek)
{
	if (!range_sizek || (reg >= BIOS_MTRRS)) {
		return reg;
	}
	while(range_sizek) {
		unsigned long max_align, align;
		unsigned long sizek;
		/* Compute the maximum size I can make a range */
		max_align = fls(range_startk);
		align = fms(range_sizek); 
		if (align > max_align) {
			align = max_align;
		}
		sizek = 1 << align;
		printk_debug("Setting variable MTRR %d, base: %4dMB, range: %4dMB, type WB\n",
			reg, range_startk >>10, sizek >> 10);
		intel_set_var_mtrr(reg++, range_startk, sizek, MTRR_TYPE_WRBACK);
		range_startk += sizek;
		range_sizek -= sizek;
		if (reg >= BIOS_MTRRS)
			break;
	}
	return reg;
}

void setup_mtrrs(struct mem_range *mem)
{
	/* Try this the simple way of incrementally adding together
	 * mtrrs.  If this doesn't work out we can get smart again 
	 * and clear out the mtrrs.
	 */
	struct mem_range *memp;
	unsigned long range_startk, range_sizek;
	unsigned int reg;

	printk_debug("\n");
	/* Initialized the fixed_mtrrs to uncached */
	printk_debug("Setting fixed MTRRs(%d-%d) type: UC\n", 
		0, NUM_FIXED_RANGES);
	set_fixed_mtrrs(0, NUM_FIXED_RANGES, MTRR_TYPE_UNCACHABLE);

	/* Now see which of the fixed mtrrs cover ram.
	 */
	for(memp = mem; memp->sizek; memp++) {
		unsigned int start_mtrr;
		unsigned int last_mtrr;
		start_mtrr = fixed_mtrr_index(memp->basek);
		last_mtrr = fixed_mtrr_index(memp->basek + memp->sizek);
		if (start_mtrr >= NUM_FIXED_RANGES) {
			break;
		}
		printk_debug("Setting fixed MTRRs(%d-%d) type: WB\n",
			start_mtrr, last_mtrr);
		set_fixed_mtrrs(start_mtrr, last_mtrr, MTRR_TYPE_WRBACK);
	}
	printk_debug("DONE fixed MTRRs\n");
	/* Cache as many memory areas as possible */
	/* FIXME is there an algorithm for computing the optimal set of mtrrs? 
	 * In some cases it is definitely possible to do better.
	 */
	range_startk = 0;
	range_sizek = 0;
	reg = 0;
	for (memp = mem; memp->sizek; memp++) {
		/* See if I can merge with the last range 
		 * Either I am below 1M and the fixed mtrrs handle it, or
		 * the ranges touch.
		 */
		if ((memp->basek <= 1024) || (range_startk + range_sizek == memp->basek)) {
			unsigned long endk = memp->basek + memp->sizek;
			range_sizek = endk - range_startk;
			continue;
		}
		/* Write the range mtrrs */
		if (range_sizek != 0) {
			reg = range_to_mtrr(reg, range_startk, range_sizek);
			range_startk = 0;
			range_sizek = 0;
			if (reg >= BIOS_MTRRS)
				break;
		}
		/* Allocate an msr */
		range_startk = memp->basek;
		range_sizek = memp->sizek;
	}
	/* Write the last range */
	reg = range_to_mtrr(reg, range_startk, range_sizek);
	printk_debug("DONE variable MTRRs\n");
	printk_debug("Clear out the extra MTRR's\n");
	/* Clear out the extra MTRR's */
	while(reg < MTRRS) {
		intel_set_var_mtrr(reg++, 0, 0, 0);
	}
	/* enable fixed MTRR */
	printk_debug("call intel_enable_fixed_mtrr()\n");
	intel_enable_fixed_mtrr();
	printk_debug("call intel_enable_var_mtrr()\n");
	intel_enable_var_mtrr();
	printk_debug("Leave %s\n", __FUNCTION__);
}
