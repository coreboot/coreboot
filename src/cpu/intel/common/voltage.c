/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/msr.h>
#include <smbios.h>

/* This is not an architectural MSR. */
#define MSR_PERF_STATUS 0x198

unsigned int smbios_cpu_get_voltage(void)
{
	return (rdmsr(MSR_PERF_STATUS).hi & 0xffff) * 10 / 8192;
}
