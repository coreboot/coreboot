/* Needed so the AMD K8 runs correctly.  */
/* this should be done by Eric
 * 2004.11 yhlu add d0 e0 support
 * 2004.12 yhlu add dual core support
 * 2005.02 yhlu add e0 memory hole support

*/
/*============================================================================
Copyright 2005 ADVANCED MICRO DEVICES, INC. All Rights Reserved.
This software and any related documentation (the "Materials") are the
confidential proprietary information of AMD. Unless otherwise provided in a
software agreement specifically licensing the Materials, the Materials are
provided in confidence and may not be distributed, modified, or reproduced in
whole or in part by any means.
LIMITATION OF LIABILITY: THE MATERIALS ARE PROVIDED "AS IS" WITHOUT ANY
EXPRESS OR IMPLIED WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO
WARRANTIES OF MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY
PARTICULAR PURPOSE, OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR
USAGE OF TRADE. IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY
DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS,
BUSINESS INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF THE USE OF OR
INABILITY TO USE THE MATERIALS, EVEN IF AMD HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES. BECAUSE SOME JURISDICTIONS PROHIBIT THE EXCLUSION
OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES, THE ABOVE
LIMITATION MAY NOT APPLY TO YOU.
AMD does not assume any responsibility for any errors which may appear in the
Materials nor any responsibility to support or update the Materials. AMD
retains the right to modify the Materials at any time, without notice, and is
not obligated to provide such modified Materials to you.
NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
further information, software, technical information, know-how, or show-how
available to you.
U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with "RESTRICTED
RIGHTS." Use, duplication, or disclosure by the Government is subject to the
restrictions as set forth in FAR 52.227-14 and DFAR 252.227-7013, et seq., or
its successor. Use of the Materials by the Government constitutes
acknowledgement of AMD's proprietary rights in them.
============================================================================*/
//@DOC
// in model_fxx_init.c
/*
$1.0$
*/
// Description: microcode patch support for k8
// by yhlu
//
//============================================================================

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/device.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/pae.h>
#include <pc80/mc146818rtc.h>
#include <cpu/x86/lapic.h>
#include "../../../northbridge/amd/amdk8/amdk8.h"
#include "../../../northbridge/amd/amdk8/cpu_rev.c"
#include <cpu/cpu.h>
#include <cpu/amd/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/mem.h>
#include <cpu/amd/dualcore.h>

