/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <soc/cpu.h>
#include <soc/msr.h>
#include <soc/romstage.h>

void set_max_freq(void)
{
	msr_t msr, perf_ctl, platform_info;

	/* Check for configurable TDP option */
	platform_info = rdmsr(MSR_PLATFORM_INFO);

	if ((platform_info.hi >> 1) & 3) {
		/* Set to nominal TDP ratio */
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		perf_ctl.lo = (msr.lo & 0xff) << 8;
	} else {
		/* Platform Info bits 15:8 give max ratio */
		msr = rdmsr(MSR_PLATFORM_INFO);
		perf_ctl.lo = msr.lo & 0xff00;
	}

	perf_ctl.hi = 0;
	wrmsr(IA32_PERF_CTL, perf_ctl);

	printk(BIOS_DEBUG, "CPU: frequency set to %d MHz\n",
	       ((perf_ctl.lo >> 8) & 0xff) * CPU_BCLK);
}
