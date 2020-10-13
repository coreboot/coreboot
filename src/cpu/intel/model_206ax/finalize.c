/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include "model_206ax.h"

/* MSR Documentation based on
 * "Sandy Bridge Processor Family BIOS Writer's Guide (BWG)"
 * Document Number 504790
 * Revision 1.6.0, June 2012 */

void intel_model_206ax_finalize_smm(void)
{
	/* Lock AES-NI only if supported */
	if (cpuid_ecx(1) & (1 << 25))
		msr_set(MSR_FEATURE_CONFIG, BIT(0));

	/* Lock TM interrupts - route thermal events to all processors */
	msr_set(MSR_MISC_PWR_MGMT, BIT(22));

	/* Lock memory configuration to protect SMM */
	msr_set(MSR_LT_LOCK_MEMORY, BIT(0));
}
