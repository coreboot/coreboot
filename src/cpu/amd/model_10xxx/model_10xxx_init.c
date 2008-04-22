/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/pae.h>
#include <pc80/mc146818rtc.h>
#include <cpu/x86/lapic.h>

#include "../../../northbridge/amd/amdfam10/amdfam10.h"

#include <cpu/amd/model_10xxx_rev.h>
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/mem.h>

#include <cpu/amd/quadcore.h>

#include <cpu/amd/model_10xxx_msr.h>

extern void prep_pstates_all(void);
extern void init_pstates(device_t dev, u32 nodeid, u32 coreid);
extern device_t get_node_pci(u32 nodeid, u32 fn);



void cpus_ready_for_init(void)
{
	prep_pstates_all();
#if MEM_TRAIN_SEQ == 1
	struct sys_info *sysinfox = (struct sys_info *)((CONFIG_LB_MEM_TOPK<<10) - DCACHE_RAM_GLOBAL_VAR_SIZE);
	// wait for ap memory to trained
	wait_all_core0_mem_trained(sysinfox);
#endif
}


#define MCI_STATUS 0x401


static inline msr_t rdmsr_amd(u32 index)
{
	 msr_t result;
	 __asm__ __volatile__ (
		 "rdmsr"
		 : "=a" (result.lo), "=d" (result.hi)
		 : "c" (index), "D" (0x9c5a203a)
		 );
	 return result;
}


static inline void wrmsr_amd(u32 index, msr_t msr)
{
	__asm__ __volatile__ (
		"wrmsr"
		: /* No outputs */
		: "c" (index), "a" (msr.lo), "d" (msr.hi), "D" (0x9c5a203a)
		);
}


#define MTRR_COUNT 8
#define ZERO_CHUNK_KB 0x800UL /* 2M */
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
	for(i = 0; i < MTRR_COUNT; i++) {
		state->mtrrs[i].base = rdmsr(MTRRphysBase_MSR(i));
		state->mtrrs[i].mask = rdmsr(MTRRphysMask_MSR(i));
	}
	state->top_mem	= rdmsr(TOP_MEM);
	state->top_mem2 = rdmsr(TOP_MEM2);
	state->def_type = rdmsr(MTRRdefType_MSR);
}


static void restore_mtrr_state(struct mtrr_state *state)
{
	int i;
	disable_cache();

	for(i = 0; i < MTRR_COUNT; i++) {
		wrmsr(MTRRphysBase_MSR(i), state->mtrrs[i].base);
		wrmsr(MTRRphysMask_MSR(i), state->mtrrs[i].mask);
	}
	wrmsr(TOP_MEM, state->top_mem);
	wrmsr(TOP_MEM2, state->top_mem2);
	wrmsr(MTRRdefType_MSR, state->def_type);

	enable_cache();
}


#if 0
static void print_mtrr_state(struct mtrr_state *state)
{
	int i;
	for(i = 0; i < MTRR_COUNT; i++) {
		printk_debug("var mtrr %d: %08x%08x mask: %08x%08x\n",
			i,
			state->mtrrs[i].base.hi, state->mtrrs[i].base.lo,
			state->mtrrs[i].mask.hi, state->mtrrs[i].mask.lo);
	}
	printk_debug("top_mem:	%08x%08x\n",
		state->top_mem.hi, state->top_mem.lo);
	printk_debug("top_mem2: %08x%08x\n",
		state->top_mem2.hi, state->top_mem2.lo);
	printk_debug("def_type: %08x%08x\n",
		state->def_type.hi, state->def_type.lo);
}
#endif


static void set_init_ecc_mtrrs(void)
{
	msr_t msr;
	int i;
	disable_cache();

	/* First clear all of the msrs to be safe */
	for(i = 0; i < MTRR_COUNT; i++) {
		msr_t zero;
		zero.lo = zero.hi = 0;
		wrmsr(MTRRphysBase_MSR(i), zero);
		wrmsr(MTRRphysMask_MSR(i), zero);
	}

	/* Write back cache the first 1MB */
	msr.hi = 0x00000000;
	msr.lo = 0x00000000 | MTRR_TYPE_WRBACK;
	wrmsr(MTRRphysBase_MSR(0), msr);
	msr.hi = 0x000000ff;
	msr.lo = ~((CONFIG_LB_MEM_TOPK << 10) - 1) | 0x800;
	wrmsr(MTRRphysMask_MSR(0), msr);

	/* Set the default type to write combining */
	msr.hi = 0x00000000;
	msr.lo = 0xc00 | MTRR_TYPE_WRCOMB;
	wrmsr(MTRRdefType_MSR, msr);

	/* Set TOP_MEM to 4G */
	msr.hi = 0x00000001;
	msr.lo = 0x00000000;
	wrmsr(TOP_MEM, msr);

	enable_cache();
}


