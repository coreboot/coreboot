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
	struct st_status ret = {.status = ST_PASSED, .error_code = ST_ERR_NONE};
	int cpu_count;

	st_debug("Starting %s\n", __func__);
	st_debug("Test: Is microcode loaded? MSR(IA32_BIOS_SIGN_ID/0x%x)\n", IA32_BIOS_SIGN_ID);

	cpu_count = get_cpu_count();
	st_debug("CPU count is %d\n", cpu_count);
	if (cpu_count <= 0 || st_readmsr_all_cpus(IA32_BIOS_SIGN_ID) != CB_SUCCESS) {
		return (struct st_status){
			.status = ST_FAILED,
			.error_code = ST_UCODE_ERR_MSR_READ_FAILED,
		};
	}

	uint32_t bsp_rev = msr_per_cpu.value[0].hi;

	for (int i = 0; i < cpu_count; i++) {
		uint32_t core_rev = msr_per_cpu.value[i].hi;
		st_debug("[cpu%d]: 0x%x %x\n", i, core_rev, msr_per_cpu.value[i].lo);

		if (core_rev == 0) {
			return (struct st_status){
				.status = ST_FAILED,
				.error_code = ST_UCODE_ERR_NOT_LOADED,
			};
		}

		if (core_rev != bsp_rev) {
			return (struct st_status){
				.status = ST_FAILED,
				.error_code = ST_UCODE_ERR_CORE_MISMATCH,
			};
		}
	}

	return ret;
}
