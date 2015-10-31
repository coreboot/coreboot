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
 */

#include <stdint.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/tsc.h>
#include <soc/msr.h>

unsigned bus_freq_khz(void)
{
	msr_t clk_info = rdmsr(MSR_BSEL_CR_OVERCLOCK_CONTROL);
	switch (clk_info.lo & 0x3) {
	case 0:
		return 83333;
	case 1:
		return 100000;
	case 2:
		return 133333;
	case 3:
		return 116666;
	default:
		return 0;
	}
}

unsigned long tsc_freq_mhz(void)
{
	msr_t platform_info;
	unsigned bclk_khz = bus_freq_khz();

	if (!bclk_khz)
		return 0;

	platform_info = rdmsr(MSR_PLATFORM_INFO);
	return (bclk_khz * ((platform_info.lo >> 8) & 0xff)) / 1000;
}

#if !defined(__SMM__)
#if !defined(__PRE_RAM__)
#include <soc/ramstage.h>
#else
#include <soc/romstage.h>
#endif

void set_max_freq(void)
{
	msr_t perf_ctl;
	msr_t msr;

	/* Enable speed step. */
	msr = rdmsr(MSR_IA32_MISC_ENABLES);
	msr.lo |= (1 << 16);
	wrmsr(MSR_IA32_MISC_ENABLES, msr);

	/* Set guaranteed ratio [21:16] from IACORE_RATIOS to bits [15:8] of
	* the PERF_CTL. */
	msr = rdmsr(MSR_IACORE_RATIOS);
	perf_ctl.lo = (msr.lo & 0x3f0000) >> 8;
	/* Set guaranteed vid [21:16] from IACORE_VIDS to bits [7:0] of
	* the PERF_CTL. */
	msr = rdmsr(MSR_IACORE_VIDS);
	perf_ctl.lo |= (msr.lo & 0x7f0000) >> 16;
	perf_ctl.hi = 0;

	wrmsr(MSR_IA32_PERF_CTL, perf_ctl);
}

#endif /* __SMM__ */
