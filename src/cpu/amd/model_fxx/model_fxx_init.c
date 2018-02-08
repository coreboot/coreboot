/*
 * This file is part of the coreboot project.
 *
 * 2004.11 yhlu add d0 e0 support
 * 2004.12 yhlu add dual core support
 * 2005.02 yhlu add e0 memory hole support
 * Copyright 2005 AMD
 * 2005.08 yhlu add microcode support
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

/* Needed so the AMD K8 runs correctly.  */

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/pae.h>
#include <pc80/mc146818rtc.h>
#include <cpu/x86/lapic.h>
#include "northbridge/amd/amdk8/amdk8.h"
#include <cpu/amd/model_fxx_rev.h>
#include <cpu/amd/microcode.h>
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <cpu/amd/multicore.h>
#include <cpu/amd/msr.h>

#if IS_ENABLED(CONFIG_WAIT_BEFORE_CPUS_INIT)
void cpus_ready_for_init(void)
{
#if IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
#if CONFIG_MEM_TRAIN_SEQ == 1
	struct sys_info *sysinfox = (struct sys_info *)((CONFIG_RAMTOP) - sizeof(*sysinfox));
	// wait for ap memory to trained
	wait_all_core0_mem_trained(sysinfox);
#endif
#endif
}
#endif

int is_e0_later_in_bsp(int nodeid)
{
	uint32_t val;
	uint32_t val_old;
	int e0_later;

	if (IS_ENABLED(CONFIG_K8_REV_F_SUPPORT))
		return 1;

	if (nodeid == 0) {	// we don't need to do that for node 0 in core0/node0
		return !is_cpu_pre_e0();
	}
	// d0 will be treated as e0 with this methods, but the d0 nb_cfg_54 always 0
	struct device *dev;
	dev = dev_find_slot(0, PCI_DEVFN(0x18 + nodeid, 2));
	if (!dev)
		return 0;
	val_old = pci_read_config32(dev, 0x80);
	val = val_old;
	val |= (1 << 3);
	pci_write_config32(dev, 0x80, val);
	val = pci_read_config32(dev, 0x80);
	e0_later = !!(val & (1 << 3));
	if (e0_later) {		// pre_e0 bit 3 always be 0 and can not be changed
		pci_write_config32(dev, 0x80, val_old);	// restore it
	}

	return e0_later;
}

int is_cpu_f0_in_bsp(int nodeid)
{
	uint32_t dword;
	struct device *dev;

	if (!IS_ENABLED(CONFIG_K8_REV_F_SUPPORT))
		return 0;

	dev = dev_find_slot(0, PCI_DEVFN(0x18 + nodeid, 3));
	dword = pci_read_config32(dev, 0xfc);
	return (dword & 0xfff00) == 0x40f00;
}

#define MCI_STATUS 0x401

#define MTRR_COUNT 8
#define ZERO_CHUNK_KB 0x800UL	/* 2M */
#define TOLM_KB 0x400000UL

struct mtrr {
	msr_t base;
	msr_t mask;
};

struct mtrr_state {
	struct mtrr mtrrs[MTRR_COUNT];
	msr_t top_mem, top_mem2;
	msr_t def_type;
};

static void save_mtrr_state(struct mtrr_state *state)
{
	int i;
	for (i = 0; i < MTRR_COUNT; i++) {
		state->mtrrs[i].base = rdmsr(MTRR_PHYS_BASE(i));
		state->mtrrs[i].mask = rdmsr(MTRR_PHYS_MASK(i));
	}
	state->top_mem = rdmsr(TOP_MEM);
	state->top_mem2 = rdmsr(TOP_MEM2);
	state->def_type = rdmsr(MTRR_DEF_TYPE_MSR);
}

static void restore_mtrr_state(struct mtrr_state *state)
{
	int i;
	disable_cache();

	for (i = 0; i < MTRR_COUNT; i++) {
		wrmsr(MTRR_PHYS_BASE(i), state->mtrrs[i].base);
		wrmsr(MTRR_PHYS_MASK(i), state->mtrrs[i].mask);
	}
	wrmsr(TOP_MEM, state->top_mem);
	wrmsr(TOP_MEM2, state->top_mem2);
	wrmsr(MTRR_DEF_TYPE_MSR, state->def_type);

	enable_cache();
}

