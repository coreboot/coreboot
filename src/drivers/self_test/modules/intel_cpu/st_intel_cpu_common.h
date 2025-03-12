/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ST_INTEL_CPU_COMMON_H_
#define _ST_INTEL_CPU_COMMON_H_

#include <cpu/x86/msr.h>

struct msr_per_cpu {
	unsigned int reg;
	msr_t value[CONFIG_MAX_CPUS];
};

void st_readmsr(void *unused);
void st_readmsr_all_cpus(unsigned int reg);

#endif /* _ST_INTEL_CPU_COMMON_H_ */
