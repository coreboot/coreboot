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

#include <stdint.h>
#include <cpu/x86/tsc.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/speedstep.h>
#include "delay.h"

/**
 * Intel Core(tm) CPUs always run the TSC at the maximum possible CPU clock
 */
static void _udelay(const u32 us, const u32 numerator, const int total)
{
	u32 dword;
	tsc_t tsc, tsc1, tscd;
	msr_t msr;
	u32 fsb = 0, divisor;
	u32 d;			/* ticks per us */

	msr = rdmsr(MSR_FSB_FREQ);
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
	case 2:
		fsb = 800;
		break;
	case 0:
		fsb = 1067;
		break;
	case 4:
		fsb = 1333;
		break;
	case 6:
		fsb = 1600;
		break;
	}

	msr = rdmsr(0x198);
	divisor = (msr.hi >> 8) & 0x1f;

	d = ((fsb * divisor) / numerator) / 4;	/* CPU clock is always a quarter. */

	multiply_to_tsc(&tscd, us, d);

	if (!total) {
		tsc1 = rdtsc();
		dword = tsc1.lo + tscd.lo;
		if ((dword < tsc1.lo) || (dword < tscd.lo)) {
			tsc1.hi++;
		}
		tsc1.lo = dword;
		tsc1.hi += tscd.hi;
	} else {
		tsc1 = tscd;
	}

	do {
		tsc = rdtsc();
	} while ((tsc.hi < tsc1.hi)
		 || ((tsc.hi == tsc1.hi) && (tsc.lo < tsc1.lo)));
}

void udelay(const u32 us)
{
	_udelay(us, 1, 0);
}

void ns100delay(const u32 ns100)
{
	_udelay(ns100, 10, 0);
}

void udelay_from_reset(const u32 us)
{
	_udelay(us, 1, 1);
}
