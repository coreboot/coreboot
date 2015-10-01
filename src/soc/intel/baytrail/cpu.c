/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#include <stdlib.h>
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
#include <reg_script.h>

#include <soc/iosf.h>
#include <soc/msr.h>
#include <soc/pattrs.h>
#include <soc/ramstage.h>
#include <soc/smm.h>

static void smm_relocate(void *unused);
static void enable_smis(void *unused);

static struct mp_flight_record mp_steps[] = {
	MP_FR_BLOCK_APS(smm_relocate, NULL, smm_relocate, NULL),
	MP_FR_BLOCK_APS(mp_initialize_cpu, NULL, mp_initialize_cpu, NULL),
	/* Wait for APs to finish initialization before proceeding. */
	MP_FR_BLOCK_APS(NULL, NULL, enable_smis, NULL),
};

/* The APIC id space on Bay Trail is sparse. Each id is separated by 2. */
static int adjust_apic_id(int index, int apic_id)
{
	return 2 * index;
}

/* Package level MSRs */
const struct reg_script package_msr_script[] = {
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

/* Core level MSRs */
const struct reg_script core_msr_script[] = {
	/* Dynamic L2 shrink enable and threshold, clear SINGLE_PCTL bit 11 */
	REG_MSR_RMW(MSR_PMG_CST_CONFIG_CONTROL, ~0x3f080f, 0xe0008),
	REG_MSR_RMW(MSR_POWER_MISC,
		    ~(ENABLE_ULFM_AUTOCM_MASK | ENABLE_INDP_AUTOCM_MASK), 0),
	/* Disable C1E */
	REG_MSR_RMW(MSR_POWER_CTL, ~0x2, 0),
	REG_MSR_OR(MSR_POWER_MISC, 0x44),
	REG_SCRIPT_END
};

void baytrail_init_cpus(device_t dev)
{
	struct bus *cpu_bus = dev->link_list;
	const struct pattrs *pattrs = pattrs_get();
	struct mp_params mp_params;
	uint32_t bsmrwac;
	void *default_smm_area;

	/* Set up MTRRs based on physical address size. */
	x86_setup_fixed_mtrrs();
	x86_setup_var_mtrrs(pattrs->address_bits, 2);
	x86_mtrr_check();

	mp_params.num_cpus = pattrs->num_cpus,
	mp_params.parallel_microcode_load = 1,
	mp_params.adjust_apic_id = adjust_apic_id;
	mp_params.flight_plan = &mp_steps[0];
	mp_params.num_records = ARRAY_SIZE(mp_steps);
	mp_params.microcode_pointer = pattrs->microcode_patch;

	default_smm_area = backup_default_smm_area();

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

	if (mp_init(cpu_bus, &mp_params)) {
		printk(BIOS_ERR, "MP initialization failure.\n");
	}

	restore_default_smm_area(default_smm_area);
}

static void baytrail_core_init(device_t cpu)
{
	printk(BIOS_DEBUG, "Init BayTrail core.\n");

	/* On bay trail the turbo disable bit is actually scoped at building
	 * block level -- not package. For non-bsp cores that are within a
	 * building block enable turbo. The cores within the BSP's building
	 * block will just see it already enabled and move on. */
	if (lapicid())
		enable_turbo();

	/* Set core MSRs */
	reg_script_run(core_msr_script);

	/* Set this core to max frequency ratio */
	set_max_freq();
}

static struct device_operations cpu_dev_ops = {
	.init = baytrail_core_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x30673 },
	{ X86_VENDOR_INTEL, 0x30678 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};


/*
 * SMM loading and initialization.
 */

struct smm_relocation_attrs {
	uint32_t smbase;
	uint32_t smrr_base;
	uint32_t smrr_mask;
};

static struct smm_relocation_attrs relo_attrs;

static void adjust_apic_id_map(struct smm_loader_params *smm_params)
{
	int i;
	struct smm_runtime *runtime = smm_params->runtime;

	for (i = 0; i < CONFIG_MAX_CPUS; i++)
		runtime->apic_id_to_cpu[i] = mp_get_apic_id(i);
}

static void asmlinkage cpu_smm_do_relocation(void *arg)
{
	msr_t smrr;
	em64t100_smm_state_save_area_t *smm_state;
	const struct smm_module_params *p;
	const struct smm_runtime *runtime;
	int cpu;

	p = arg;
	runtime = p->runtime;
	cpu = p->cpu;

	if (cpu >= CONFIG_MAX_CPUS) {
		printk(BIOS_CRIT,
		       "Invalid CPU number assigned in SMM stub: %d\n", cpu);
		return;
	}

	/* Set up SMRR. */
	smrr.lo = relo_attrs.smrr_base;
	smrr.hi = 0;
	wrmsr(SMRR_PHYS_BASE, smrr);
	smrr.lo = relo_attrs.smrr_mask;
	smrr.hi = 0;
	wrmsr(SMRR_PHYS_MASK, smrr);

	/* The relocated handler runs with all CPUs concurrently. Therefore
	 * stagger the entry points adjusting SMBASE downwards by save state
	 * size * CPU num. */
	smm_state = (void *)(SMM_EM64T100_SAVE_STATE_OFFSET + runtime->smbase);
	smm_state->smbase = relo_attrs.smbase - cpu * runtime->save_state_size;
	printk(BIOS_DEBUG, "New SMBASE 0x%08x\n", smm_state->smbase);
}

static int install_relocation_handler(int num_cpus)
{
	const int save_state_size = sizeof(em64t100_smm_state_save_area_t);

	struct smm_loader_params smm_params = {
		.per_cpu_stack_size = save_state_size,
		.num_concurrent_stacks = num_cpus,
		.per_cpu_save_state_size = save_state_size,
		.num_concurrent_save_states = 1,
		.handler = (smm_handler_t)&cpu_smm_do_relocation,
	};

	if (smm_setup_relocation_handler(&smm_params))
		return -1;

	adjust_apic_id_map(&smm_params);

	return 0;
}

static int install_permanent_handler(int num_cpus)
{
	/* There are num_cpus concurrent stacks and num_cpus concurrent save
	 * state areas. Lastly, set the stack size to the save state size. */
	int save_state_size = sizeof(em64t100_smm_state_save_area_t);
	struct smm_loader_params smm_params = {
		.per_cpu_stack_size = save_state_size,
		.num_concurrent_stacks = num_cpus,
		.per_cpu_save_state_size = save_state_size,
		.num_concurrent_save_states = num_cpus,
	};
	const int tseg_size = smm_region_size() - CONFIG_SMM_RESERVED_SIZE;

	printk(BIOS_DEBUG, "Installing SMM handler to 0x%08x\n",
	       relo_attrs.smbase);

	if (smm_load_module((void *)relo_attrs.smbase, tseg_size, &smm_params))
		return -1;

	adjust_apic_id_map(&smm_params);

	return 0;
}

static int smm_load_handlers(void)
{
	/* All range registers are aligned to 4KiB */
	const uint32_t rmask = ~((1 << 12) - 1);
	const struct pattrs *pattrs = pattrs_get();

	/* Initialize global tracking state. */
	relo_attrs.smbase = (uint32_t)smm_region_start();
	relo_attrs.smrr_base = relo_attrs.smbase | MTRR_TYPE_WRBACK;
	relo_attrs.smrr_mask = ~(smm_region_size() - 1) & rmask;
	relo_attrs.smrr_mask |= MTRR_PHYS_MASK_VALID;

	/* Install handlers. */
	if (install_relocation_handler(pattrs->num_cpus) < 0) {
		printk(BIOS_ERR, "Unable to install SMM relocation handler.\n");
		return -1;
	}

	if (install_permanent_handler(pattrs->num_cpus) < 0) {
		printk(BIOS_ERR, "Unable to install SMM permanent handler.\n");
		return -1;
	}

	/* Ensure the SMM handlers hit DRAM before performing first SMI. */
	wbinvd();

	return 0;
}

static void smm_relocate(void *unused)
{
	const struct pattrs *pattrs = pattrs_get();

	/* Load relocation and permanent handler. */
	if (boot_cpu()) {
		if (smm_load_handlers() < 0) {
			printk(BIOS_ERR, "Error loading SMM handlers.\n");
			return;
		}
		southcluster_smm_clear_state();
	}

	/* Relocate SMM space. */
	smm_initiate_relocation();

	/* Load microcode after SMM relocation. */
	intel_microcode_load_unlocked(pattrs->microcode_patch);
}

static void enable_smis(void *unused)
{
	southcluster_smm_enable_smi();
}
