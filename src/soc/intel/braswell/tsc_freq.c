/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/msr.h>
#include <cpu/x86/tsc.h>
#include <soc/msr.h>

static const unsigned int cpu_bus_clk_freq_table[] = {
	83333,
	100000,
	133333,
	116666,
	80000,
	93333,
	90000,
	88900,
	87500
};

unsigned int cpu_bus_freq_khz(void)
{
	msr_t clk_info = rdmsr(MSR_BSEL_CR_OVERCLOCK_CONTROL);

	if ((clk_info.lo & 0xf) < ARRAY_SIZE(cpu_bus_clk_freq_table))
		return cpu_bus_clk_freq_table[clk_info.lo & 0xf];

	return 0;
}

unsigned long tsc_freq_mhz(void)
{
	msr_t platform_info;
	unsigned int bclk_khz = cpu_bus_freq_khz();

	if (!bclk_khz)
		return 0;

	platform_info = rdmsr(MSR_PLATFORM_INFO);
	return (bclk_khz * ((platform_info.lo >> 8) & 0xff)) / 1000;
}

void set_max_freq(void)
{
	msr_t perf_ctl;
	msr_t msr;

	/* Enable Intel SpeedStep */
	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= (1 << 16);
	wrmsr(IA32_MISC_ENABLE, msr);

	/* Enable Burst Mode */
	msr = rdmsr(IA32_MISC_ENABLE);
	msr.hi = 0;
	wrmsr(IA32_MISC_ENABLE, msr);

	/* Set guaranteed ratio [21:16] from IACORE_RATIOS to bits [15:8] of the PERF_CTL */
	msr = rdmsr(MSR_IACORE_TURBO_RATIOS);
	perf_ctl.lo = (msr.lo & 0x003f0000) >> 8;

	/* Set guaranteed vid [21:16] from IACORE_VIDS to bits [7:0] of the PERF_CTL */
	msr = rdmsr(MSR_IACORE_TURBO_VIDS);
	perf_ctl.lo |= (msr.lo & 0x007f0000) >> 16;
	perf_ctl.hi = 0;

	wrmsr(IA32_PERF_CTL, perf_ctl);
}
