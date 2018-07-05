/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <inttypes.h>
#include <console/console.h>
#include <string.h>
#include "mct_d.h"
#include "mct_d_gcc.h"
#include <cpu/amd/mtrr.h>

static void SetMTRRrangeWB_D(u32 Base, u32 *pLimit, u32 *pMtrrAddr);
static void SetMTRRrange_D(u32 Base, u32 *pLimit, u32 *pMtrrAddr, u16 MtrrType);

void CPUMemTyping_D(struct MCTStatStruc *pMCTstat,
			 struct DCTStatStruc *pDCTstatA)
{
	/* BSP only.  Set the fixed MTRRs for common legacy ranges.
	 * Set TOP_MEM and TOM2.
	 * Set some variable MTRRs with WB Uncacheable type.
	 */

	u32 Bottom32bIO, Bottom40bIO, Cache32bTOP;
	u32 val;
	u32 addr;
	u32 lo, hi;

	/* Set temporary top of memory from Node structure data.
	 * Adjust temp top of memory down to accommodate 32-bit IO space.
	 * Bottom40bIO = top of memory, right justified 8 bits
	 *	(defines dram versus IO space type)
	 * Bottom32bIO = sub 4GB top of memory, right justified 8 bits
	 *	(defines dram versus IO space type)
	 * Cache32bTOP = sub 4GB top of WB cacheable memory,
	 *	right justified 8 bits
	 */

	val = mctGet_NVbits(NV_BottomIO);
	if (val == 0)
		val++;

	Bottom32bIO = val << (24-8);

	val = pMCTstat->SysLimit + 1;
	if (val <= _4GB_RJ8) {
		Bottom40bIO = 0;
		if (Bottom32bIO >= val)
			Bottom32bIO = val;
	} else {
		Bottom40bIO = val;
	}

	Cache32bTOP = Bottom32bIO;

	/*======================================================================
	 Set default values for CPU registers
	======================================================================*/

	/* NOTE : For coreboot, we don't need to set mtrr enables here because
	they are still enable from cache_as_ram.inc */

	addr = 0x250;
	lo = 0x1E1E1E1E;
	hi = lo;
	_WRMSR(addr, lo, hi);		/* 0 - 512K = WB Mem */
	addr = 0x258;
	_WRMSR(addr, lo, hi);		/* 512K - 640K = WB Mem */

	/*======================================================================
	  Set variable MTRR values
	 ======================================================================*/
	/* NOTE: for coreboot change from 0x200 to 0x204: coreboot is using
		0x200, 0x201 for [1M, CONFIG_TOP_MEM)
		0x202, 0x203 for ROM Caching
		 */
	addr = 0x204;	/* MTRR phys base 2*/
			/* use TOP_MEM as limit*/
			/* Limit = TOP_MEM|TOM2*/
			/* Base = 0*/
	printk(BIOS_DEBUG, "\t CPUMemTyping: Cache32bTOP:%x\n", Cache32bTOP);
	SetMTRRrangeWB_D(0, &Cache32bTOP, &addr);
				/* Base */
				/* Limit */
				/* MtrrAddr */
	if (addr == -1)		/* ran out of MTRRs?*/
		pMCTstat->GStatus |= 1<<GSB_MTRRshort;

	pMCTstat->Sub4GCacheTop = Cache32bTOP<<8;

	/*======================================================================
	 Set TOP_MEM and TOM2 CPU registers
	======================================================================*/
	addr = TOP_MEM;
	lo = Bottom32bIO<<8;
	hi = Bottom32bIO>>24;
	_WRMSR(addr, lo, hi);
	printk(BIOS_DEBUG, "\t CPUMemTyping: Bottom32bIO:%x\n", Bottom32bIO);
	printk(BIOS_DEBUG, "\t CPUMemTyping: Bottom40bIO:%x\n", Bottom40bIO);
	if (Bottom40bIO) {
		hi = Bottom40bIO >> 24;
		lo = Bottom40bIO << 8;
		addr += 3;		/* TOM2 */
		_WRMSR(addr, lo, hi);
	}
	addr = 0xC0010010;		/* SYS_CFG */
	_RDMSR(addr, &lo, &hi);
	if (Bottom40bIO) {
		lo |= (1<<21);		/* MtrrTom2En = 1 */
		lo |= (1<<22);		/* Tom2ForceMemTypeWB */
	} else {
		lo &= ~(1<<21);		/* MtrrTom2En = 0 */
		lo &= ~(1<<22);		/* Tom2ForceMemTypeWB */
	}
	_WRMSR(addr, lo, hi);
}

static void SetMTRRrangeWB_D(u32 Base, u32 *pLimit, u32 *pMtrrAddr)
{
	/*set WB type*/
	SetMTRRrange_D(Base, pLimit, pMtrrAddr, 6);
}

