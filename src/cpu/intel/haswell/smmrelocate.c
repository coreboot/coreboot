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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <string.h>
#include <device/device.h>
#include <device/pci.h>
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <console/console.h>
#include <northbridge/intel/haswell/haswell.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include "haswell.h"

#define EMRRphysBase_MSR 0x1f4
#define EMRRphysMask_MSR 0x1f5
#define UNCORE_EMRRphysBase_MSR 0x2f4
#define UNCORE_EMRRphysMask_MSR 0x2f5
#define SMM_MCA_CAP_MSR 0x17d
#define   SMM_CPU_SVRSTR_BIT 57
#define   SMM_CPU_SVRSTR_MASK (1 << (SMM_CPU_SVRSTR_BIT - 32))
#define SMM_FEATURE_CONTROL_MSR 0x4e0
#define   SMM_CPU_SAVE_EN (1 << 1)
/* SMM save state MSRs */
#define SMBASE_MSR 0xc20
#define IEDBASE_MSR 0xc22

#define SMRR_SUPPORTED (1<<11)
#define EMRR_SUPPORTED (1<<12)

struct smm_relocation_params {
	u32 smram_base;
	u32 smram_size;
	u32 ied_base;
	u32 ied_size;
	msr_t smrr_base;
	msr_t smrr_mask;
	msr_t emrr_base;
	msr_t emrr_mask;
	msr_t uncore_emrr_base;
	msr_t uncore_emrr_mask;
	/* The smm_save_state_in_msrs field indicates if SMM save state
	 * locations live in MSRs. This indicates to the CPUs how to adjust
	 * the SMMBASE and IEDBASE */
	int smm_save_state_in_msrs;
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

static inline void write_emrr(struct smm_relocation_params *relo_params)
{
	printk(BIOS_DEBUG, "Writing EMRR. base = 0x%08x, mask=0x%08x\n",
	       relo_params->emrr_base.lo, relo_params->emrr_mask.lo);
	wrmsr(EMRRphysBase_MSR, relo_params->emrr_base);
	wrmsr(EMRRphysMask_MSR, relo_params->emrr_mask);
}

static inline void write_uncore_emrr(struct smm_relocation_params *relo_params)
{
	printk(BIOS_DEBUG,
	       "Writing UNCORE_EMRR. base = 0x%08x, mask=0x%08x\n",
	       relo_params->uncore_emrr_base.lo,
	       relo_params->uncore_emrr_mask.lo);
	wrmsr(UNCORE_EMRRphysBase_MSR, relo_params->uncore_emrr_base);
	wrmsr(UNCORE_EMRRphysMask_MSR, relo_params->uncore_emrr_mask);
}

static void update_save_state(int cpu,
                              struct smm_relocation_params *relo_params,
                              const struct smm_runtime *runtime)
{
	u32 smbase;
	u32 iedbase;

	/* The relocated handler runs with all CPUs concurrently. Therefore
	 * stagger the entry points adjusting SMBASE downwards by save state
	 * size * CPU num. */
	smbase = relo_params->smram_base - cpu * runtime->save_state_size;
	iedbase = relo_params->ied_base;

	printk(BIOS_DEBUG, "New SMBASE=0x%08x IEDBASE=0x%08x\n",
	       smbase, iedbase);

	/* All threads need to set IEDBASE and SMBASE to the relocated
	 * handler region. However, the save state location depends on the
	 * smm_save_state_in_msrs field in the relocation parameters. If
	 * smm_save_state_in_msrs is non-zero then the CPUs are relocating
	 * the SMM handler in parallel, and each CPUs save state area is
	 * located in their respective MSR space. If smm_save_state_in_msrs
	 * is zero then the SMM relocation is happening serially so the
	 * save state is at the same default location for all CPUs. */
	if (relo_params->smm_save_state_in_msrs) {
		msr_t smbase_msr;
		msr_t iedbase_msr;

		smbase_msr.lo = smbase;
		smbase_msr.hi = 0;

		/* According the BWG the IEDBASE MSR is in bits 63:32. It's
		 * not clear why it differs from the SMBASE MSR. */
		iedbase_msr.lo = 0;
		iedbase_msr.hi = iedbase;

		wrmsr(SMBASE_MSR, smbase_msr);
		wrmsr(IEDBASE_MSR, iedbase_msr);
	} else {
		em64t101_smm_state_save_area_t *save_state;

		save_state = (void *)(runtime->smbase + SMM_DEFAULT_SIZE -
				      runtime->save_state_size);

		save_state->smbase = smbase;
		save_state->iedbase = iedbase;
	}
}

/* Returns 1 if SMM MSR save state was set. */
static int bsp_setup_msr_save_state(struct smm_relocation_params *relo_params)
{
	msr_t smm_mca_cap;

	smm_mca_cap = rdmsr(SMM_MCA_CAP_MSR);
	if (smm_mca_cap.hi & SMM_CPU_SVRSTR_MASK) {
		msr_t smm_feature_control;

		smm_feature_control = rdmsr(SMM_FEATURE_CONTROL_MSR);
		smm_feature_control.hi = 0;
		smm_feature_control.lo |= SMM_CPU_SAVE_EN;
		wrmsr(SMM_FEATURE_CONTROL_MSR, smm_feature_control);
		relo_params->smm_save_state_in_msrs = 1;
	}
	return relo_params->smm_save_state_in_msrs;
}

/* The relocation work is actually performed in SMM context, but the code
 * resides in the ramstage module. This occurs by trampolining from the default
 * SMRAM entry point to here. */
static void __attribute__((cdecl))
cpu_smm_do_relocation(void *arg, int cpu, const struct smm_runtime *runtime)
{
	msr_t mtrr_cap;
	struct smm_relocation_params *relo_params = arg;

	if (cpu >= CONFIG_MAX_CPUS) {
		printk(BIOS_CRIT,
		       "Invalid CPU number assigned in SMM stub: %d\n", cpu);
		return;
	}

	printk(BIOS_DEBUG, "In relocation handler: cpu %d\n", cpu);

	/* Determine if the processor supports saving state in MSRs. If so,
	 * enable it before the non-BSPs run so that SMM relocation can occur
	 * in parallel in the non-BSP CPUs. */
	if (cpu == 0) {
		/* If smm_save_state_in_msrs is 1 then that means this is the
		 * 2nd time through the relocation handler for the BSP.
		 * Parallel SMM handler relocation is taking place. However,
		 * it is desired to access other CPUs save state in the real
		 * SMM handler. Therefore, disable the SMM save state in MSRs
		 * feature. */
		if (relo_params->smm_save_state_in_msrs) {
			msr_t smm_feature_control;

			smm_feature_control = rdmsr(SMM_FEATURE_CONTROL_MSR);
			smm_feature_control.lo &= ~SMM_CPU_SAVE_EN;
			wrmsr(SMM_FEATURE_CONTROL_MSR, smm_feature_control);
		} else if (bsp_setup_msr_save_state(relo_params))
			/* Just return from relocation handler if MSR save
			 * state is enabled. In that case the BSP will come
			 * back into the relocation handler to setup the new
			 * SMBASE as well disabling SMM save state in MSRs. */
			return;
	}

	/* Make appropriate changes to the save state map. */
	update_save_state(cpu, relo_params, runtime);

	/* Write EMRR and SMRR MSRs based on indicated support. */
	mtrr_cap = rdmsr(MTRRcap_MSR);
	if (mtrr_cap.lo & SMRR_SUPPORTED)
		write_smrr(relo_params);

	if (mtrr_cap.lo & EMRR_SUPPORTED) {
		write_emrr(relo_params);
		/* UNCORE_EMRR msrs are package level. Therefore, only
		 * configure these MSRs on the BSP. */
		if (cpu == 0)
			write_uncore_emrr(relo_params);
	}
}

static u32 northbridge_get_base_reg(device_t dev, int reg)
{
	u32 value;

	value = pci_read_config32(dev, reg);
	/* Base registers are at 1MiB granularity. */
	value &= ~((1 << 20) - 1);
	return value;
}

static void fill_in_relocation_params(device_t dev,
                                      struct smm_relocation_params *params)
{
	u32 tseg_size;
	u32 tsegmb;
	u32 bgsm;
	u32 emrr_base;
	u32 emrr_size;
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
	tsegmb = northbridge_get_base_reg(dev, TSEG);
	bgsm = northbridge_get_base_reg(dev, BGSM);
	tseg_size = bgsm - tsegmb;

