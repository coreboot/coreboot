/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/cache.h>
#include <string.h>
#include "s3_resume.h"

static void write_mtrr(u8 **p_nvram_pos, unsigned idx)
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
	write_mtrr(&nvram_pos, 0x250);
	write_mtrr(&nvram_pos, 0x258);
	write_mtrr(&nvram_pos, 0x259);

	for (i = 0x268; i < 0x270; i++)
		write_mtrr(&nvram_pos, i);

	/* Disable access to AMD RdDram and WrDram extension bits */
	msr_data = rdmsr(SYSCFG_MSR);
	msr_data.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, msr_data);

	/* Variable MTRRs */
	for (i = 0x200; i < 0x210; i++)
		write_mtrr(&nvram_pos, i);

	/* SYSCFG_MSR */
	write_mtrr(&nvram_pos, SYSCFG_MSR);
	/* TOM */
	write_mtrr(&nvram_pos, 0xC001001A);
	/* TOM2 */
	write_mtrr(&nvram_pos, 0xC001001D);

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
	wrmsr(0x250, msr_data);

	msr_data.lo = *msrPtr;
	msrPtr ++;
	msr_data.hi = *msrPtr;
	msrPtr ++;
	wrmsr(0x258, msr_data);

	msr_data.lo = *msrPtr;
	msrPtr ++;
	msr_data.hi = *msrPtr;
	msrPtr ++;
	wrmsr(0x259, msr_data);

	for (msr = 0x268; msr <= 0x26F; msr++) {
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
	for (msr = 0x200; msr <= 0x20F; msr++) {
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
