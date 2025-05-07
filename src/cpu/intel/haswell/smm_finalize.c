/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/msr.h>

#include "haswell.h"

void enable_smm_code_access_check(void)
{
	msr_t smm_mca_cap = rdmsr(SMM_MCA_CAP_MSR);
	if (!(smm_mca_cap.hi & SMM_CODE_ACCESS_CHK_MASK)) {
		printk(BIOS_WARNING, "SMM code access check is not supported\n");
		return;
	}

	/* Despite the prolific EDK2 implementation of this feature
	   performing it on all cores, this is still a package-scoped MSR,
	   per the Haswell BWG, and tested on a Raptor Lake platform. */
	msr_t smm_feature_control = rdmsr(SMM_FEATURE_CONTROL_MSR);
	if (smm_feature_control.lo & SMM_FEATURE_CONTROL_LOCK) {
		if (!(smm_feature_control.lo & SMM_CODE_CHK_EN))
			printk(BIOS_WARNING,
			       "SMM feature control was locked without SMM code access check\n");
		return;
	}

	smm_feature_control.lo |= SMM_CODE_CHK_EN | SMM_FEATURE_CONTROL_LOCK;
	wrmsr(SMM_FEATURE_CONTROL_MSR, smm_feature_control);
	printk(BIOS_DEBUG, "Enabled SMM code access check\n");
}
