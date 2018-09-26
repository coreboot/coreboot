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

#include <types.h>
#include <string.h>
#include <compiler.h>
#include <device/device.h>
#include <device/pci.h>
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <console/console.h>
#include "smi.h"

#define SMRR_SUPPORTED (1 << 11)

#define  D_OPEN		(1 << 6)
#define  D_CLS		(1 << 5)
#define  D_LCK		(1 << 4)
#define  G_SMRAME	(1 << 3)
#define  C_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0))

#define IA32_FEATURE_CONTROL	0x3a
#define FEATURE_CONTROL_LOCK_BIT	(1 << 0)
#define SMRR_ENABLE			(1 << 3)

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

static void write_smrr(struct smm_relocation_params *relo_params)
{
	struct cpuinfo_x86 c;

	printk(BIOS_DEBUG, "Writing SMRR. base = 0x%08x, mask=0x%08x\n",
	       relo_params->smrr_base.lo, relo_params->smrr_mask.lo);
	/* Both model_6fx and model_1067x SMRR function slightly differently
	   from the rest. The MSR are at different location from the rest
	   and need to be explicitly enabled. */
	get_fms(&c, cpuid_eax(1));
	if (c.x86 == 6 && (c.x86_model == 0xf || c.x86_model == 0x17)) {
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

/* The relocation work is actually performed in SMM context, but the code
 * resides in the ramstage module. This occurs by trampolining from the default
 * SMRAM entry point to here. */
static void asmlinkage cpu_smm_do_relocation(void *arg)
{
	em64t101_smm_state_save_area_t *save_state;
	msr_t mtrr_cap;
	struct smm_relocation_params *relo_params;
	const struct smm_module_params *p;
	const struct smm_runtime *runtime;
	int cpu;

	p = arg;
	runtime = p->runtime;
	relo_params = p->arg;
	cpu = p->cpu;

	if (cpu >= CONFIG_MAX_CPUS) {
		printk(BIOS_CRIT,
		       "Invalid CPU number assigned in SMM stub: %d\n", cpu);
		return;
	}

	printk(BIOS_DEBUG, "In relocation handler: cpu %d\n", cpu);

	/* All threads need to set IEDBASE and SMBASE in the save state area.
	 * Since one thread runs at a time during the relocation the save state
	 * is the same for all cpus. */
	save_state = (void *)(runtime->smbase + SMM_DEFAULT_SIZE -
			      runtime->save_state_size);

	/* The relocated handler runs with all CPUs concurrently. Therefore
	 * stagger the entry points adjusting SMBASE downwards by save state
	 * size * CPU num. */
	save_state->smbase = relo_params->smram_base -
			     cpu * runtime->save_state_size;
	save_state->iedbase = relo_params->ied_base;

	printk(BIOS_DEBUG, "New SMBASE=0x%08x IEDBASE=0x%08x @ %p\n",
	       save_state->smbase, save_state->iedbase, save_state);

	/* Write SMRR MSRs based on indicated support. */
	mtrr_cap = rdmsr(MTRR_CAP_MSR);
	if (mtrr_cap.lo & SMRR_SUPPORTED)
		write_smrr(relo_params);

	southbridge_clear_smi_status();
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

	/* The SMRAM available to the handler is 4MiB
	   since the IEDRAM lives at TSEGMB + 4MiB. */
	params->smram_base = tsegmb;
	params->smram_size = 4 << 20;
	params->ied_base = tsegmb + params->smram_size;
	params->ied_size = tseg_size - params->smram_size;

	/* Adjust available SMM handler memory size. */
	if (IS_ENABLED(CONFIG_CACHE_RELOCATED_RAMSTAGE_OUTSIDE_CBMEM))
		params->smram_size -= CONFIG_SMM_RESERVED_SIZE;

	if (IS_ALIGNED(tsegmb, tseg_size)) {
		/* SMRR has 32-bits of valid address aligned to 4KiB. */
		struct cpuinfo_x86 c;

		/* On model_6fx and model_1067x bits [0:11] on smrr_base
		   are reserved */
		get_fms(&c, cpuid_eax(1));
		if (c.x86 == 6 && (c.x86_model == 0xf || c.x86_model == 0x17))
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

static int install_relocation_handler(int *apic_id_map, int num_cpus,
				      struct smm_relocation_params *relo_params)
{
	/* The default SMM entry happens serially at the default location.
	 * Therefore, there is only 1 concurrent save state area. Set the
	 * stack size to the save state size, and call into the
	 * do_relocation handler. */
	int save_state_size = sizeof(em64t101_smm_state_save_area_t);
	struct smm_loader_params smm_params = {
		.per_cpu_stack_size = save_state_size,
		.num_concurrent_stacks = num_cpus,
		.per_cpu_save_state_size = save_state_size,
		.num_concurrent_save_states = 1,
		.handler = &cpu_smm_do_relocation,
		.handler_arg = (void *)relo_params,
	};

	default_smm_area = backup_default_smm_area();

	if (smm_setup_relocation_handler(&smm_params))
		return -1;
	int i;
	for (i = 0; i < num_cpus; i++)
		smm_params.runtime->apic_id_to_cpu[i] = apic_id_map[i];
	return 0;
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

static int install_permanent_handler(int *apic_id_map, int num_cpus,
				     struct smm_relocation_params *relo_params)
{
	/* There are num_cpus concurrent stacks and num_cpus concurrent save
	 * state areas. Lastly, set the stack size to the save state size. */
	int save_state_size = sizeof(em64t101_smm_state_save_area_t);
	struct smm_loader_params smm_params = {
		.per_cpu_stack_size = save_state_size,
		.num_concurrent_stacks = num_cpus,
		.per_cpu_save_state_size = save_state_size,
		.num_concurrent_save_states = num_cpus,
	};

	printk(BIOS_DEBUG, "Installing SMM handler to 0x%08x\n",
	       relo_params->smram_base);
	if (smm_load_module((void *)relo_params->smram_base,
			    relo_params->smram_size, &smm_params))
		return -1;
	int i;
	for (i = 0; i < num_cpus; i++)
		smm_params.runtime->apic_id_to_cpu[i] = apic_id_map[i];
	return 0;
}

static int cpu_smm_setup(void)
{
	int num_cpus;
	int apic_id_map[CONFIG_MAX_CPUS];

	printk(BIOS_DEBUG, "Setting up SMI for CPU\n");

	fill_in_relocation_params(&smm_reloc_params);

	/* enable the SMM memory window */
	northbridge_write_smram(D_OPEN | G_SMRAME | C_BASE_SEG);

	setup_ied_area(&smm_reloc_params);

	num_cpus = cpu_get_apic_id_map(apic_id_map);
	if (num_cpus > CONFIG_MAX_CPUS) {
		printk(BIOS_CRIT,
		       "Error: Hardware CPUs (%d) > MAX_CPUS (%d)\n",
		       num_cpus, CONFIG_MAX_CPUS);
	}

	if (install_relocation_handler(apic_id_map, num_cpus,
		&smm_reloc_params)) {
		printk(BIOS_CRIT, "SMM Relocation handler install failed.\n");
		return -1;
	}

	if (install_permanent_handler(apic_id_map, num_cpus,
		&smm_reloc_params)) {
		printk(BIOS_CRIT, "SMM Permanent handler install failed.\n");
		return -1;
	}

	/* Ensure the SMM handlers hit DRAM before performing first SMI. */
	/* TODO(adurbin): Is this really needed? */
	wbinvd();

	/* close the SMM memory window and enable normal SMM */
	northbridge_write_smram(G_SMRAME | C_BASE_SEG);

	return 0;
}

void smm_init(void)
{
	/* Return early if CPU SMM setup failed. */
	if (cpu_smm_setup())
		return;

	southbridge_smm_init();

	/* Initiate first SMI to kick off SMM-context relocation. Note: this
	 * SMI being triggered here queues up an SMI in the APs which are in
	 * wait-for-SIPI state. Once an AP gets an SIPI it will service the SMI
	 * at the SMM_DEFAULT_BASE before jumping to startup vector. */
	southbridge_trigger_smi();

	printk(BIOS_DEBUG, "Relocation complete.\n");

	/* Lock down the SMRAM space. */
	smm_lock();
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