	params->smram_base = tsegmb;
	params->smram_size = 4 << 20;
	params->ied_base = tsegmb + params->smram_size;
	params->ied_size = tseg_size - params->smram_size;

	/* Adjust available SMM handler memory size. */
	params->smram_size -= RESERVED_SMM_SIZE;

	/* SMRR has 32-bits of valid address aligned to 4KiB. */
	params->smrr_base.lo = (params->smram_base & rmask) | MTRR_TYPE_WRBACK;
	params->smrr_base.hi = 0;
	params->smrr_mask.lo = (~(tseg_size - 1) & rmask) | MTRRphysMaskValid;
	params->smrr_mask.hi = 0;

	/* The EMRR and UNCORE_EMRR are at IEDBASE + 2MiB */
	emrr_base = (params->ied_base + (2 << 20)) & rmask;
	emrr_size = params->ied_size - (2 << 20);

	/* EMRR has 46 bits of valid address aligned to 4KiB. It's dependent
	 * on the number of physical address bits supported. */
	params->emrr_base.lo = emrr_base | MTRR_TYPE_WRBACK;
	params->emrr_base.hi = 0;
	params->emrr_mask.lo = (~(emrr_size - 1) & rmask) | MTRRphysMaskValid;
	params->emrr_mask.hi = (1 << (phys_bits - 32)) - 1;