static uint8_t microcode_updates[] __attribute__ ((aligned(16))) = {

#include "microcode_rev_c.h"
#include "microcode_rev_d.h"
#include "microcode_rev_e.h"
        /*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};

#include "model_fxx_msr.h"

#define MCI_STATUS 0x401

static inline msr_t rdmsr_amd(unsigned index)
{
        msr_t result;
        __asm__ __volatile__ (
                "rdmsr"
                : "=a" (result.lo), "=d" (result.hi)
                : "c" (index), "D" (0x9c5a203a)
                );
        return result;
}

static inline void wrmsr_amd(unsigned index, msr_t msr)
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
	state->top_mem  = rdmsr(TOP_MEM);
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
	wrmsr(TOP_MEM,         state->top_mem);
	wrmsr(TOP_MEM2,        state->top_mem2);
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
	printk_debug("top_mem:  %08x%08x\n",
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

static void init_ecc_memory(unsigned node_id)
{
	unsigned long startk, begink, endk;
	unsigned long basek;
	struct mtrr_state mtrr_state;
	device_t f1_dev, f2_dev, f3_dev;
	int enable_scrubbing;
	uint32_t dcl;

	f1_dev = dev_find_slot(0, PCI_DEVFN(0x18 + node_id, 1));
	if (!f1_dev) {
		die("Cannot find cpu function 1\n");
	}
	f2_dev = dev_find_slot(0, PCI_DEVFN(0x18 + node_id, 2));
	if (!f2_dev) {
		die("Cannot find cpu function 2\n");
	}
	f3_dev = dev_find_slot(0, PCI_DEVFN(0x18 + node_id, 3));
	if (!f3_dev) {
		die("Cannot find cpu function 3\n");
	}

	/* See if we scrubbing should be enabled */
	enable_scrubbing = 1;
	get_option(&enable_scrubbing, "hw_scrubber");

	/* Enable cache scrubbing at the lowest possible rate */
	if (enable_scrubbing) {
		pci_write_config32(f3_dev, SCRUB_CONTROL,
			(SCRUB_84ms << 16) | (SCRUB_84ms << 8) | (SCRUB_NONE << 0));
	} else {
		pci_write_config32(f3_dev, SCRUB_CONTROL,
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
	for(basek = begink; basek < endk; 
		basek = ((basek + ZERO_CHUNK_KB) & ~(ZERO_CHUNK_KB - 1))) 
	{
		unsigned long limitk;
		unsigned long size;
		void *addr;

		/* Report every 64M */
		if ((basek % (64*1024)) == 0) {
			/* Restore the normal state */
			map_2M_page(0);
			restore_mtrr_state(&mtrr_state);
			enable_lapic();

			/* Print a status message */
			printk_debug("%c", (basek >= TOLM_KB)?'+':'-');

			/* Return to the initialization state */
			set_init_ecc_mtrrs();
			disable_lapic();
		}

		limitk = (basek + ZERO_CHUNK_KB) & ~(ZERO_CHUNK_KB - 1);
		if (limitk > endk) {
			limitk = endk;
		}
		size = (limitk - basek) << 10;
		addr = map_2M_page(basek >> 11);
		if (addr == MAPPING_ERROR) {
			printk_err("Cannot map page: %x\n", basek >> 11);
			continue;
		}

		/* clear memory 2M (limitk - basek) */
		addr = (void *)(((uint32_t)addr) | ((basek & 0x7ff) << 10));
		clear_memory(addr, size);
	}
	/* Restore the normal state */
	map_2M_page(0);
	restore_mtrr_state(&mtrr_state);
	enable_lapic();

	/* Set the scrub base address registers */
	pci_write_config32(f3_dev, SCRUB_ADDR_LOW,  startk << 10);
	pci_write_config32(f3_dev, SCRUB_ADDR_HIGH, startk >> 22);

	/* Enable the scrubber? */
	if (enable_scrubbing) {
		/* Enable scrubbing at the lowest possible rate */
		pci_write_config32(f3_dev, SCRUB_CONTROL,
			(SCRUB_84ms << 16) | (SCRUB_84ms << 8) | (SCRUB_84ms << 0));
	}

	printk_debug(" done\n");
}

static inline void k8_errata(void)
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
		msr.lo |=  (1 << 10);
		wrmsr_amd(DC_CFG_MSR, msr);
			
	}
	/* I can't touch this msr on early buggy cpus */
	if (!is_cpu_pre_b3()) {

		/* Erratum 89 ... */
		msr = rdmsr(NB_CFG_MSR);
		msr.lo |= 1 << 3;
		
		if (!is_cpu_pre_c0() && is_cpu_pre_d0()) {
			/* D0 later don't need it */
			/* Erratum 86 Disable data masking on C0 and 
			 * later processor revs.
			 * FIXME this is only needed if ECC is enabled.
			 */
			msr.hi |= 1 << (36 - 32);
		}	
		wrmsr(NB_CFG_MSR, msr);
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

	if (is_cpu_pre_e0() && !is_cpu_pre_d0()) {
		/* Erratum 110 ...*/
		msr = rdmsr_amd(CPU_ID_HYPER_EXT_FEATURES);
		msr.hi |=1;
		wrmsr_amd(CPU_ID_HYPER_EXT_FEATURES, msr);
 	}

	if (!is_cpu_pre_e0()) {
		/* Erratum 110 ... */
                msr = rdmsr_amd(CPU_ID_EXT_FEATURES_MSR);
                msr.hi |=1;
                wrmsr_amd(CPU_ID_EXT_FEATURES_MSR, msr);

		/* Erratum 113 ... */
               msr = rdmsr_amd(BU_CFG_MSR);
               msr.hi |= (1 << 16);
               wrmsr_amd(BU_CFG_MSR, msr);
	}

	/* Erratum 122 */
	if (!is_cpu_pre_c0()) {
		msr = rdmsr(HWCR_MSR);
		msr.lo |= 1 << 6;
		wrmsr(HWCR_MSR, msr);
	}

	/* Erratum 123? dual core deadlock? */
	
	/* Erratum 131 */
	msr = rdmsr(NB_CFG_MSR);
	msr.lo |= 1 << 20;
	wrmsr(NB_CFG_MSR, msr);
	
}

static unsigned id_mapping_table[] = {
	0x0f48, 0x0048,
	0x0f58, 0x0048,

	0x0f4a, 0x004a,
	0x0f5a, 0x004a,
	0x0f7a, 0x004a,
	0x0f82, 0x004a,
	0x0fc0, 0x004a,
	0x0ff0, 0x004a,

	0x10f50, 0x0150,
	0x10f70, 0x0150,
	0x10f80, 0x0150,
	0x10fc0, 0x0150,
	0x10ff0, 0x0150,

	0x20f10, 0x0210,
	0x20f12, 0x0210,
	0x20f32, 0x0210,
	0x20fb1, 0x0210,	

}; 

static unsigned get_equivalent_processor_rev_id(unsigned orig_id) {
	unsigned new_id;
	int i;
	
	new_id = 0;

	for(i=0; i<sizeof(id_mapping_table); i+=2 ) {
		if(id_mapping_table[i]==orig_id) {
			new_id = id_mapping_table[i+1];
			break;
		}
	}

	return new_id;	

}

void model_fxx_init(device_t cpu)
{
	unsigned long i;
	msr_t msr;
	struct node_core_id id;
	unsigned equivalent_processor_rev_id;

	/* Turn on caching if we haven't already */
	x86_enable_cache();
	amd_setup_mtrrs();
	x86_mtrr_check();

        /* Update the microcode */
	equivalent_processor_rev_id = get_equivalent_processor_rev_id(cpu->device );
	if(equivalent_processor_rev_id != 0)
	        amd_update_microcode(microcode_updates, equivalent_processor_rev_id);
	
	disable_cache();
	
	/* zero the machine check error status registers */
	msr.lo = 0;
	msr.hi = 0;
	for(i=0; i<5; i++) {
		wrmsr(MCI_STATUS + (i*4),msr);
	}

	k8_errata();
	
	enable_cache();

	/* Enable the local cpu apics */
	setup_lapic();

	/* Find our node and core */
	id = get_node_core_id();

	/* Is this a bad location?  In particular can another node prefetch
	 * data from this node before we have initialized it?
	 */
	if (id.coreid == 0) {
		init_ecc_memory(id.nodeid); // only do it for core 0
	}

	/* Deal with sibling cpus */
	amd_sibling_init(cpu, id);
}

static struct device_operations cpu_dev_ops = {
	.init = model_fxx_init,
};
static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, 0xf50 }, /* B3 */
	{ X86_VENDOR_AMD, 0xf51 }, /* SH7-B3 */
	{ X86_VENDOR_AMD, 0xf58 }, /* SH7-C0 */
	{ X86_VENDOR_AMD, 0xf48 },

	{ X86_VENDOR_AMD, 0xf5A }, /* SH7-CG */
	{ X86_VENDOR_AMD, 0xf4A },
	{ X86_VENDOR_AMD, 0xf7A },
	{ X86_VENDOR_AMD, 0xfc0 }, /* DH7-CG */
	{ X86_VENDOR_AMD, 0xfe0 },
	{ X86_VENDOR_AMD, 0xff0 },
	{ X86_VENDOR_AMD, 0xf82 }, /* CH7-CG */
	{ X86_VENDOR_AMD, 0xfb2 },
