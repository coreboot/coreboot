/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/lapic_def.h>
#include <cpu/x86/msr.h>
#include <smp/node.h>

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
