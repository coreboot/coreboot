/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include "haswell.h"

/* MSR Documentation based on
 * "Sandy Bridge Processor Family BIOS Writer's Guide (BWG)"
 * Document Number 504790
 * Revision 1.6.0, June 2012 */

void intel_cpu_haswell_finalize_smm(void)
{
#if 0
	/* Lock C-State MSR */
	msr_set_bit(MSR_PKG_CST_CONFIG_CONTROL, 15);

	/* Lock AES-NI only if supported */
	if (cpuid_ecx(1) & (1 << 25))
		msr_set_bit(MSR_FEATURE_CONFIG, 0);

#ifdef LOCK_POWER_CONTROL_REGISTERS
	/*
	 * Lock the power control registers.
	 *
	 * These registers can be left unlocked if modifying power
	 * limits from the OS is desirable. Modifying power limits
	 * from the OS can be especially useful for experimentation
	 * during  early phases of system bringup while the thermal
	 * power envelope is being proven.
	 */

	msr_set_bit(MSR_PP0_CURRENT_CONFIG, 31);
	msr_set_bit(MSR_PP1_CURRENT_CONFIG, 31);
	msr_set_bit(MSR_PKG_POWER_LIMIT, 63);
	msr_set_bit(MSR_PP0_POWER_LIMIT, 31);
	msr_set_bit(MSR_PP1_POWER_LIMIT, 31);
#endif

	/* Lock TM interrupts - route thermal events to all processors */
	msr_set_bit(MSR_MISC_PWR_MGMT, 22);

	/* Lock memory configuration to protect SMM */
	msr_set_bit(MSR_LT_LOCK_MEMORY, 0);
#endif
}