	/* UNCORE_EMRR has 39 bits of valid address aligned to 4KiB. */
	params->uncore_emrr_base.lo = emrr_base;
	params->uncore_emrr_base.hi = 0;
	params->uncore_emrr_mask.lo = (~(emrr_size - 1) & rmask) |
	                              MTRRphysMaskValid;
	params->uncore_emrr_mask.hi = (1 << (39 - 32)) - 1;
}

static int install_relocation_handler(int num_cpus,
                                      struct smm_relocation_params *relo_params)
{
	/* The default SMM entry can happen in parallel or serially. If the
	 * default SMM entry is done in parallel the BSP has already setup
	 * the saving state to each CPU's MSRs. At least one save state size
	 * is required for the initial SMM entry for the BSP to determine if
	 * parallel SMM relocation is even feasible.  Set the stack size to
	 * the save state size, and call into the do_relocation handler. */
	int save_state_size = sizeof(em64t101_smm_state_save_area_t);
	struct smm_loader_params smm_params = {
		.per_cpu_stack_size = save_state_size,
		.num_concurrent_stacks = num_cpus,
		.per_cpu_save_state_size = save_state_size,
		.num_concurrent_save_states = 1,
		.handler = (smm_handler_t)&cpu_smm_do_relocation,
		.handler_arg = (void *)relo_params,
	};

	return smm_setup_relocation_handler(&smm_params);
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

	/* According to the BWG MP init section 2MiB of memory at IEDBASE +
	 * 2MiB should be zeroed as well. However, I suspect what is inteneded
	 * is to clear the memory covered by EMRR. TODO(adurbin): figure out if 	 * this is really required. */
	//memset(ied_base + (2 << 20), 0, (2 << 20));
}

static int install_permanent_handler(int num_cpus,
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
	return smm_load_module((void *)relo_params->smram_base,
	                       relo_params->smram_size, &smm_params);
}

static int cpu_smm_setup(void)
{
	device_t dev;
	int num_cpus;
	msr_t msr;

	printk(BIOS_DEBUG, "Setting up SMI for CPU\n");

	dev = dev_find_slot(0, PCI_DEVFN(0, 0));

	fill_in_relocation_params(dev, &smm_reloc_params);

	setup_ied_area(&smm_reloc_params);

	msr = rdmsr(CORE_THREAD_COUNT_MSR);
	num_cpus = msr.lo & 0xffff;
	if (num_cpus > CONFIG_MAX_CPUS) {
		printk(BIOS_CRIT,
		       "Error: Hardware CPUs (%d) > MAX_CPUS (%d)\n",
		       num_cpus, CONFIG_MAX_CPUS);
	}

	if (install_relocation_handler(num_cpus, &smm_reloc_params)) {
		printk(BIOS_CRIT, "SMM Relocation handler install failed.\n");
		return -1;
	}

	if (install_permanent_handler(num_cpus, &smm_reloc_params)) {
		printk(BIOS_CRIT, "SMM Permanent handler install failed.\n");
		return -1;
	}

	/* Ensure the SMM handlers hit DRAM before performing first SMI. */
	/* TODO(adurbin): Is this really needed? */
	wbinvd();

	return 0;
}

int smm_initialize(void)
{
	/* Return early if CPU SMM setup failed. */
	if (cpu_smm_setup())
		return -1;

	/* Clear the SMM state in the southbridge. */
	southbridge_smm_clear_state();

	/* Run the relocation handler. */
	smm_initiate_relocation();

	/* If smm_save_state_in_msrs is non-zero then parallel SMM relocation
	 * shall take place. Run the relocation handler a second time to do
	 * the final move. */
	if (smm_reloc_params.smm_save_state_in_msrs) {
		printk(BIOS_DEBUG, "Doing parallel SMM relocation.\n");
		release_aps_for_smm_relocation(1);
		smm_initiate_relocation_parallel();
	} else {
		release_aps_for_smm_relocation(0);
	}

	/* Now that all APs have been relocated as well as the BSP let SMIs
	 * start flowing. */
	southbridge_smm_enable_smi();

	/* Lock down the SMRAM space. */
	smm_lock();

	return 0;
}

void smm_init(void)
{
	/* smm_init() is normally called from initialize_cpus() in
	 * lapic_cpu_init.c. However, that path is no longer used. Don't reuse
	 * the function name because that would cause confusion.
	 * The smm_initialize() function above is used to setup SMM at the
	 * appropriate time. */
}

void smm_lock(void)
{
	/* LOCK the SMM memory window and enable normal SMM.
	 * After running this function, only a full reset can
	 * make the SMM registers writable again.
	 */
	printk(BIOS_DEBUG, "Locking SMM.\n");
	pci_write_config8(dev_find_slot(0, PCI_DEVFN(0, 0)), SMRAM,
			D_LCK | G_SMRAME | C_BASE_SEG);
}