static inline void clear_2M_ram(unsigned long basek, struct mtrr_state *mtrr_state)
{
		unsigned long limitk;
		unsigned long size;
		void *addr;

		/* Report every 64M */
		if ((basek % (64*1024)) == 0) {

			/* Restore the normal state */
			map_2M_page(0);
			restore_mtrr_state(mtrr_state);
			enable_lapic();

			/* Print a status message */
			printk_debug("%c", (basek >= TOLM_KB)?'+':'-');

			/* Return to the initialization state */
			set_init_ecc_mtrrs();
			disable_lapic();

		}

		limitk = (basek + ZERO_CHUNK_KB) & ~(ZERO_CHUNK_KB - 1);

		size = (limitk - basek) << 10;
		addr = map_2M_page(basek >> 11);
		if (addr == MAPPING_ERROR) {
			printk_err("Cannot map page: %x\n", basek >> 11);
			return;
		}

		/* clear memory 2M (limitk - basek) */
		addr = (void *)(((u32)addr) | ((basek & 0x7ff) << 10));
		clear_memory(addr, size);
}


static void init_ecc_memory(u32 node_id)
{
	unsigned long startk, begink, endk;
	unsigned long hole_startk = 0;
	unsigned long basek;
	struct mtrr_state mtrr_state;

	device_t f1_dev, f2_dev, f3_dev;
	int enable_scrubbing;
	u32 dcl;

	f1_dev = get_node_pci(node_id, 1);

	if (!f1_dev) {
		die("Cannot find cpu function 1\n");
	}
	f2_dev = get_node_pci(node_id, 2);
	if (!f2_dev) {
		die("Cannot find cpu function 2\n");
	}
	f3_dev = get_node_pci(node_id, 3);
	if (!f3_dev) {
		die("Cannot find cpu function 3\n");
	}

	/* See if we scrubbing should be enabled */
	enable_scrubbing = 1;
	get_option(&enable_scrubbing, "hw_scrubber");

	/* Enable cache scrubbing at the lowest possible rate */
	if (enable_scrubbing) {
		pci_write_config32(f3_dev, DRAM_SCRUB_RATE_CTRL,
			(SCRUB_84ms << 16) | (SCRUB_84ms << 8) | (SCRUB_NONE << 0));
	} else {
		pci_write_config32(f3_dev, DRAM_SCRUB_RATE_CTRL,
			(SCRUB_NONE << 16) | (SCRUB_NONE << 8) | (SCRUB_NONE << 0));
		printk_debug("Scrubbing Disabled\n");
	}


	/* If ecc support is not enabled don't touch memory */
	dcl = pci_read_config32(f2_dev, DRAM_CONFIG_LOW);
	if (!(dcl & DCL_DimmEccEn)) {
		printk_debug("ECC Disabled\n");
		return;
	}

	startk = (pci_read_config32(f1_dev, 0x40 + (node_id*8)) & 0xffff0000) >> 2;
	endk   = ((pci_read_config32(f1_dev, 0x44 + (node_id*8)) & 0xffff0000) >> 2) + 0x4000;

#if HW_MEM_HOLE_SIZEK != 0
		u32 val;
		val = pci_read_config32(f1_dev, 0xf0);
		if(val & 1) {
			hole_startk = ((val & (0xff<<24)) >> 10);
		}
#endif


	/* Don't start too early */
	begink = startk;
	if (begink < CONFIG_LB_MEM_TOPK) {
		begink = CONFIG_LB_MEM_TOPK;
	}

	printk_debug("Clearing memory %uK - %uK: ", begink, endk);

	/* Save the normal state */
	save_mtrr_state(&mtrr_state);

	/* Switch to the init ecc state */
	set_init_ecc_mtrrs();
	disable_lapic();

	/* Walk through 2M chunks and zero them */
#if HW_MEM_HOLE_SIZEK != 0
	/* here hole_startk can not be equal to begink, never. Also hole_startk is in 2M boundary, 64M? */
	 if ( (hole_startk != 0) && ((begink < hole_startk) && (endk>(4*1024*1024)))) {
			for(basek = begink; basek < hole_startk;
				basek = ((basek + ZERO_CHUNK_KB) & ~(ZERO_CHUNK_KB - 1)))
			{
				clear_2M_ram(basek, &mtrr_state);
			}
			for(basek = 4*1024*1024; basek < endk;
				basek = ((basek + ZERO_CHUNK_KB) & ~(ZERO_CHUNK_KB - 1)))
			{
				clear_2M_ram(basek, &mtrr_state);
			}
	} else
#endif
	for(basek = begink; basek < endk;
		basek = ((basek + ZERO_CHUNK_KB) & ~(ZERO_CHUNK_KB - 1))) {
		clear_2M_ram(basek, &mtrr_state);
	}


	/* Restore the normal state */
	map_2M_page(0);
	restore_mtrr_state(&mtrr_state);
	enable_lapic();

	/* Set the scrub base address registers */
	pci_write_config32(f3_dev, DRAM_SCRUB_ADDR_LOW,	 startk << 10);
	pci_write_config32(f3_dev, DRAM_SCRUB_ADDR_HIGH, startk >> 22);

	/* Enable the scrubber? */
	if (enable_scrubbing) {
		/* Enable scrubbing at the lowest possible rate */
		pci_write_config32(f3_dev, DRAM_SCRUB_RATE_CTRL,
			(SCRUB_84ms << 16) | (SCRUB_84ms << 8) | (SCRUB_84ms << 0));
	}

	printk_debug(" done\n");
}


