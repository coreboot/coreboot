/* SPDX-License-Identifier: GPL-2.0-only */

#include <smp/node.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic_def.h>

#if CONFIG(SMP)
int boot_cpu(void)
{
	int bsp;
	msr_t msr;
	msr = rdmsr(LAPIC_BASE_MSR);
	bsp = !!(msr.lo & (1 << 8));
	return bsp;
}
#endif
