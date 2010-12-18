/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 coresystems GmbH
 * Copyright (C) 2010 Rudolf Marek
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <arch/io.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/model_fxx_msr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>
#include <string.h>

#define SMM_BASE_MSR 0xc0010111
#define SMM_ADDR_MSR 0xc0010112
#define SMM_MASK_MSR 0xc0010113
#define SMM_BASE 0xa0000

extern unsigned char _binary_smm_start;
extern unsigned char _binary_smm_size;

static int smm_handler_copied = 0;

void smm_init(void)
{
	msr_t msr;

	msr = rdmsr(HWCR_MSR);
	if (msr.lo & (1 << 0)) {
		// This sounds like a bug... ? 
		printk(BIOS_DEBUG, "SMM is still locked from last boot, using old handler.\n");
		return;
	}

	/* Only copy SMM handler once, not once per CPU */
	if (!smm_handler_copied) {
		msr_t syscfg_orig, mtrr_aseg_orig;

		smm_handler_copied = 1;

		/* MTRR changes don't like an enabled cache */
		disable_cache();

		/* Back up MSRs for later restore */
		syscfg_orig = rdmsr(SYSCFG_MSR);
		mtrr_aseg_orig = rdmsr(MTRRfix16K_A0000_MSR);

		msr = syscfg_orig;
		/* Allow changes to MTRR extended attributes */
		msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
		/* turn the extended attributes off until we fix
		 * them so A0000 is routed to memory
		 */
		msr.lo &= ~SYSCFG_MSR_MtrrFixDramEn;
		wrmsr(SYSCFG_MSR, msr);

		/* set DRAM access to 0xa0000 */
		/* A0000 is memory */
		msr.lo = 0x18181818;
		msr.hi = 0x18181818;
		wrmsr(MTRRfix16K_A0000_MSR, msr);
		enable_cache();

		/* disable the extended features */
		msr = syscfg_orig;
		msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
		msr.lo |= SYSCFG_MSR_MtrrFixDramEn;
		wrmsr(SYSCFG_MSR, msr);

		/* enable the SMM memory window */
		// TODO does "Enable ASEG SMRAM Range"  have to happen on 
		// every CPU core?
		msr = rdmsr(SMM_MASK_MSR);
		msr.lo |= (1 << 0); // Enable ASEG SMRAM Range
		msr.lo &= ~(1 << 2); // Open ASEG SMRAM Range
		wrmsr(SMM_MASK_MSR, msr);

		/* copy the real SMM handler */
		memcpy((void *)SMM_BASE, &_binary_smm_start, (size_t)&_binary_smm_size);
		wbinvd();

		msr = rdmsr(SMM_MASK_MSR);
		msr.lo |= ~(1 << 2); // Close ASEG SMRAM Range
		wrmsr(SMM_MASK_MSR, msr);

		/* Change SYSCFG so we can restore the MTRR */
		msr = syscfg_orig;
		msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
		msr.lo &= ~SYSCFG_MSR_MtrrFixDramEn;
		wrmsr(SYSCFG_MSR, msr);

		/* Restore MTRR */
		disable_cache();
		wrmsr(MTRRfix16K_A0000_MSR, mtrr_aseg_orig);

		/* Restore SYSCFG */
		wrmsr(SYSCFG_MSR, syscfg_orig);
		enable_cache();
	}

	/* But set SMM base address on all CPUs/cores */
	msr = rdmsr(SMM_BASE_MSR);
	msr.lo = SMM_BASE - (lapicid() * 0x400);
	wrmsr(SMM_BASE_MSR, msr);
}

void smm_lock(void)
{
	// TODO I think this should be running on each CPU
	msr_t msr;

	printk(BIOS_DEBUG, "Locking SMM.\n");

	/* Set SMMLOCK to avoid exploits messing with SMM */
	msr = rdmsr(HWCR_MSR);
	msr.lo |= (1 << 0);
	wrmsr(HWCR_MSR, msr);
}
