/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_APOLLOLAKE_CPU_H_
#define _SOC_APOLLOLAKE_CPU_H_

#include <cpu/x86/msr.h>
#include <intelblocks/msr.h>
#include <soc/msr.h>

struct device;
void mainboard_devtree_update(struct device *dev);

/* Flush L1D to L2 */
static inline void flush_l1d_to_l2(void)
{
	msr_t msr = rdmsr(MSR_POWER_MISC);
	msr.lo |= FLUSH_DL1_L2;
	wrmsr(MSR_POWER_MISC, msr);
}

/* Clear MCA in Bank 4 */
static inline void clear_mca_bank4(void)
{
	msr_t msr;
	msr.hi = 0;
	msr.lo = 0;
	wrmsr(IA32_MC_STATUS(4), msr);
}

#endif /* _SOC_APOLLOLAKE_CPU_H_ */
