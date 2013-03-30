/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2000 Ronald G. Minnich
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/* Microcode update for Intel PIII and later CPUs */

#include <stdint.h>
#include <stddef.h>
#if !defined(__ROMCC__)
#include <console/console.h>
#endif
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/microcode.h>

#ifdef __PRE_RAM__
#if CONFIG_CPU_MICROCODE_IN_CBFS
#include <arch/cbfs.h>
#endif
#else
#if CONFIG_CPU_MICROCODE_IN_CBFS
#include <cbfs.h>
#endif
#include <smp/spinlock.h>
DECLARE_SPIN_LOCK(microcode_lock)
#endif

struct microcode {
	u32 hdrver;	/* Header Version */
	u32 rev;	/* Update Revision */
	u32 date;	/* Date */
	u32 sig;	/* Processor Signature */

	u32 cksum;	/* Checksum */
	u32 ldrver;	/* Loader Revision */
	u32 pf;		/* Processor Flags */

	u32 data_size;	/* Data Size */
	u32 total_size;	/* Total Size */

	u32 reserved[3];
};

static inline u32 read_microcode_rev(void)
{
	/* Some Intel CPUs can be very finicky about the
	 * CPUID sequence used.  So this is implemented in
	 * assembly so that it works reliably.
	 */
	msr_t msr;
	asm volatile (
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
		 "ebx", "ecx"
	);
	return msr.hi;
}

#if CONFIG_CPU_MICROCODE_IN_CBFS

#define MICROCODE_CBFS_FILE "cpu_microcode_blob.bin"

void intel_microcode_load_unlocked(const void *microcode_patch)
{
	u32 current_rev;
	msr_t msr;
	const struct microcode *m = microcode_patch;

	if (!m)
		return;

	current_rev = read_microcode_rev();

	/* No use loading the same revision. */
	if (current_rev == m->rev)
		return;

	msr.lo = (unsigned long)m + sizeof(struct microcode);
	msr.hi = 0;
	wrmsr(0x79, msr);

#if !defined(__ROMCC__)
	printk(BIOS_DEBUG, "microcode: updated to revision "
		    "0x%x date=%04x-%02x-%02x\n", read_microcode_rev(),
		    m->date & 0xffff, (m->date >> 24) & 0xff,
		    (m->date >> 16) & 0xff);
#endif
}

const void *intel_microcode_find(void)
{
	void *microcode_updates;
	u32 eax;
	u32 pf, rev, sig;
	unsigned int x86_model, x86_family;
	const struct microcode *m;
	const char *c;
	msr_t msr;

#ifdef __PRE_RAM__
	microcode_updates = walkcbfs((char *) MICROCODE_CBFS_FILE);
#else
	microcode_updates = cbfs_get_file_content(CBFS_DEFAULT_MEDIA,
					       MICROCODE_CBFS_FILE,
					       CBFS_TYPE_MICROCODE);
#endif

	if (!microcode_updates)
		return NULL;

	/* CPUID sets MSR 0x8B iff a microcode update has been loaded. */
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
#if !defined(__ROMCC__)
	/* If this code is compiled with ROMCC we're probably in
	 * the bootblock and don't have console output yet.
	 */
	printk(BIOS_DEBUG, "microcode: sig=0x%x pf=0x%x revision=0x%x\n",
			sig, pf, rev);
#endif

	m = microcode_updates;
	for(c = microcode_updates; m->hdrver; m = (const struct microcode *)c) {
		if ((m->sig == sig) && (m->pf & pf))
			return m;

		if (m->total_size) {
			c += m->total_size;
		} else {
#if !defined(__ROMCC__)
			printk(BIOS_WARNING, "Microcode has no valid size field!\n");
#endif
			c += 2048;
		}
	}

	/* ROMCC doesn't like NULL. */
	return (void *)0;
}

void intel_update_microcode_from_cbfs(void)
{
	const void *patch = intel_microcode_find();

#if !defined(__ROMCC__) && !defined(__PRE_RAM__)
	spin_lock(&microcode_lock);
#endif

	intel_microcode_load_unlocked(patch);

#if !defined(__ROMCC__) && !defined(__PRE_RAM__)
	spin_unlock(&microcode_lock);
#endif
}

#else /* !CONFIG_CPU_MICROCODE_IN_CBFS */

void intel_update_microcode(const void *microcode_updates)
{
	u32 eax;
	u32 pf, rev, sig;
	unsigned int x86_model, x86_family;
	const struct microcode *m;
	const char *c;
	msr_t msr;

	if (!microcode_updates) {
#if !defined(__ROMCC__)
		printk(BIOS_WARNING, "No microcode updates found.\n");
#endif
		return;
	}

	/* CPUID sets MSR 0x8B iff a microcode update has been loaded. */
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
#if !defined(__ROMCC__)
	/* If this code is compiled with ROMCC we're probably in
	 * the bootblock and don't have console output yet.
	 */
	printk(BIOS_DEBUG, "microcode: sig=0x%x pf=0x%x revision=0x%x\n",
			sig, pf, rev);
#endif
#if !defined(__ROMCC__) && !defined(__PRE_RAM__)
	spin_lock(&microcode_lock);
#endif

	m = microcode_updates;
	for(c = microcode_updates; m->hdrver; m = (const struct microcode *)c) {
		if ((m->sig == sig) && (m->pf & pf)) {
			unsigned int new_rev;
			msr.lo = (unsigned long)c + sizeof(struct microcode);
			msr.hi = 0;
			wrmsr(0x79, msr);

			/* Read back the new microcode version */
			new_rev = read_microcode_rev();

#if !defined(__ROMCC__)
			printk(BIOS_DEBUG, "microcode: updated to revision "
				    "0x%x date=%04x-%02x-%02x\n", new_rev,
				    m->date & 0xffff, (m->date >> 24) & 0xff,
				    (m->date >> 16) & 0xff);
#endif
			break;
		}

		if (m->total_size) {
			c += m->total_size;
		} else {
#if !defined(__ROMCC__)
			printk(BIOS_WARNING, "Microcode has no valid size field!\n");
#endif
			c += 2048;
		}
	}

#if !defined(__ROMCC__) && !defined(__PRE_RAM__)
	spin_unlock(&microcode_lock);
#endif
}

#endif
