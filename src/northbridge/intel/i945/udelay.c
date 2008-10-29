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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <cpu/x86/tsc.h>
#include <cpu/x86/msr.h>

/**
 * Intel Core(tm) cpus always run the TSC at the maximum possible CPU clock
 */

static void udelay(u32 us)
{
	u32 dword;
	tsc_t tsc, tsc1, tscd;
	msr_t msr;
	u32 fsb = 0, divisor;
	u32 d;			
	u32 dn = 0x1000000 / 2;	

	msr = rdmsr(0xcd);
	switch (msr.lo & 0x07) {
	case 5:
		fsb = 400;
		break;
	case 1:
		fsb = 533;
		break;
	case 3:
		fsb = 667;
		break;
	}

	msr = rdmsr(0x198);
	divisor = (msr.hi >> 8) & 0x1f;

	d = fsb * divisor;

	tscd.hi = us / dn;
	tscd.lo = (us - tscd.hi * dn) * d;

	tsc1 = rdtsc();
	dword = tsc1.lo + tscd.lo;
	if ((dword < tsc1.lo) || (dword < tscd.lo)) {
		tsc1.hi++;
	}
	tsc1.lo = dword;
	tsc1.hi += tscd.hi;

	do {
		tsc = rdtsc();
	} while ((tsc.hi > tsc1.hi)
		 || ((tsc.hi == tsc1.hi) && (tsc.lo > tsc1.lo)));

}
