/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google LLC
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

/* SMM relocation with intention to work for i945-ivybridge.
   Right now used for sandybridge and ivybridge.  */

#include <assert.h>
#include <types.h>
#include <string.h>
#include <device/device.h>
#include <device/pci.h>
#include <commonlib/helpers.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <console/console.h>
#include <smp/node.h>
#include "smi.h"

#define SMRR_SUPPORTED (1 << 11)

#define  D_OPEN		(1 << 6)
#define  D_CLS		(1 << 5)
#define  D_LCK		(1 << 4)
#define  G_SMRAME	(1 << 3)
#define  C_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0))

struct ied_header {
	char signature[10];
	u32 size;
	u8 reserved[34];
} __packed;


struct smm_relocation_params {
	u32 smram_base;
	u32 smram_size;
	u32 ied_base;
	u32 ied_size;
	msr_t smrr_base;
	msr_t smrr_mask;
};

/* This gets filled in and used during relocation. */
static struct smm_relocation_params smm_reloc_params;
static void *default_smm_area = NULL;

/* On model_6fx, model_1067x and model_106cx SMRR functions slightly
   differently. The MSR are at different location from the rest
   and need to be explicitly enabled in IA32_FEATURE_CONTROL MSR. */
bool cpu_has_alternative_smrr(void)
{
	struct cpuinfo_x86 c;
	get_fms(&c, cpuid_eax(1));
	if (c.x86 != 6)
		return false;
	switch (c.x86_model) {
	case 0xf:
	case 0x17: /* core2 */
	case 0x1c: /* Bonnell */
		return true;
	default:
		return false;
	}
}

static void write_smrr(struct smm_relocation_params *relo_params)
{
	printk(BIOS_DEBUG, "Writing SMRR. base = 0x%08x, mask=0x%08x\n",
	       relo_params->smrr_base.lo, relo_params->smrr_mask.lo);

	if (cpu_has_alternative_smrr()) {
		msr_t msr;
		msr = rdmsr(IA32_FEATURE_CONTROL);
		/* SMRR enabled and feature locked */
		if (!((msr.lo & SMRR_ENABLE)
				&& (msr.lo & FEATURE_CONTROL_LOCK_BIT))) {
			printk(BIOS_WARNING,
				"SMRR not enabled, skip writing SMRR...\n");
			return;
		}
		wrmsr(MSR_SMRR_PHYS_BASE, relo_params->smrr_base);
		wrmsr(MSR_SMRR_PHYS_MASK, relo_params->smrr_mask);
	} else {
		wrmsr(IA32_SMRR_PHYS_BASE, relo_params->smrr_base);
		wrmsr(IA32_SMRR_PHYS_MASK, relo_params->smrr_mask);
	}
}

static void fill_in_relocation_params(struct smm_relocation_params *params)
{
	/* All range registers are aligned to 4KiB */
	const u32 rmask = ~((1 << 12) - 1);

	const u32 tsegmb = northbridge_get_tseg_base();
	/* TSEG base is usually aligned down (to 8MiB). So we can't
	   derive the TSEG size from the distance to GTT but use the
	   configuration value instead. */
	const u32 tseg_size = northbridge_get_tseg_size();

	params->smram_base = tsegmb;
	params->smram_size = tseg_size;
	if (CONFIG_IED_REGION_SIZE != 0) {
		ASSERT(params->smram_size > CONFIG_IED_REGION_SIZE);
		params->smram_size -= CONFIG_IED_REGION_SIZE;
		params->ied_base = tsegmb + tseg_size - CONFIG_IED_REGION_SIZE;
		params->ied_size = CONFIG_IED_REGION_SIZE;
	}

	/* Adjust available SMM handler memory size. */
	if (CONFIG(CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM)) {
		ASSERT(params->smram_size > CONFIG_SMM_RESERVED_SIZE);
		params->smram_size -= CONFIG_SMM_RESERVED_SIZE;
	}

	if (IS_ALIGNED(tsegmb, tseg_size)) {
		/* SMRR has 32-bits of valid address aligned to 4KiB. */
		struct cpuinfo_x86 c;

		/* On model_6fx and model_1067x bits [0:11] on smrr_base
		   are reserved */
		get_fms(&c, cpuid_eax(1));
		if (cpu_has_alternative_smrr())
			params->smrr_base.lo = (params->smram_base & rmask);
		else
			params->smrr_base.lo = (params->smram_base & rmask)
				| MTRR_TYPE_WRBACK;
		params->smrr_base.hi = 0;
		params->smrr_mask.lo = (~(tseg_size - 1) & rmask)
			| MTRR_PHYS_MASK_VALID;
		params->smrr_mask.hi = 0;
	} else {
		printk(BIOS_WARNING,
		       "TSEG base not aligned with TSEG SIZE! Not setting SMRR\n");
	}
}