#if 0
static void print_mtrr_state(struct mtrr_state *state)
{
	int i;
	for (i = 0; i < MTRR_COUNT; i++) {
		printk(BIOS_DEBUG, "var mtrr %d: %08x%08x mask: %08x%08x\n",
		       i,
		       state->mtrrs[i].base.hi, state->mtrrs[i].base.lo,
		       state->mtrrs[i].mask.hi, state->mtrrs[i].mask.lo);
	}
	printk(BIOS_DEBUG, "top_mem:  %08x%08x\n",
	       state->top_mem.hi, state->top_mem.lo);
	printk(BIOS_DEBUG, "top_mem2: %08x%08x\n",
	       state->top_mem2.hi, state->top_mem2.lo);
	printk(BIOS_DEBUG, "def_type: %08x%08x\n",
	       state->def_type.hi, state->def_type.lo);
}
#endif

static void set_init_ecc_mtrrs(void)
{
	msr_t msr;
	int i;
	disable_cache();

	/* First clear all of the msrs to be safe */
	for (i = 0; i < MTRR_COUNT; i++) {
		msr_t zero;
		zero.lo = zero.hi = 0;
		wrmsr(MTRR_PHYS_BASE(i), zero);
		wrmsr(MTRR_PHYS_MASK(i), zero);
	}

	/* Write back cache from 0x0 to CACHE_TMP_RAMTOP. */
	msr.hi = 0x00000000;
	msr.lo = 0x00000000 | MTRR_TYPE_WRBACK;
	wrmsr(MTRR_PHYS_BASE(0), msr);
	msr.hi = 0x000000ff;
	msr.lo = ~((CACHE_TMP_RAMTOP) - 1) | 0x800;
	wrmsr(MTRR_PHYS_MASK(0), msr);

	/* Set the default type to write combining */
	msr.hi = 0x00000000;
	msr.lo = 0xc00 | MTRR_TYPE_WRCOMB;
	wrmsr(MTRR_DEF_TYPE_MSR, msr);

	/* Set TOP_MEM to 4G */
	msr.hi = 0x00000001;
	msr.lo = 0x00000000;
	wrmsr(TOP_MEM, msr);

	enable_cache();
}

static inline void clear_2M_ram(unsigned long basek,
				struct mtrr_state *mtrr_state)
{
	unsigned long limitk;
	unsigned long size;
	void *addr;

	/* Report every 64M */
	if ((basek % (64 * 1024)) == 0) {

		/* Restore the normal state */
		map_2M_page(0);
		restore_mtrr_state(mtrr_state);
		enable_lapic();

		/* Print a status message */
		printk(BIOS_DEBUG, "%c", (basek >= TOLM_KB) ? '+' : '-');

		/* Return to the initialization state */
		set_init_ecc_mtrrs();
		disable_lapic();

	}

	limitk = (basek + ZERO_CHUNK_KB) & ~(ZERO_CHUNK_KB - 1);
#if 0
	/* couldn't happen, memory must on 2M boundary */
	if (limitk > endk) {
		limitk = enk;
	}
#endif
	size = (limitk - basek) << 10;
	addr = map_2M_page(basek >> 11);
	if (addr == MAPPING_ERROR) {
		printk(BIOS_ERR, "Cannot map page: %lx\n", basek >> 11);
		return;
	}

	/* clear memory 2M (limitk - basek) */
	addr = (void *)(((uint32_t) addr) | ((basek & 0x7ff) << 10));
	memset(addr, 0, size);
}

