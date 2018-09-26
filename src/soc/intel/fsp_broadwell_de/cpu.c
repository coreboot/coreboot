/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
 * Copyright (C) 2017 Siemens AG
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

#include <stdlib.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <soc/msr.h>
#include <soc/pattrs.h>
#include <soc/ramstage.h>
#include <soc/smm.h>

/* MP initialization support. */
static const void *microcode_patch;

static void pre_mp_init(void)
{
	x86_mtrr_check();

	/* Enable the local CPU apics */
	setup_lapic();
}

static int get_cpu_count(void)
{
	const struct pattrs *pattrs = pattrs_get();

	return pattrs->num_cpus;
}

static void per_cpu_smm_trigger(void)
{
	/* Relocate the SMM handler. */
	smm_relocate();

	/* After SMM relocation a 2nd microcode load is required. */
	intel_microcode_load_unlocked(microcode_patch);
}

static void get_microcode_info(const void **microcode, int *parallel)
{
	const struct pattrs *pattrs = pattrs_get();

	microcode_patch = pattrs->microcode_patch;
	*microcode = pattrs->microcode_patch;
	*parallel = 1;
}

static int cpu_config_tdp_levels(void)
{
	msr_t platform_info;

	/* Bits 34:33 indicate how many levels are supported. */
	platform_info = rdmsr(MSR_PLATFORM_INFO);
	return (platform_info.hi >> 1) & 3;
}

static void set_max_ratio(void)
{
	msr_t msr, perf_ctl;

	perf_ctl.hi = 0;

	/* Check for configurable TDP option. */
	if (cpu_config_tdp_levels()) {
		/* Set to nominal TDP ratio. */
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		perf_ctl.lo = (msr.lo & 0xff) << 8;
	} else {
		/* Platform Info Bits 15:8 give max ratio. */
		msr = rdmsr(MSR_PLATFORM_INFO);
		perf_ctl.lo = msr.lo & 0xff00;
	}
	wrmsr(IA32_PERF_CTL, perf_ctl);
}

static void post_mp_init(void)
{
	/* Set Max Ratio */
	set_max_ratio();
	/* Now that all APs have been relocated as well as the BSP let SMIs
	   start flowing. */
	southbridge_smm_enable_smi();

	/* Set SMI lock bits. */
	smm_lock();
}

static const struct mp_ops mp_ops = {
	.pre_mp_init = pre_mp_init,
	.get_smm_info = smm_info,
	.get_cpu_count = get_cpu_count,
	.get_microcode_info = get_microcode_info,
	.pre_mp_smm_init = smm_initialize,
	.per_cpu_smm_trigger = per_cpu_smm_trigger,
	.relocation_handler = smm_relocation_handler,
	.post_mp_init = post_mp_init
};

void broadwell_de_init_cpus(struct device *dev)
{
	struct bus *cpu_bus = dev->link_list;

	if (mp_init_with_smm(cpu_bus, &mp_ops)) {
		printk(BIOS_ERR, "MP initialization failure.\n");
	}
}

static void configure_mca(void)
{
	msr_t msr;
	int i;
	int num_banks;

	msr = rdmsr(IA32_MCG_CAP);
	num_banks = msr.lo & 0xff;

	/* TODO(adurbin): This should only be done on a cold boot. Also, some
	   of these banks are core vs package scope. For now every CPU clears
	   every bank. */
	msr.lo = msr.hi = 0;
	for (i = 0; i < num_banks; i++) {
		wrmsr(IA32_MC0_STATUS + (i * 4), msr);
		wrmsr(IA32_MC0_STATUS + (i * 4) + 1, msr);
		wrmsr(IA32_MC0_STATUS + (i * 4) + 2, msr);
	}

	msr.lo = msr.hi = 0xffffffff;
	for (i = 0; i < num_banks; i++)
		wrmsr(IA32_MC0_CTL + (i * 4), msr);
}

static void broadwell_de_core_init(struct device *cpu)
{
	printk(BIOS_DEBUG, "Init Broadwell-DE core.\n");
	configure_mca();
}

static struct device_operations cpu_dev_ops = {
	.init = broadwell_de_core_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x50661 },
	{ X86_VENDOR_INTEL, 0x50662 },
	{ X86_VENDOR_INTEL, 0x50663 },
	{ X86_VENDOR_INTEL, 0x50664 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
