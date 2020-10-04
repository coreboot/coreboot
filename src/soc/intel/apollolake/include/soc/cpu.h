/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_APOLLOLAKE_CPU_H_
#define _SOC_APOLLOLAKE_CPU_H_

#include <cpu/x86/msr.h>
#include <intelblocks/msr.h>

struct device;
void apollolake_init_cpus(struct device *dev);
void mainboard_devtree_update(struct device *dev);

/* Flush L1D to L2 */
static inline void flush_l1d_to_l2(void)
{
	msr_t msr = rdmsr(MSR_POWER_MISC);
	msr.lo |= FLUSH_DL1_L2;
	wrmsr(MSR_POWER_MISC, msr);
}
#endif /* _SOC_APOLLOLAKE_CPU_H_ */
