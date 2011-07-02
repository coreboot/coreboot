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

extern unsigned char _binary_smm_start;
extern unsigned char _binary_smm_size;

void smm_init(void)
{
	msr_t msr, syscfg_orig, mtrr_aseg_orig;

	/* Back up MSRs for later restore */
	syscfg_orig = rdmsr(SYSCFG_MSR);
	mtrr_aseg_orig = rdmsr(MTRRfix16K_A0000_MSR);

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
	wrmsr(MTRRfix16K_A0000_MSR, msr);

	/* enable the extended features */
	msr = syscfg_orig;
	msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	msr.lo |= SYSCFG_MSR_MtrrFixDramEn;
	wrmsr(SYSCFG_MSR, msr);

	enable_cache();
	/* copy the real SMM handler */
	memcpy((void *)SMM_BASE, &_binary_smm_start, (size_t)&_binary_smm_size);
	wbinvd();
	disable_cache();

	/* Restore SYSCFG and MTRR */
	wrmsr(SYSCFG_MSR, syscfg_orig);
	wrmsr(MTRRfix16K_A0000_MSR, mtrr_aseg_orig);
	enable_cache();

	/* CPU MSR are set in CPU init */
}

void smm_lock(void)
{
	/* We lock SMM in CPU init */
}
