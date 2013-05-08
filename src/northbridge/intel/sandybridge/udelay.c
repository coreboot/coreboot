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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <delay.h>
#include <stdint.h>
#include <cpu/x86/tsc.h>
#include <cpu/x86/msr.h>

/**
 * Intel Sandy Bridge/Ivy Bridge CPUs always run the TSC at BCLK=100MHz
 */

void udelay(u32 us)
{
	u32 dword;
	tsc_t tsc, tsc1, tscd;
	msr_t msr;
	u32 fsb = 100, divisor;
	u32 d;			/* ticks per us */

	msr = rdmsr(0xce);
	divisor = (msr.lo >> 8) & 0xff;

	d = fsb * divisor;	/* On Core/Core2 this is divided by 4 */
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
		 || ((tsc.hi == tsc1.hi) && (tsc.lo < tsc1.lo)));
}
