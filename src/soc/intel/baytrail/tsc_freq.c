/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stdint.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/tsc.h>
#include <soc/msr.h>

unsigned int bus_freq_khz(void)
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
	unsigned int bclk_khz = bus_freq_khz();

	if (!bclk_khz)
		return 0;

	platform_info = rdmsr(MSR_PLATFORM_INFO);
	return (bclk_khz * ((platform_info.lo >> 8) & 0xff)) / 1000;
}

void set_max_freq(void)
{
	msr_t perf_ctl;
	msr_t msr;

	/* Enable speed step. */
	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= (1 << 16);
	wrmsr(IA32_MISC_ENABLE, msr);

	/* Set guaranteed ratio [21:16] from IACORE_RATIOS to bits [15:8] of
	* the PERF_CTL. */
	msr = rdmsr(MSR_IACORE_RATIOS);
	perf_ctl.lo = (msr.lo & 0x3f0000) >> 8;
	/* Set guaranteed vid [21:16] from IACORE_VIDS to bits [7:0] of
	* the PERF_CTL. */
	msr = rdmsr(MSR_IACORE_VIDS);
	perf_ctl.lo |= (msr.lo & 0x7f0000) >> 16;
	perf_ctl.hi = 0;

	wrmsr(IA32_PERF_CTL, perf_ctl);
}