static void init_ecc_memory(unsigned node_id)
{
	unsigned long startk, begink, endk;
	unsigned long basek;
	struct mtrr_state mtrr_state;

	struct device *f1_dev, *f2_dev, *f3_dev;
	int enable_scrubbing;
	uint32_t dcl;

	f1_dev = dev_find_slot(0, PCI_DEVFN(0x18 + node_id, 1));
	if (!f1_dev) {
		die("Cannot find CPU function 1\n");
	}
	f2_dev = dev_find_slot(0, PCI_DEVFN(0x18 + node_id, 2));
	if (!f2_dev) {
		die("Cannot find CPU function 2\n");
	}
	f3_dev = dev_find_slot(0, PCI_DEVFN(0x18 + node_id, 3));
	if (!f3_dev) {
		die("Cannot find CPU function 3\n");
	}

	/* See if we scrubbing should be enabled */
	enable_scrubbing = CONFIG_HW_SCRUBBER;
	get_option(&enable_scrubbing, "hw_scrubber");

	/* Enable cache scrubbing at the lowest possible rate */
	if (enable_scrubbing) {
		pci_write_config32(f3_dev, SCRUB_CONTROL,
				   (SCRUB_84ms << 16) | (SCRUB_84ms << 8) |
				   (SCRUB_NONE << 0));
	} else {
		pci_write_config32(f3_dev, SCRUB_CONTROL,
				   (SCRUB_NONE << 16) | (SCRUB_NONE << 8) |
				   (SCRUB_NONE << 0));
		printk(BIOS_DEBUG, "Scrubbing Disabled\n");
	}

	/* If ecc support is not enabled don't touch memory */
	dcl = pci_read_config32(f2_dev, DRAM_CONFIG_LOW);
	if (!(dcl & DCL_DimmEccEn)) {
		printk(BIOS_DEBUG, "ECC Disabled\n");
		return;
	}

	startk =
	    (pci_read_config32(f1_dev, 0x40 + (node_id * 8)) & 0xffff0000) >> 2;
	endk =
	    ((pci_read_config32(f1_dev, 0x44 + (node_id * 8)) & 0xffff0000) >>
	     2) + 0x4000;

#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	unsigned long hole_startk = 0;

	if (IS_ENABLED(CONFIG_K8_REV_F_SUPPORT) || !is_cpu_pre_e0()) {
		uint32_t val;
		val = pci_read_config32(f1_dev, 0xf0);
		if (val & 1) {
			hole_startk = ((val & (0xff << 24)) >> 10);
		}
	}
#endif

	/* Don't start too early */
	begink = startk;
	if (begink < (CONFIG_RAMTOP >> 10)) {
		begink = (CONFIG_RAMTOP >> 10);
	}

	printk(BIOS_DEBUG, "Clearing memory %luK - %luK: ", begink, endk);

	/* Save the normal state */
	save_mtrr_state(&mtrr_state);

	/* Switch to the init ecc state */
	set_init_ecc_mtrrs();
	disable_lapic();

	/* Walk through 2M chunks and zero them */
#if CONFIG_HW_MEM_HOLE_SIZEK != 0
	/* here hole_startk can not be equal to begink, never. Also hole_startk is in 2M boundary, 64M? */
	if ((hole_startk != 0)
	    && ((begink < hole_startk) && (endk > (4 * 1024 * 1024)))) {
		for (basek = begink; basek < hole_startk;
		     basek = ((basek + ZERO_CHUNK_KB) & ~(ZERO_CHUNK_KB - 1))) {
			clear_2M_ram(basek, &mtrr_state);
		}
		for (basek = 4 * 1024 * 1024; basek < endk;
		     basek = ((basek + ZERO_CHUNK_KB) & ~(ZERO_CHUNK_KB - 1))) {
			clear_2M_ram(basek, &mtrr_state);
		}
	} else
#endif
		for (basek = begink; basek < endk;
		     basek = ((basek + ZERO_CHUNK_KB) & ~(ZERO_CHUNK_KB - 1))) {
			clear_2M_ram(basek, &mtrr_state);
		}

	/* Restore the normal state */
	map_2M_page(0);
	restore_mtrr_state(&mtrr_state);
	enable_lapic();

	/* Set the scrub base address registers */
	pci_write_config32(f3_dev, SCRUB_ADDR_LOW, startk << 10);
	pci_write_config32(f3_dev, SCRUB_ADDR_HIGH, startk >> 22);

	/* Enable the scrubber? */
	if (enable_scrubbing) {
		/* Enable scrubbing at the lowest possible rate */
		pci_write_config32(f3_dev, SCRUB_CONTROL,
				   (SCRUB_84ms << 16) | (SCRUB_84ms << 8) |
				   (SCRUB_84ms << 0));
	}

	printk(BIOS_DEBUG, " done\n");
}

