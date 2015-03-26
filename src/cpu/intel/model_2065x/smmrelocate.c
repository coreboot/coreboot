/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 ChromeOS Authors
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
 * Foundation, Inc.
 */

#define __SIMPLE_DEVICE__

#include <types.h>
#include <string.h>
#include <device/device.h>
#include <device/pci.h>
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <console/console.h>
#include <northbridge/intel/nehalem/nehalem.h>
#include <southbridge/intel/ibexpeak/pch.h>
#include "model_2065x.h"

#define CORE_THREAD_COUNT_MSR 0x35

#define SMRR_SUPPORTED (1<<11)

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

static inline void write_smrr(struct smm_relocation_params *relo_params)
{
	printk(BIOS_DEBUG, "Writing SMRR. base = 0x%08x, mask=0x%08x\n",
	       relo_params->smrr_base.lo, relo_params->smrr_mask.lo);
	wrmsr(SMRRphysBase_MSR, relo_params->smrr_base);
	wrmsr(SMRRphysMask_MSR, relo_params->smrr_mask);
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

	/* Write EMRR and SMRR MSRs based on indicated support. */
	mtrr_cap = rdmsr(MTRRcap_MSR);
	if (mtrr_cap.lo & SMRR_SUPPORTED)
		write_smrr(relo_params);

	southbridge_clear_smi_status();
}

static void fill_in_relocation_params(struct smm_relocation_params *params)
{
	u32 tseg_size;
	u32 tsegmb;
	u32 bgsm;
	int phys_bits;
	/* All range registers are aligned to 4KiB */
	const u32 rmask = ~((1 << 12) - 1);

	/* Some of the range registers are dependent on the number of physical
	 * address bits supported. */
	phys_bits = cpuid_eax(0x80000008) & 0xff;

	/* The range bounded by the TSEGMB and BGSM registers encompasses the
	 * SMRAM range as well as the IED range. However, the SMRAM available
	 * to the handler is 4MiB since the IEDRAM lives TSEGMB + 4MiB.
	 */
	tsegmb = pci_read_config32(PCI_DEV(0, 0, 0), TSEG) & ~1;
	bgsm = pci_read_config32(PCI_DEV(0, 0, 0), D0F0_GTT_BASE);
	tseg_size = bgsm - tsegmb;

	params->smram_base = tsegmb;
	params->smram_size = 4 << 20;
	params->ied_base = tsegmb + params->smram_size;
	params->ied_size = tseg_size - params->smram_size;

	/* SMRR has 32-bits of valid address aligned to 4KiB. */
	params->smrr_base.lo = (params->smram_base & rmask) | MTRR_TYPE_WRBACK;
	params->smrr_base.hi = 0;
	params->smrr_mask.lo = (~(tseg_size - 1) & rmask) | MTRRphysMaskValid;
	params->smrr_mask.hi = 0;
}

static void make_apicid_map(int threads_per_package, int threads_per_core, struct smm_loader_params *relo_params)
{
	int i;
	for (i = 0; i < threads_per_package; ++i) {
		relo_params->runtime->apic_id_to_cpu[i] = (i % threads_per_core)
			+ ((i / threads_per_core) << 2);
	}
}

static int install_relocation_handler(int threads_per_package, int threads_per_core,
                                      struct smm_relocation_params *relo_params)
{
	/* The default SMM entry happens serially at the default location.
	 * Therefore, there is only 1 concurrent save state area. Set the
	 * stack size to the save state size, and call into the
	 * do_relocation handler. */
	int save_state_size = sizeof(em64t101_smm_state_save_area_t);
	struct smm_loader_params smm_params = {
		.per_cpu_stack_size = save_state_size,
		.num_concurrent_stacks = threads_per_package,
		.per_cpu_save_state_size = save_state_size,
		.num_concurrent_save_states = 1,
		.handler = &cpu_smm_do_relocation,
		.handler_arg = (void *)relo_params,
	};

	if (smm_setup_relocation_handler(&smm_params))
		return -1;
	make_apicid_map(threads_per_package, threads_per_core, &smm_params);
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

static int install_permanent_handler(int threads_per_package, int threads_per_core,
                                     struct smm_relocation_params *relo_params)
{
	/* There are threads_per_package concurrent stacks and threads_per_package concurrent save
	 * state areas. Lastly, set the stack size to the save state size. */
	int save_state_size = sizeof(em64t101_smm_state_save_area_t);
	struct smm_loader_params smm_params = {
		.per_cpu_stack_size = save_state_size,
		.num_concurrent_stacks = threads_per_package,
		.per_cpu_save_state_size = save_state_size,
		.num_concurrent_save_states = threads_per_package,
	};

	printk(BIOS_DEBUG, "Installing SMM handler to 0x%08x\n",
	       relo_params->smram_base);
	if (smm_load_module((void *)relo_params->smram_base,
			    relo_params->smram_size, &smm_params))
		return -1;
	make_apicid_map(threads_per_package, threads_per_core, &smm_params);
	return 0;
}

static int cpu_smm_setup(void)
{
	device_t qpdev = PCI_DEV(QUICKPATH_BUS, 0, 1);
	struct cpuid_result result;
	unsigned threads_per_package, threads_per_core;

	/* Logical processors (threads) per core */
	result = cpuid_ext(0xb, 0);
	threads_per_core = result.ebx & 0xffff;

	/* Logical processors (threads) per package */
	result = cpuid_ext(0xb, 1);
	threads_per_package = result.ebx & 0xffff;

	printk(BIOS_DEBUG, "Setting up SMI for CPU\n");

	/* enable the SMM memory window */
	pci_write_config8(qpdev, QPD0F1_SMRAM, D_OPEN | G_SMRAME | C_BASE_SEG);

	fill_in_relocation_params(&smm_reloc_params);

	setup_ied_area(&smm_reloc_params);

	if (threads_per_package > CONFIG_MAX_CPUS) {
		printk(BIOS_CRIT,
		       "Error: Hardware CPUs (%d) > MAX_CPUS (%d)\n",
		       threads_per_package, CONFIG_MAX_CPUS);
	}

	if (install_relocation_handler(threads_per_package, threads_per_core, &smm_reloc_params)) {
		printk(BIOS_CRIT, "SMM Relocation handler install failed.\n");
		return -1;
	}

	if (install_permanent_handler(threads_per_package, threads_per_core, &smm_reloc_params)) {
		printk(BIOS_CRIT, "SMM Permanent handler install failed.\n");
		return -1;
	}

	/* Ensure the SMM handlers hit DRAM before performing first SMI. */
	/* TODO(adurbin): Is this really needed? */
	wbinvd();

	/* close the SMM memory window and enable normal SMM */
	pci_write_config8(qpdev, QPD0F1_SMRAM, G_SMRAME | C_BASE_SEG);

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

void smm_lock(void)
{
	/* LOCK the SMM memory window and enable normal SMM.
	 * After running this function, only a full reset can
	 * make the SMM registers writable again.
	 */
	printk(BIOS_DEBUG, "Locking SMM.\n");
	pci_write_config8(PCI_DEV(QUICKPATH_BUS, 0, 1), QPD0F1_SMRAM,
			D_LCK | G_SMRAME | C_BASE_SEG);
}