static void SetMTRRrange_D(u32 Base, u32 *pLimit, u32 *pMtrrAddr, u16 MtrrType)
{
	/* Program MTRRs to describe given range as given cache type.
	 * Use MTRR pairs starting with the given MTRRphys Base address,
	 * and use as many as is required up to (excluding) MSR 020C, which
	 * is reserved for OS.
	 *
	 * "Limit" in the context of this procedure is not the numerically
	 * correct limit, but rather the Last address+1, for purposes of coding
	 * efficiency and readability.  Size of a region is then Limit-Base.
	 *
	 * 1. Size of each range must be a power of two
	 * 2. Each range must be naturally aligned (Base is same as size)
	 *
	 * There are two code paths: the ascending path and descending path
	 * (analogous to bsf and bsr), where the next limit is a function of the
	 * next set bit in a forward or backward sequence of bits (as a function
	 * of the Limit). We start with the ascending path, to ensure that
	 * regions are naturally aligned, then we switch to the descending path
	 * to maximize MTRR usage efficiency. Base = 0 is a special case where we
	 * start with the descending path. Correct Mask for region is
	 * 2comp(Size-1)-1, which is 2comp(Limit-Base-1)-1
	 */

	u32 curBase, curLimit, curSize;
	u32 val, valx;
	u32 addr;

	val = curBase = Base;
	curLimit = *pLimit;
	addr = *pMtrrAddr;
	while ((addr >= 0x200) && (addr < 0x20C) && (val < *pLimit)) {
		/* start with "ascending" code path */
		/* alignment (largest block size)*/
		valx = 1 << bsf(curBase);
		curSize = valx;

		/* largest legal limit, given current non-zero range Base*/
		valx += curBase;
		if ((curBase == 0) || (*pLimit < valx)) {
			/* flop direction to "descending" code path*/
			valx = 1<<bsr(*pLimit - curBase);
			curSize = valx;
			valx += curBase;
		}
		curLimit = valx;		/*eax = curBase, edx = curLimit*/
		valx = val>>24;
		val <<= 8;

		/* now program the MTRR */
		val |= MtrrType;		/* set cache type (UC or WB)*/
		_WRMSR(addr, val, valx);	/* prog. MTRR with current region Base*/
		val = ((~(curSize - 1))+1) - 1;	/* Size-1*/ /*Mask = 2comp(Size-1)-1*/
		valx = (val >> 24) | (0xff00);	/* GH have 48 bits addr */
		val <<= 8;
		val |= (1 << 11);			/* set MTRR valid*/
		addr++;
		_WRMSR(addr, val, valx);	/* prog. MTRR with current region Mask*/
		val = curLimit;
		curBase = val;			/* next Base = current Limit (loop exit)*/
		addr++;				/* next MTRR pair addr */
	}
	if (val < *pLimit) {
		*pLimit = val;
		addr = -1;
	}
	*pMtrrAddr = addr;
}

void UMAMemTyping_D(struct MCTStatStruc *pMCTstat, struct DCTStatStruc *pDCTstatA)
{
	/* UMA memory size may need splitting the MTRR configuration into two
	 * Before training use NB_BottomIO or the physical memory size to set the MTRRs.
	 * After training, add UMAMemTyping function to reconfigure the MTRRs based on
	 * NV_BottomUMA (for UMA systems only).
	 * This two-step process allows all memory to be cached for training
	 */

	u32 Bottom32bIO, Cache32bTOP;
	u32 val;
	u32 addr;
	u32 lo, hi;

	/*======================================================================
	 * Adjust temp top of memory down to accommodate UMA memory start
	 *======================================================================*/
	/* Bottom32bIO = sub 4GB top of memory, right justified 8 bits
	 * (defines dram versus IO space type)
	 * Cache32bTOP = sub 4GB top of WB cacheable memory, right justified 8 bits */

	Bottom32bIO = pMCTstat->Sub4GCacheTop >> 8;

	val = mctGet_NVbits(NV_BottomUMA);
	if (val == 0)
		val++;

	val <<= (24-8);
	if (val < Bottom32bIO) {
		Cache32bTOP = val;
		pMCTstat->Sub4GCacheTop = val;

		/*======================================================================
		 * Clear variable MTRR values
		 *======================================================================*/
		addr = 0x200;
		lo = 0;
		hi = lo;
		while (addr < 0x20C) {
			_WRMSR(addr, lo, hi);		/* prog. MTRR with current region Mask */
			addr++;						/* next MTRR pair addr */
		}

		/*======================================================================
		 * Set variable MTRR values
		 *======================================================================*/
		printk(BIOS_DEBUG, "\t UMAMemTyping_D: Cache32bTOP:%x\n", Cache32bTOP);
		SetMTRRrangeWB_D(0, &Cache32bTOP, &addr);
		if (addr == -1)		/* ran out of MTRRs?*/
			pMCTstat->GStatus |= 1<<GSB_MTRRshort;
	}
}
