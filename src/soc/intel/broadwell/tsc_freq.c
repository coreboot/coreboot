/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/msr.h>
#include <cpu/x86/tsc.h>
#include <soc/cpu.h>
#include <soc/msr.h>

unsigned long tsc_freq_mhz(void)
{
	msr_t platform_info;

	platform_info = rdmsr(MSR_PLATFORM_INFO);
	return CPU_BCLK * ((platform_info.lo >> 8) & 0xff);
}
