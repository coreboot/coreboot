/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/tsc.h>
#include <console/console.h>
#include <soc/msr.h>

/* Use this default TSC frequency when it can not be correctly calculated.
   Higher numbers are safer as it will result in longer delays using TSC */
#define TSC_DEFAULT_FREQ_MHZ	4000

unsigned long tsc_freq_mhz(void)
{
	union pstate_msr pstate_reg;
	static unsigned long mhz;

	if (mhz)
		return mhz;

	/* The TSC counts at the P0 rate, whatever the P-state limit is */
	pstate_reg.raw = rdmsr(PSTATE_MSR(get_pstate_0_reg())).raw;
	if (!pstate_reg.pstate_en)
		die("Unknown error: cannot determine P-state 0\n");

	mhz = get_pstate_core_freq(pstate_reg);

	if (!mhz) {
		mhz = TSC_DEFAULT_FREQ_MHZ;
		printk(BIOS_ERR, "Invalid divisor, set TSC frequency to %ldMHz\n", mhz);
	}

	return mhz;
}
