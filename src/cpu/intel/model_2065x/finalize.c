/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/speedstep.h>

#include "model_2065x.h"

/* MSR Documentation based on
 * "Sandy Bridge Processor Family BIOS Writer's Guide (BWG)"
 * Document Number 504790
 * Revision 1.6.0, June 2012 */

void intel_model_2065x_finalize_smm(void)
{
	/* Lock C-State MSR */
	msr_set(MSR_PKG_CST_CONFIG_CONTROL, BIT(15));

	/* Lock TM interrupts - route thermal events to all processors */
	msr_set(MSR_MISC_PWR_MGMT, BIT(22));
}
