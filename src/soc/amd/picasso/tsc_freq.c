/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/tsc.h>
#include <console/console.h>

static unsigned long mhz;

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

	cpufid = (msr.lo & 0x3f);
	cpudid = (msr.lo & 0x1c0) >> 6;

	mhz = (100 * (cpufid + 0x10)) / (0x01 << cpudid);
	return mhz;
}