static void k8_pre_f_errata(void)
{
	msr_t msr;

	if (is_cpu_pre_c0()) {
		/* Erratum 63... */
		msr = rdmsr(HWCR_MSR);
		msr.lo |= (1 << 6);
		wrmsr(HWCR_MSR, msr);

		/* Erratum 69... */
		msr = rdmsr_amd(BU_CFG_MSR);
		msr.hi |= (1 << (45 - 32));
		wrmsr_amd(BU_CFG_MSR, msr);

		/* Erratum 81... */
		msr = rdmsr_amd(DC_CFG_MSR);
		msr.lo |= (1 << 10);
		wrmsr_amd(DC_CFG_MSR, msr);

	}

	/* Erratum 97 ... */
	if (!is_cpu_pre_c0() && is_cpu_pre_d0()) {
		msr = rdmsr_amd(DC_CFG_MSR);
		msr.lo |= 1 << 3;
		wrmsr_amd(DC_CFG_MSR, msr);
	}

	/* Erratum 94 ... */
	if (is_cpu_pre_d0()) {
		msr = rdmsr_amd(IC_CFG_MSR);
		msr.lo |= 1 << 11;
		wrmsr_amd(IC_CFG_MSR, msr);
	}

	/* Erratum 91 prefetch miss is handled in the kernel */

	/* Erratum 106 ... */
	msr = rdmsr_amd(LS_CFG_MSR);
	msr.lo |= 1 << 25;
	wrmsr_amd(LS_CFG_MSR, msr);

	/* Erratum 107 ... */
	msr = rdmsr_amd(BU_CFG_MSR);
	msr.hi |= 1 << (43 - 32);
	wrmsr_amd(BU_CFG_MSR, msr);

	/* Erratum 110 */
	/* This erratum applies to D0 thru E6 revisions
	 * Revision F and later are unaffected. There are two fixes
	 * depending on processor revision.
	 */
	if (is_cpu_d0()) {
		/* Erratum 110 ... */
		msr = rdmsr_amd(CPU_ID_HYPER_EXT_FEATURES);
		msr.hi |= 1;
		wrmsr_amd(CPU_ID_HYPER_EXT_FEATURES, msr);
	}

	if (!is_cpu_pre_e0())
	{
		/* Erratum 110 ... */
		msr = rdmsr_amd(CPU_ID_EXT_FEATURES_MSR);
		msr.hi |= 1;
		wrmsr_amd(CPU_ID_EXT_FEATURES_MSR, msr);
	}
}

static void k8_errata(void)
{
	msr_t msr;

	/* I can't touch this msr on early buggy cpus */
	if (!is_cpu_pre_b3()) {
		msr = rdmsr(NB_CFG_MSR);

		if (is_cpu_pre_d0() && !is_cpu_pre_c0()) {
			/* D0 later don't need it */
			/* Erratum 86 Disable data masking on C0 and
			 * later processor revs.
			 * FIXME this is only needed if ECC is enabled.
			 */
			msr.hi |= 1 << (36 - 32);
		}

		/* Erratum 89 ... */
		/* Erratum 89 is mistakenly labeled as 88 in AMD pub #25759
		 * It is correctly labeled as 89 on page 49 of the document
		 * and in AMD pub#33610
		 */
		msr.lo |= 1 << 3;
		/* Erratum 169 */
		/* This supersedes erratum 131; 131 should not be applied with 169
		 * We also need to set some bits in the northbridge, handled in src/northbridge/amdk8/
		 */
		msr.hi |= 1;

		wrmsr(NB_CFG_MSR, msr);
	}

	/* Erratum 122 */
	msr = rdmsr(HWCR_MSR);
	msr.lo |= 1 << 6;
	wrmsr(HWCR_MSR, msr);
}

