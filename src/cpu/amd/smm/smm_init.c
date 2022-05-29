/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <cpu/x86/smi_deprecated.h>
#include <string.h>

void smm_init(void)
{
	msr_t msr, syscfg_orig, mtrr_aseg_orig;

	/* Back up MSRs for later restore */
	syscfg_orig = rdmsr(SYSCFG_MSR);
	mtrr_aseg_orig = rdmsr(MTRR_FIX_16K_A0000);

	/* MTRR changes don't like an enabled cache */
	disable_cache();

	msr = syscfg_orig;

	/* Allow changes to MTRR extended attributes */
	msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	/* turn the extended attributes off until we fix
	 * them so A0000 is routed to memory
	 */
	msr.lo &= ~SYSCFG_MSR_MtrrFixDramEn;
	wrmsr(SYSCFG_MSR, msr);

	/* set DRAM access to 0xa0000 */
	msr.lo = 0x18181818;
	msr.hi = 0x18181818;
	wrmsr(MTRR_FIX_16K_A0000, msr);

	/* enable the extended features */
	msr = syscfg_orig;
	msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	msr.lo |= SYSCFG_MSR_MtrrFixDramEn;
	wrmsr(SYSCFG_MSR, msr);

	enable_cache();
	/* copy the real SMM handler */
	memcpy((void *)SMM_BASE, _binary_smm_start, _binary_smm_end - _binary_smm_start);
	wbinvd();
	disable_cache();

	/* Restore SYSCFG and MTRR */
	wrmsr(SYSCFG_MSR, syscfg_orig);
	wrmsr(MTRR_FIX_16K_A0000, mtrr_aseg_orig);
	enable_cache();

	/* CPU MSR are set in CPU init */
}

void smm_init_completion(void)
{
}
