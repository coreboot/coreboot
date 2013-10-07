/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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
#include <cpu/x86/msr.h>
#include <cpu/x86/tsc.h>
#include <baytrail/msr.h>
#if !defined(__PRE_RAM__)
#include <baytrail/ramstage.h>
#else
#include <baytrail/romstage.h>
#endif


#define BCLK 100 /* 100 MHz */
unsigned long tsc_freq_mhz(void)
{
	msr_t platform_info;

	platform_info = rdmsr(MSR_PLATFORM_INFO);
	return BCLK * ((platform_info.lo >> 8) & 0xff);
}

void set_max_freq(void)
{
	msr_t perf_ctl;
	msr_t msr;

	/* Enable speed step. */
	msr = rdmsr(MSR_IA32_MISC_ENABLES);
	msr.lo |= (1 << 16);
	wrmsr(MSR_IA32_MISC_ENABLES, msr);

	/* Set guranteed ratio [21:16] from IACORE_RATIOS to bits [15:8] of
	* the PERF_CTL. */
	msr = rdmsr(MSR_IACORE_RATIOS);
	perf_ctl.lo = (msr.lo & 0x3f0000) >> 8;
	/* Set guranteed vid [21:16] from IACORE_VIDS to bits [7:0] of
	* the PERF_CTL. */
	msr = rdmsr(MSR_IACORE_VIDS);
	perf_ctl.lo |= (msr.lo & 0x7f0000) >> 16;
	perf_ctl.hi = 0;

	wrmsr(MSR_IA32_PERF_CTL, perf_ctl);
}
