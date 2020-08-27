/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include "haswell.h"

void intel_cpu_haswell_finalize_smm(void)
{
	/* Lock memory configuration to protect SMM */
	msr_set_bit(MSR_LT_LOCK_MEMORY, 0);
}