static inline void fam10_errata(void)
{
	msr_t msr;
	/* FIXME: Is doing errata here too late? */

	/* 298 : FIXME: Fixed in B3/C1 */
/*	msr = rdmsr(0xC0010015);
	msr.lo |= 1 << 3;
	wrmsr(0xC0010015, msr);

	msr = rdmsr(0xC0010023);
	msr.lo |= 1 << 1;
	wrmsr(0xC0010023, msr);
*/
}

static void smash1Gpages(void)
{
	msr_t msr;

	/* 1G pages are smashed and installed in the TLB as 2M pages.
	   BIOS must set this bit for revision B. */
	/* FIXME: What about RevC? */

	msr = rdmsr(0xC001102A);
	msr.lo |= 1 << 29;
	wrmsr(0xC001102A, msr);

}



void model_10xxx_init(device_t dev)
{
	unsigned long i;
	msr_t msr;
	struct node_core_id id;
#if CONFIG_LOGICAL_CPUS == 1
	unsigned siblings;
#endif

	/* Turn on caching if we haven't already */
	x86_enable_cache();
	amd_setup_mtrrs();
	x86_mtrr_check();

	disable_cache();

	/* zero the machine check error status registers */
	msr.lo = 0;
	msr.hi = 0;
	for(i=0; i < 5; i++) {
		wrmsr(MCI_STATUS + (i * 4),msr);
	}

	fam10_errata();

	enable_cache();

	/* Enable the local cpu apics */
	setup_lapic();

#if CONFIG_LOGICAL_CPUS == 1
	siblings = cpuid_ecx(0x80000008) & 0xff;

	if (siblings > 0) {
		msr = rdmsr_amd(CPU_ID_FEATURES_MSR);
		msr.lo |= 1 << 28;
		wrmsr_amd(CPU_ID_FEATURES_MSR, msr);

		msr = rdmsr_amd(LOGICAL_CPUS_NUM_MSR);
		msr.lo = (siblings+1)<<16;
		wrmsr_amd(LOGICAL_CPUS_NUM_MSR, msr);

		msr = rdmsr_amd(CPU_ID_EXT_FEATURES_MSR);
		msr.hi |= 1<<(33-32);
		wrmsr_amd(CPU_ID_EXT_FEATURES_MSR, msr);
	}
	printk_debug("siblings = %02d, ", siblings);
#endif

	id = get_node_core_id(read_nb_cfg_54()); // pre e0 nb_cfg_54 can not be set

	printk_debug("nodeid = %02d, coreid = %02d\n", id.nodeid, id.coreid);

	init_pstates(dev, id.nodeid, id.coreid); // is it a good place? some cores are clearing their ram

	/* Is this a bad location?  In particular can another node prefecth
	 * data from this node before we have initialized it?
	 */
	if (id.coreid == 0) init_ecc_memory(id.nodeid); // only do it for core 0

#if CONFIG_LOGICAL_CPUS==1
	 /* Start up my cpu siblings */
//	if(id.coreid==0)  amd_sibling_init(dev); // Don't need core1 is already be put in the CPU BUS in bus_cpu_scan
#endif

	smash1Gpages();
}

static struct device_operations cpu_dev_ops = {
	.init = model_10xxx_init,
};
static struct cpu_device_id cpu_table[] = {
//AMD_GH_SUPPORT
	{ X86_VENDOR_AMD, 0x100f00 },		/* SH-F0 L1 */
	{ X86_VENDOR_AMD, 0x100f10 },		/* M2 */
	{ X86_VENDOR_AMD, 0x100f20 },		/* S1g1 */
	{ X86_VENDOR_AMD, 0x100f21 },
	{ X86_VENDOR_AMD, 0x100f2A },
	{ X86_VENDOR_AMD, 0x100f22 },
	{ X86_VENDOR_AMD, 0x100f23 },
	{ 0, 0 },
};
static struct cpu_driver model_10xxx __cpu_driver = {
	.ops	  = &cpu_dev_ops,
	.id_table = cpu_table,
};