static void setup_ied_area(struct smm_relocation_params *params)
{
	char *ied_base;

	struct ied_header ied = {
		.signature = "INTEL RSVD",
		.size = params->ied_size,
		.reserved = {0},
	};

	ied_base = (void *)params->ied_base;

	/* Place IED header at IEDBASE. */
	memcpy(ied_base, &ied, sizeof(ied));

	/* Zero out 32KiB at IEDBASE + 1MiB */
	memset(ied_base + (1 << 20), 0, (32 << 10));
}

void smm_init_completion(void)
{
	restore_default_smm_area(default_smm_area);
}

void smm_lock(void)
{
	/* LOCK the SMM memory window and enable normal SMM.
	 * After running this function, only a full reset can
	 * make the SMM registers writable again.
	 */
	printk(BIOS_DEBUG, "Locking SMM.\n");

	northbridge_write_smram(D_LCK | G_SMRAME | C_BASE_SEG);
}

void smm_info(uintptr_t *perm_smbase, size_t *perm_smsize,
		size_t *smm_save_state_size)
{
	printk(BIOS_DEBUG, "Setting up SMI for CPU\n");

	fill_in_relocation_params(&smm_reloc_params);

	if (CONFIG_IED_REGION_SIZE != 0)
		setup_ied_area(&smm_reloc_params);

	*perm_smbase = smm_reloc_params.smram_base;
	*perm_smsize = smm_reloc_params.smram_size;
	*smm_save_state_size = sizeof(em64t101_smm_state_save_area_t);
}

void smm_initialize(void)
{
	/* Clear the SMM state in the southbridge. */
	southbridge_smm_clear_state();

	/*
	 * Run the relocation handler for on the BSP to check and set up
	 * parallel SMM relocation.
	 */
	smm_initiate_relocation();
}

/* The relocation work is actually performed in SMM context, but the code
 * resides in the ramstage module. This occurs by trampolining from the default
 * SMRAM entry point to here. */
void smm_relocation_handler(int cpu, uintptr_t curr_smbase,
				uintptr_t staggered_smbase)
{
	msr_t mtrr_cap;
	struct smm_relocation_params *relo_params = &smm_reloc_params;
	em64t101_smm_state_save_area_t *save_state;
	u32 smbase = staggered_smbase;
	u32 iedbase = relo_params->ied_base;

	printk(BIOS_DEBUG, "In relocation handler: cpu %d\n", cpu);

	/* Make appropriate changes to the save state map. */
	if (CONFIG_IED_REGION_SIZE != 0)
		printk(BIOS_DEBUG, "New SMBASE=0x%08x IEDBASE=0x%08x\n",
		       smbase, iedbase);
	else
		printk(BIOS_DEBUG, "New SMBASE=0x%08x\n",
		       smbase);

	save_state = (void *)(curr_smbase + SMM_DEFAULT_SIZE -
			sizeof(*save_state));
	save_state->smbase = smbase;
	save_state->iedbase = iedbase;

	/* Write EMRR and SMRR MSRs based on indicated support. */
	mtrr_cap = rdmsr(MTRR_CAP_MSR);
	if (mtrr_cap.lo & SMRR_SUPPORTED && relo_params->smrr_mask.lo != 0)
		write_smrr(relo_params);
}

/*
 * The default SMM entry can happen in parallel or serially. If the
 * default SMM entry is done in parallel the BSP has already setup
 * the saving state to each CPU's MSRs. At least one save state size
 * is required for the initial SMM entry for the BSP to determine if
 * parallel SMM relocation is even feasible.
 */
void smm_relocate(void)
{
	/*
	 * If smm_save_state_in_msrs is non-zero then parallel SMM relocation
	 * shall take place. Run the relocation handler a second time on the
	 * BSP to do the final move. For APs, a relocation handler always
	 * needs to be run.
	 */
	if (!boot_cpu())
		smm_initiate_relocation();
}
