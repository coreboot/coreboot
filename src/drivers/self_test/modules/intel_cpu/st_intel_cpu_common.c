/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <bootstate.h>
#include <console/console.h>
#include <cpu/x86/mp.h>
#include <intelblocks/mp_init.h>
#include <self_test.h>
#include <smp/spinlock.h>

#include "st_intel_cpu_common.h"

struct msr_per_cpu msr_per_cpu;
DECLARE_SPIN_LOCK(st_lock);

void st_readmsr(void *unused)
{
	spin_lock(&st_lock);
	msr_per_cpu.value[cpu_index()] = rdmsr(msr_per_cpu.reg);
	spin_unlock(&st_lock);
}

void st_readmsr_all_cpus(unsigned int reg)
{
	memset(&msr_per_cpu, 0, sizeof(msr_per_cpu));
	msr_per_cpu.reg = reg;
	if (mp_run_on_all_cpus(st_readmsr, NULL) != CB_SUCCESS) {
		st_debug("mp_run_on_all_cpus for st_readmsr(0x%x) failed\n", reg);
		return;
	}
}
