/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <bootstate.h>
#include <console/console.h>
#include <intelblocks/mp_init.h>
#include <self_test.h>

#include "st_intel_cpu.h"
#include "st_intel_cpu_common.h"

extern struct msr_per_cpu msr_per_cpu;

struct st_status st_intel_microcode(void)
{
	struct st_status ret = {.status = ST_PASSED, .error_code = 0};
	int cpu_count;

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