//AMD_D0_SUPPORT
	{ X86_VENDOR_AMD, 0x10f50 }, /* SH7-D0 */
	{ X86_VENDOR_AMD, 0x10f40 },
	{ X86_VENDOR_AMD, 0x10f70 },
        { X86_VENDOR_AMD, 0x10fc0 }, /* DH7-D0 */
        { X86_VENDOR_AMD, 0x10ff0 },
        { X86_VENDOR_AMD, 0x10f80 }, /* CH7-D0 */
        { X86_VENDOR_AMD, 0x10fb0 },
//AMD_E0_SUPPORT
        { X86_VENDOR_AMD, 0x20f50 }, /* SH8-E0*/
        { X86_VENDOR_AMD, 0x20f40 },
        { X86_VENDOR_AMD, 0x20f70 },
        { X86_VENDOR_AMD, 0x20fc0 }, /* DH8-E0 */ /* DH-E3 */
        { X86_VENDOR_AMD, 0x20ff0 },
        { X86_VENDOR_AMD, 0x20f10 }, /* JH8-E1 */
        { X86_VENDOR_AMD, 0x20f30 },
        { X86_VENDOR_AMD, 0x20f51 }, /* SH-E4 */
        { X86_VENDOR_AMD, 0x20f71 },
        { X86_VENDOR_AMD, 0x20f42 }, /* SH-E5 */
        { X86_VENDOR_AMD, 0x20ff2 }, /* DH-E6 */
        { X86_VENDOR_AMD, 0x20fc2 },
        { X86_VENDOR_AMD, 0x20f12 }, /* JH-E6 */
        { X86_VENDOR_AMD, 0x20f32 },

	{ 0, 0 },
};
static struct cpu_driver model_fxx __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
