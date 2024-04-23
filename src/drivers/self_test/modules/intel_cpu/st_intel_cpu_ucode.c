/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <self_test.h>

#include "st_intel_cpu.h"

#include <cpu/x86/msr.h>
#include <arch/cpu.h>
#include <smp/spinlock.h>
#include <cpu/x86/mp.h>
#include <intelblocks/mp_init.h>

struct msr_per_cpu {
	unsigned int reg;
	msr_t value[CONFIG_MAX_CPUS];
};

static struct msr_per_cpu msr_per_cpu;
static int cpu_count;
DECLARE_SPIN_LOCK(st_lock);

static void st_readmsr(void *unused)
{
	spin_lock(&st_lock);
	msr_per_cpu.value[cpu_index()] = rdmsr(msr_per_cpu.reg);
	spin_unlock(&st_lock);
}

static void st_readmsr_all_cpus(unsigned int reg)
{
	memset(&msr_per_cpu, 0, sizeof(msr_per_cpu));
	msr_per_cpu.reg = reg;
	if (mp_run_on_all_cpus(st_readmsr, NULL) != CB_SUCCESS) {
		st_debug("mp_run_on_all_cpus for st_readmsr(0x%x) failed\n", reg);
		return;
	}
}

struct st_status st_intel_microcode(void)
{
	struct st_status ret = {.status = ST_PASSED, .error_code = 0};

	st_debug("Starting %s\n", __func__);
	st_debug("Test: Is microcode loaded? MSR(IA32_BIOS_SIGN_ID/0x%x)\n", IA32_BIOS_SIGN_ID);
	cpu_count = get_cpu_count();
	st_debug("CPU count is %d\n", cpu_count);
	st_readmsr_all_cpus(IA32_BIOS_SIGN_ID);

	for (int i = 0; i < cpu_count; i++) {
		st_debug("[cpu%d]: 0x%x %x\n", i, msr_per_cpu.value[i].hi, msr_per_cpu.value[i].lo);
		if (!((msr_per_cpu.value[i].hi != 0) || (msr_per_cpu.value[i].lo != 0))) {
			ret.status = ST_FAILED;
			ret.error_code = (uint16_t)i;
		}
	}

	return ret;
}
