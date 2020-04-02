/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stdint.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/cache.h>
#include <string.h>
#include <northbridge/amd/agesa/agesa_helper.h>

static void write_mtrr(u8 **p_nvram_pos, unsigned int idx)
{
	msr_t  msr_data;
	msr_data = rdmsr(idx);

	memcpy(*p_nvram_pos, &msr_data, sizeof(msr_data));
	*p_nvram_pos += sizeof(msr_data);
}

void backup_mtrr(void *mtrr_store, u32 *mtrr_store_size)
{
	u8 *nvram_pos = mtrr_store;
	msr_t  msr_data;
	u32 i;

	/* Enable access to AMD RdDram and WrDram extension bits */
	msr_data = rdmsr(SYSCFG_MSR);
	msr_data.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, msr_data);

	/* Fixed MTRRs */
	write_mtrr(&nvram_pos, MTRR_FIX_64K_00000);
	write_mtrr(&nvram_pos, MTRR_FIX_16K_80000);
	write_mtrr(&nvram_pos, MTRR_FIX_16K_A0000);

	for (i = MTRR_FIX_4K_C0000; i <= MTRR_FIX_4K_F8000; i++)
		write_mtrr(&nvram_pos, i);

	/* Disable access to AMD RdDram and WrDram extension bits */
	msr_data = rdmsr(SYSCFG_MSR);
	msr_data.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, msr_data);

	/* Variable MTRRs */
	for (i = MTRR_PHYS_BASE(0); i < MTRR_PHYS_BASE(8); i++)
		write_mtrr(&nvram_pos, i);

	/* SYSCFG_MSR */
	write_mtrr(&nvram_pos, SYSCFG_MSR);
	/* TOM */
	write_mtrr(&nvram_pos, TOP_MEM);
	/* TOM2 */
	write_mtrr(&nvram_pos, TOP_MEM2);

	*mtrr_store_size = nvram_pos - (u8*) mtrr_store;
}

void restore_mtrr(void)
{
	volatile u32 *msrPtr = (u32 *) OemS3Saved_MTRR_Storage();
	u32 msr;
	msr_t msr_data;

	if (!msrPtr)
		return;

	disable_cache();

	/* Enable access to AMD RdDram and WrDram extension bits */
	msr_data = rdmsr(SYSCFG_MSR);
	msr_data.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, msr_data);

	/* Now restore the Fixed MTRRs */
	msr_data.lo = *msrPtr;
	msrPtr ++;
	msr_data.hi = *msrPtr;
	msrPtr ++;
	wrmsr(MTRR_FIX_64K_00000, msr_data);

	msr_data.lo = *msrPtr;
	msrPtr ++;
	msr_data.hi = *msrPtr;
	msrPtr ++;
	wrmsr(MTRR_FIX_16K_80000, msr_data);

	msr_data.lo = *msrPtr;
	msrPtr ++;
	msr_data.hi = *msrPtr;
	msrPtr ++;
	wrmsr(MTRR_FIX_16K_A0000, msr_data);

	for (msr = MTRR_FIX_4K_C0000; msr <= MTRR_FIX_4K_F8000; msr++) {
		msr_data.lo = *msrPtr;
		msrPtr ++;
		msr_data.hi = *msrPtr;
		msrPtr ++;
		wrmsr(msr, msr_data);
	}

	/* Disable access to AMD RdDram and WrDram extension bits */
	msr_data = rdmsr(SYSCFG_MSR);
	msr_data.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, msr_data);

	/* Restore the Variable MTRRs */
	for (msr = MTRR_PHYS_BASE(0); msr <= MTRR_PHYS_MASK(7); msr++) {
		msr_data.lo = *msrPtr;
		msrPtr ++;
		msr_data.hi = *msrPtr;
		msrPtr ++;
		wrmsr(msr, msr_data);
	}

	/* Restore SYSCFG MTRR */
	msr_data.lo = *msrPtr;
	msrPtr ++;
	msr_data.hi = *msrPtr;
	msrPtr ++;
	wrmsr(SYSCFG_MSR, msr_data);
}