static void model_fxx_init(struct device *dev)
{
	unsigned long i;
	msr_t msr;
	struct node_core_id id;

	/* Turn on caching if we haven't already */
	x86_enable_cache();

	/* Initialize all variable MTRRs except the first pair */
	msr.hi = 0x00000000;
	msr.lo = 0x00000000;

	disable_cache();

	for (i = 0x2; i < 0x10; i++) {
		wrmsr(0x00000200 | i, msr);
	}

	enable_cache();
	amd_setup_mtrrs();
	x86_mtrr_check();

	/* Update the microcode */
	update_microcode(dev->device);

	disable_cache();

	/* zero the machine check error status registers */
	msr.lo = 0;
	msr.hi = 0;
	for (i = 0; i < 5; i++) {
		wrmsr(MCI_STATUS + (i * 4), msr);
	}

	if (!IS_ENABLED(CONFIG_K8_REV_F_SUPPORT))
		k8_pre_f_errata();

	k8_errata();

	enable_cache();

	/* Set the processor name string */
	init_processor_name();

	/* Enable the local CPU APICs */
	setup_lapic();

#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
	u32 siblings = cpuid_ecx(0x80000008) & 0xff;

	if (siblings > 0) {
		msr = rdmsr_amd(CPU_ID_FEATURES_MSR);
		msr.lo |= 1 << 28;
		wrmsr_amd(CPU_ID_FEATURES_MSR, msr);

		msr = rdmsr_amd(LOGICAL_CPUS_NUM_MSR);
		msr.lo = (siblings + 1) << 16;
		wrmsr_amd(LOGICAL_CPUS_NUM_MSR, msr);

		msr = rdmsr_amd(CPU_ID_EXT_FEATURES_MSR);
		msr.hi |= 1 << (33 - 32);
		wrmsr_amd(CPU_ID_EXT_FEATURES_MSR, msr);
	}
	printk(BIOS_DEBUG, "siblings = %02d, ", siblings);
#endif

	id = get_node_core_id(read_nb_cfg_54());	// pre e0 nb_cfg_54 can not be set

	/* Is this a bad location?  In particular can another node prefetch
	 * data from this node before we have initialized it?
	 */
	if (id.coreid == 0)
		init_ecc_memory(id.nodeid);	// only do it for core 0

	/* Set SMM base address for this CPU */
	msr = rdmsr(SMM_BASE_MSR);
	msr.lo = SMM_BASE - (lapicid() * 0x400);
	wrmsr(SMM_BASE_MSR, msr);

	/* Enable the SMM memory window */
	msr = rdmsr(SMM_MASK_MSR);
	msr.lo |= (1 << 0); /* Enable ASEG SMRAM Range */
	wrmsr(SMM_MASK_MSR, msr);

	/* Set SMMLOCK to avoid exploits messing with SMM */
	msr = rdmsr(HWCR_MSR);
	msr.lo |= (1 << 0);
	wrmsr(HWCR_MSR, msr);
}

static struct device_operations cpu_dev_ops = {
	.init = model_fxx_init,
};

