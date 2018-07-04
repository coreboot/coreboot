/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
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

#include <delay.h>
#include <stdint.h>
#include <cpu/x86/tsc.h>
#include <cpu/x86/msr.h>

/**
 * Intel Rangeley CPUs always run the TSC at BCLK = 100MHz
 */

/* Simple 32- to 64-bit multiplication. Uses 16-bit words to avoid overflow.
 * This code is used to prevent use of libgcc's umoddi3.
 */
static inline void multiply_to_tsc(tsc_t *const tsc, const u32 a, const u32 b)
{
	tsc->lo = (a & 0xffff) * (b & 0xffff);
	tsc->hi = ((tsc->lo >> 16)
		+ ((a & 0xffff) * (b >> 16))
		+ ((b & 0xffff) * (a >> 16)));
	tsc->lo = ((tsc->hi & 0xffff) << 16) | (tsc->lo & 0xffff);
	tsc->hi = ((a >> 16) * (b >> 16)) + (tsc->hi >> 16);
}

void udelay(u32 us)
{
	u32 dword;
	tsc_t tsc, tsc1, tscd;
	msr_t msr;
	u32 fsb = 100, divisor;
	u32 d;			/* ticks per us */

	msr = rdmsr(MSR_PLATFORM_INFO);
	divisor = (msr.lo >> 8) & 0xff;

	d = fsb * divisor;
	multiply_to_tsc(&tscd, us, d);

	tsc1 = rdtsc();
	dword = tsc1.lo + tscd.lo;
	if ((dword < tsc1.lo) || (dword < tscd.lo)) {
		tsc1.hi++;
	}
	tsc1.lo = dword;
	tsc1.hi += tscd.hi;

	do {
		tsc = rdtsc();
	} while ((tsc.hi < tsc1.hi)
		 || ((tsc.hi == tsc1.hi) && (tsc.lo <= tsc1.lo)));
}
