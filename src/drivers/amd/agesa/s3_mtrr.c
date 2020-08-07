/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/cache.h>
#include <string.h>
#include <northbridge/amd/agesa/agesa_helper.h>

/* TODO: Do we want MTRR_DEF_TYPE_MSR too? */
static const uint32_t msr_backup[] = {
	MTRR_FIX_64K_00000,
	MTRR_FIX_16K_80000,
	MTRR_FIX_16K_A0000,
	MTRR_FIX_4K_C0000,
	MTRR_FIX_4K_C8000,
	MTRR_FIX_4K_D0000,
	MTRR_FIX_4K_D8000,
	MTRR_FIX_4K_E0000,
	MTRR_FIX_4K_E8000,
	MTRR_FIX_4K_F0000,
	MTRR_FIX_4K_F8000,
	MTRR_PHYS_BASE(0),
	MTRR_PHYS_MASK(0),
	MTRR_PHYS_BASE(1),
	MTRR_PHYS_MASK(1),
	MTRR_PHYS_BASE(2),
	MTRR_PHYS_MASK(2),
	MTRR_PHYS_BASE(3),
	MTRR_PHYS_MASK(3),
	MTRR_PHYS_BASE(4),
	MTRR_PHYS_MASK(4),
	MTRR_PHYS_BASE(5),
	MTRR_PHYS_MASK(5),
	MTRR_PHYS_BASE(6),
	MTRR_PHYS_MASK(6),
	MTRR_PHYS_BASE(7),
	MTRR_PHYS_MASK(7),
	SYSCFG_MSR,
	TOP_MEM,
	TOP_MEM2,
};

void backup_mtrr(void *mtrr_store, u32 *mtrr_store_size)
{
	msr_t syscfg_msr;
	msr_t *mtrr_save = (msr_t *)mtrr_store;

	/* Enable access to AMD RdDram and WrDram extension bits */
	syscfg_msr = rdmsr(SYSCFG_MSR);
	syscfg_msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, syscfg_msr);

	for (int i = 0; i < ARRAY_SIZE(msr_backup); i++)
		*mtrr_save++ = rdmsr(msr_backup[i]);

	/* Disable access to AMD RdDram and WrDram extension bits */
	syscfg_msr = rdmsr(SYSCFG_MSR);
	syscfg_msr.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, syscfg_msr);

	*mtrr_store_size = sizeof(msr_t) * ARRAY_SIZE(msr_backup);
}

void restore_mtrr(void)
{
	msr_t syscfg_msr;
	msr_t *mtrr_save = (msr_t *)OemS3Saved_MTRR_Storage();

	/* Enable access to AMD RdDram and WrDram extension bits */
	syscfg_msr = rdmsr(SYSCFG_MSR);
	syscfg_msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, syscfg_msr);

	for (int i = 0; i < ARRAY_SIZE(msr_backup); i++)
		wrmsr(msr_backup[i], *mtrr_save++);

	/* Disable access to AMD RdDram and WrDram extension bits */
	syscfg_msr = rdmsr(SYSCFG_MSR);
	syscfg_msr.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, syscfg_msr);
}