static const struct cpu_device_id cpu_table[] = {
#if !IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
	{ X86_VENDOR_AMD, 0xf40 },   /* SH-B0 (socket 754) */
	{ X86_VENDOR_AMD, 0xf50 },   /* SH-B0 (socket 940) */
	{ X86_VENDOR_AMD, 0xf51 },   /* SH-B3 (socket 940) */
	{ X86_VENDOR_AMD, 0xf58 },   /* SH-C0 (socket 940) */
	{ X86_VENDOR_AMD, 0xf48 },   /* SH-C0 (socket 754) */
	{ X86_VENDOR_AMD, 0xf5a },   /* SH-CG (socket 940) */
	{ X86_VENDOR_AMD, 0xf4a },   /* SH-CG (socket 754) */
	{ X86_VENDOR_AMD, 0xf7a },   /* SH-CG (socket 939) */
	{ X86_VENDOR_AMD, 0xfc0 },   /* DH-CG (socket 754) */
	{ X86_VENDOR_AMD, 0xfe0 },   /* DH-CG (socket 754) */
	{ X86_VENDOR_AMD, 0xff0 },   /* DH-CG (socket 939) */
	{ X86_VENDOR_AMD, 0xf82 },   /* CH-CG (socket 754) */
	{ X86_VENDOR_AMD, 0xfb2 },   /* CH-CG (socket 939) */

	/* AMD D0 support */
	{ X86_VENDOR_AMD, 0x10f50 }, /* SH-D0 (socket 940) */
	{ X86_VENDOR_AMD, 0x10f40 }, /* SH-D0 (socket 754) */
	{ X86_VENDOR_AMD, 0x10f70 }, /* SH-D0 (socket 939) */
	{ X86_VENDOR_AMD, 0x10fc0 }, /* DH-D0 (socket 754) */
	{ X86_VENDOR_AMD, 0x10ff0 }, /* DH-D0 (socket 939) */
	{ X86_VENDOR_AMD, 0x10f80 }, /* CH-D0 (socket 754) */
	{ X86_VENDOR_AMD, 0x10fb0 }, /* CH-D0 (socket 939) */

	/* AMD E0 support */
	{ X86_VENDOR_AMD, 0x20f50 }, /* SH-E0 */
	{ X86_VENDOR_AMD, 0x20f40 },
	{ X86_VENDOR_AMD, 0x20f70 },
	{ X86_VENDOR_AMD, 0x20fc0 }, /* DH-E3 (socket 754) */
	{ X86_VENDOR_AMD, 0x20ff0 }, /* DH-E3 (socket 939) */
	{ X86_VENDOR_AMD, 0x20f10 }, /* JH-E1 (socket 940) */
	{ X86_VENDOR_AMD, 0x20f51 }, /* SH-E4 (socket 940) */
	{ X86_VENDOR_AMD, 0x20f71 }, /* SH-E4 (socket 939) */
	{ X86_VENDOR_AMD, 0x20fb1 }, /* BH-E4 (socket 939) */
	{ X86_VENDOR_AMD, 0x20f42 }, /* SH-E5 (socket 754) */
	{ X86_VENDOR_AMD, 0x20ff2 }, /* DH-E6 (socket 939) */
	{ X86_VENDOR_AMD, 0x20fc2 }, /* DH-E6 (socket 754) */
	{ X86_VENDOR_AMD, 0x20f12 }, /* JH-E6 (socket 940) */
	{ X86_VENDOR_AMD, 0x20f32 }, /* JH-E6 (socket 939) */
	{ X86_VENDOR_AMD, 0x30ff2 }, /* E4 ? */
#endif

#if IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
	/*
	 * AMD F0 support.
	 *
	 * See Revision Guide for AMD NPT Family 0Fh Processors,
	 * Publication #33610, Revision: 3.30, February 2008.
	 *
	 * http://www.amd.com/us-en/assets/content_type/white_papers_and_tech_docs/33610.pdf
	 */
	{ X86_VENDOR_AMD, 0x40f50 }, /* SH-F0 (socket F/1207) */
	{ X86_VENDOR_AMD, 0x40f70 }, /* SH-F0 (socket AM2) */
	{ X86_VENDOR_AMD, 0x40f40 }, /* SH-F0 (socket S1g1) */
	{ X86_VENDOR_AMD, 0x40f11 }, /* JH-F1 (socket F/1207) */
	{ X86_VENDOR_AMD, 0x40f31 }, /* JH-F1 (socket AM2) */
	{ X86_VENDOR_AMD, 0x40f01 }, /* JH-F1 (socket S1g1) */

	{ X86_VENDOR_AMD, 0x40f12 }, /* JH-F2 (socket F/1207) */
	{ X86_VENDOR_AMD, 0x40f32 }, /* JH-F2 (socket AM2) */
	{ X86_VENDOR_AMD, 0x40fb2 }, /* BH-F2 (socket AM2) */
	{ X86_VENDOR_AMD, 0x40f82 }, /* BH-F2 (socket S1g1) */
	{ X86_VENDOR_AMD, 0x40ff2 }, /* DH-F2 (socket AM2) */
	{ X86_VENDOR_AMD, 0x50ff2 }, /* DH-F2 (socket AM2) */
	{ X86_VENDOR_AMD, 0x40fc2 }, /* DH-F2 (socket S1g1) */
	{ X86_VENDOR_AMD, 0x40f13 }, /* JH-F3 (socket F/1207) */
	{ X86_VENDOR_AMD, 0x40f33 }, /* JH-F3 (socket AM2) */
	{ X86_VENDOR_AMD, 0x50fd3 }, /* JH-F3 (socket F/1207) */
	{ X86_VENDOR_AMD, 0xc0f13 }, /* JH-F3 (socket F/1207) */
	{ X86_VENDOR_AMD, 0x50ff3 }, /* DH-F3 (socket AM2) */
	{ X86_VENDOR_AMD, 0x60fb1 }, /* BH-G1 (socket AM2) */
	{ X86_VENDOR_AMD, 0x60f81 }, /* BH-G1 (socket S1g1) */
	{ X86_VENDOR_AMD, 0x60fb2 }, /* BH-G2 (socket AM2) */
	{ X86_VENDOR_AMD, 0x60f82 }, /* BH-G2 (socket S1g1) */
	{ X86_VENDOR_AMD, 0x70ff1 }, /* DH-G1 (socket AM2) */
	{ X86_VENDOR_AMD, 0x60ff2 }, /* DH-G2 (socket AM2) */
	{ X86_VENDOR_AMD, 0x70ff2 }, /* DH-G2 (socket AM2) */
	{ X86_VENDOR_AMD, 0x60fc2 }, /* DH-G2 (socket S1g1) */
	{ X86_VENDOR_AMD, 0x70fc2 }, /* DH-G2 (socket S1g1) */
#endif

	{ 0, 0 },
};

static const struct cpu_driver model_fxx __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
