/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/tsc.h>
#include <console/console.h>

static unsigned long mhz;

/* Use this default TSC frequency when it can not be correctly calculated.
   Higher numbers are safer as it will result in longer delays using TSC */
#define TSC_DEFAULT_FREQ_MHZ	4000

unsigned long tsc_freq_mhz(void)
{
	msr_t msr;
	uint8_t cpufid;
	uint8_t cpudid;
	uint8_t high_state;

	if (mhz)
		return mhz;

	high_state = rdmsr(PS_LIM_REG).lo & 0x7;
	msr = rdmsr(PSTATE_0_MSR + high_state);
	if (!(msr.hi & 0x80000000))
		die("Unknown error: cannot determine P-state 0\n");

	cpufid = (msr.lo & 0xff);
	cpudid = (msr.lo & 0x3f00) >> 8;

	/* normally core frequency is calculated as (fid * 25) / (did / 8) */
	if (!cpudid) {
		mhz = TSC_DEFAULT_FREQ_MHZ;
		printk(BIOS_ERR, "Invalid divisor, set TSC frequency to %ldMHz\n", mhz);
	} else if ((cpudid >= 8) && (cpudid < 0x3c)) {
		mhz = (200 * cpufid) / cpudid;
	} else {
		mhz = 25 * cpufid;
		printk(BIOS_ERR, "Invalid frequency divisor 0x%x, assume 1\n", cpudid);
	}

	return mhz;
}
