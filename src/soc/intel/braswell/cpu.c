/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <fsp/memmap.h>
#include <reg_script.h>
#include <soc/iosf.h>
#include <soc/msr.h>
#include <soc/pattrs.h>
#include <soc/ramstage.h>
#include <soc/smm.h>
#include <stdlib.h>

/* Core level MSRs */
static const struct reg_script core_msr_script[] = {
	/* Dynamic L2 shrink enable and threshold, clear SINGLE_PCTL bit 11 */
	REG_MSR_RMW(MSR_PMG_CST_CONFIG_CONTROL, ~0x3f080f, 0xe0008),
	REG_MSR_RMW(MSR_POWER_MISC,
		    ~(ENABLE_ULFM_AUTOCM_MASK | ENABLE_INDP_AUTOCM_MASK), 0),
	/* Disable C1E */
	REG_MSR_RMW(MSR_POWER_CTL, ~0x2, 0),
	REG_MSR_OR(MSR_POWER_MISC, 0x44),
	REG_SCRIPT_END
};

static void soc_core_init(struct device *cpu)
{
	printk(BIOS_SPEW, "%s/%s ( %s )\n",
			__FILE__, __func__, dev_name(cpu));
	printk(BIOS_DEBUG, "Init Braswell core.\n");

	/*
	 * The turbo disable bit is actually scoped at building
	 * block level -- not package. For non-bsp cores that are within a
	 * building block enable turbo. The cores within the BSP's building
	 * block will just see it already enabled and move on.
	 */
	if (lapicid())
		enable_turbo();

	/* Set core MSRs */
	reg_script_run(core_msr_script);

	/* Set this core to max frequency ratio */
	set_max_freq();
}

static struct device_operations cpu_dev_ops = {
	.init = soc_core_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x406C4 },
	{ X86_VENDOR_INTEL, 0x406C3 },
	{ X86_VENDOR_INTEL, 0x406C2 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};


/*
 * MP and SMM loading initialization.
 */

struct smm_relocation_attrs {
	uint32_t smbase;
	uint32_t smrr_base;
	uint32_t smrr_mask;
};

static struct smm_relocation_attrs relo_attrs;

/* Package level MSRs */
static const struct reg_script package_msr_script[] = {
	/* Set Package TDP to ~7W */
	REG_MSR_WRITE(MSR_PKG_POWER_LIMIT, 0x3880fa),
	REG_MSR_RMW(MSR_PP1_POWER_LIMIT, ~(0x7f << 17), 0),
	REG_MSR_WRITE(MSR_PKG_TURBO_CFG1, 0x702),
	REG_MSR_WRITE(MSR_CPU_TURBO_WKLD_CFG1, 0x200b),
	REG_MSR_WRITE(MSR_CPU_TURBO_WKLD_CFG2, 0),
	REG_MSR_WRITE(MSR_CPU_THERM_CFG1, 0x00000305),
	REG_MSR_WRITE(MSR_CPU_THERM_CFG2, 0x0405500d),
	REG_MSR_WRITE(MSR_CPU_THERM_SENS_CFG, 0x27),
	REG_SCRIPT_END
};

static void pre_mp_init(void)
{
	uint32_t bsmrwac;

	/* Set up MTRRs based on physical address size. */
	x86_setup_mtrrs_with_detect();
	x86_mtrr_check();

	/*
	 * Configure the BUNIT to allow dirty cache line evictions in non-SMM
	 * mode for the lines that were dirtied while in SMM mode. Otherwise
	 * the writes would be silently dropped.
	 */
	bsmrwac = iosf_bunit_read(BUNIT_SMRWAC) | SAI_IA_UNTRUSTED;
	iosf_bunit_write(BUNIT_SMRWAC, bsmrwac);

	/* Set package MSRs */
	reg_script_run(package_msr_script);

	/* Enable Turbo Mode on BSP and siblings of the BSP's building block. */
	enable_turbo();
}

static int get_cpu_count(void)
{
	const struct pattrs *pattrs = pattrs_get();

	return pattrs->num_cpus;
}

static void get_smm_info(uintptr_t *perm_smbase, size_t *perm_smsize,
				size_t *smm_save_state_size)
{
	void *smm_base;
	size_t smm_size;

	/* All range registers are aligned to 4KiB */
	const uint32_t rmask = ~((1 << 12) - 1);

	/* Initialize global tracking state. */
	smm_region(&smm_base, &smm_size);
	relo_attrs.smbase = (uint32_t)smm_base;
	relo_attrs.smrr_base = relo_attrs.smbase | MTRR_TYPE_WRBACK;
	relo_attrs.smrr_mask = ~(smm_size - 1) & rmask;
	relo_attrs.smrr_mask |= MTRR_PHYS_MASK_VALID;

	*perm_smbase = relo_attrs.smbase;
	*perm_smsize = smm_size - CONFIG_SMM_RESERVED_SIZE;
	*smm_save_state_size = sizeof(em64t100_smm_state_save_area_t);
}

static void get_microcode_info(const void **microcode, int *parallel)
{
	const struct pattrs *pattrs = pattrs_get();

	*microcode = pattrs->microcode_patch;
	*parallel = 1;
}

static void per_cpu_smm_trigger(void)
{
	const struct pattrs *pattrs = pattrs_get();
	msr_t msr_value;

	/* Need to make sure that all cores have microcode loaded. */
	msr_value = rdmsr(MSR_IA32_BIOS_SIGN_ID);
	if (msr_value.hi == 0)
		intel_microcode_load_unlocked(pattrs->microcode_patch);

	/* Relocate SMM space. */
	smm_initiate_relocation();

	/* Load microcode after SMM relocation. */
	intel_microcode_load_unlocked(pattrs->microcode_patch);
}

static void relocation_handler(int cpu, uintptr_t curr_smbase,
				uintptr_t staggered_smbase)
{
	msr_t smrr;
	em64t100_smm_state_save_area_t *smm_state;

	/* Set up SMRR. */
	smrr.lo = relo_attrs.smrr_base;
	smrr.hi = 0;
	wrmsr(SMRR_PHYS_BASE, smrr);
	smrr.lo = relo_attrs.smrr_mask;
	smrr.hi = 0;
	wrmsr(SMRR_PHYS_MASK, smrr);

	smm_state = (void *)(SMM_EM64T100_SAVE_STATE_OFFSET + curr_smbase);
	smm_state->smbase = staggered_smbase;
}

static const struct mp_ops mp_ops = {
	.pre_mp_init = pre_mp_init,
	.get_cpu_count = get_cpu_count,
	.get_smm_info = get_smm_info,
	.get_microcode_info = get_microcode_info,
	.pre_mp_smm_init = southcluster_smm_clear_state,
	.per_cpu_smm_trigger = per_cpu_smm_trigger,
	.relocation_handler = relocation_handler,
	.post_mp_init = southcluster_smm_enable_smi,
};

void soc_init_cpus(struct device *dev)
{
	struct bus *cpu_bus = dev->link_list;

	printk(BIOS_SPEW, "%s/%s ( %s )\n",
			__FILE__, __func__, dev_name(dev));

	if (mp_init_with_smm(cpu_bus, &mp_ops))
		printk(BIOS_ERR, "MP initialization failure.\n");
}
