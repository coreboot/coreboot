/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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

#include <stdint.h>
#include <arch/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>

static const uint32_t microcode_updates[] = {
	#include "x06_microcode.h"
};

struct microcode {
	u32 hdrver;	/* Header Version */
	u32 rev;	/* Patch ID       */
	u32 date;	/* DATE           */
	u32 sig;	/* CPUID          */

	u32 cksum;	/* Checksum       */
	u32 ldrver;	/* Loader Version */
	u32 pf;		/* Platform ID    */

	u32 data_size;	/* Data size      */
	u32 total_size;	/* Total size     */

	u32 reserved[3];
	u32 bits[1012];
};

static inline u32 read_microcode_rev(void)
{
	/* Some Intel Cpus can be very finicky about the
	 * CPUID sequence used.  So this is implemented in
	 * assembly so that it works reliably.
	 */
	msr_t msr;
	__asm__ volatile (
		"wrmsr\n\t"
		"xorl %%eax, %%eax\n\t"
		"xorl %%edx, %%edx\n\t"
		"movl $0x8b, %%ecx\n\t"
		"wrmsr\n\t"
		"movl $0x01, %%eax\n\t"
		"cpuid\n\t"
		"movl $0x08b, %%ecx\n\t"
		"rdmsr \n\t"
		: /* outputs */
		"=a" (msr.lo), "=d" (msr.hi)
		: /* inputs */
		: /* trashed */
		 "ecx"
	);
	return msr.hi;
}

void intel_update_microcode(const void *microcode_updates)
{
	unsigned int eax;
	unsigned int pf, rev, sig;
	unsigned int x86_model, x86_family;
	const struct microcode *m;
	const char *c;
	msr_t msr;

	/* cpuid sets msr 0x8B iff a microcode update has been loaded. */
	msr.lo = 0;
	msr.hi = 0;
	wrmsr(0x8B, msr);
	eax = cpuid_eax(1);
	msr = rdmsr(0x8B);
	rev = msr.hi;
	x86_model = (eax >>4) & 0x0f;
	x86_family = (eax >>8) & 0x0f;
	sig = eax;

	pf = 0;
	if ((x86_model >= 5)||(x86_family>6)) {
		msr = rdmsr(0x17);
		pf = 1 << ((msr.hi >> 18) & 7);
	}

	m = microcode_updates;
	for(c = microcode_updates; m->hdrver;  m = (const struct microcode *)c) {
		if ((m->sig == sig) && (m->pf & pf)) {
			unsigned int new_rev;
			msr.lo = (unsigned long)(&m->bits) & 0xffffffff;
			msr.hi = 0;
			wrmsr(0x79, msr);

			/* Read back the new microcode version */
			new_rev = read_microcode_rev();
			break;
		}
		if (m->total_size) {
			c += m->total_size;
		} else {
			c += 2048;
		}
	}
}

static void set_var_mtrr(
	unsigned reg, unsigned base, unsigned size, unsigned type)

{
	/* Bit Bit 32-35 of MTRRphysMask should be set to 1 */
	/* FIXME: It only support 4G less range */
	msr_t basem, maskm;
	basem.lo = base | type;
	basem.hi = 0;
	wrmsr(MTRRphysBase_MSR(reg), basem);
	maskm.lo = ~(size - 1) | MTRRphysMaskValid;
	maskm.hi = (1 << (CONFIG_CPU_ADDR_BITS - 32)) - 1;
	wrmsr(MTRRphysMask_MSR(reg), maskm);
}

static void enable_rom_caching(void)
{
	msr_t msr;

	disable_cache();
	set_var_mtrr(1, 0xffc00000, 4*1024*1024, MTRR_TYPE_WRPROT);
	enable_cache();

	/* Enable Variable MTRRs */
	msr.hi = 0x00000000;
	msr.lo = 0x00000800;
	wrmsr(MTRRdefType_MSR, msr);
}

static void bootblock_cpu_init(void)
{
	enable_rom_caching();
	intel_update_microcode(microcode_updates);
}
