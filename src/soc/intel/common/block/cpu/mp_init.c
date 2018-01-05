/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
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

#include <arch/io.h>
#include <assert.h>
#include <bootstate.h>
#include <compiler.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mp.h>
#include <cpu/intel/microcode.h>
#include <intelblocks/chip.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/msr.h>
#include <soc/cpu.h>

static const void *microcode_patch;

/* SoC override function */
__weak void soc_core_init(struct device *dev)
{
	/* no-op */
}

__weak void soc_init_cpus(struct bus *cpu_bus)
{
	/* no-op */
}

static void init_one_cpu(struct device *dev)
{
	soc_core_init(dev);
	intel_microcode_load_unlocked(microcode_patch);
}

static struct device_operations cpu_dev_ops = {
	.init = init_one_cpu,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_C0 },
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_D0 },
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_HQ0 },
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_HR0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_G0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_H0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_Y0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_HA0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_HB0 },
	{ X86_VENDOR_INTEL, CPUID_CANNONLAKE_A0 },
	{ X86_VENDOR_INTEL, CPUID_CANNONLAKE_B0 },
	{ X86_VENDOR_INTEL, CPUID_CANNONLAKE_C0 },
	{ X86_VENDOR_INTEL, CPUID_CANNONLAKE_D0 },
	{ X86_VENDOR_INTEL, CPUID_APOLLOLAKE_A0 },
	{ X86_VENDOR_INTEL, CPUID_APOLLOLAKE_B0 },
	{ X86_VENDOR_INTEL, CPUID_APOLLOLAKE_E0 },
	{ X86_VENDOR_INTEL, CPUID_GLK_A0 },
	{ X86_VENDOR_INTEL, CPUID_GLK_B0 },
	{ X86_VENDOR_INTEL, CPUID_WHISKEYLAKE_W0 },
	{ X86_VENDOR_INTEL, CPUID_COFFEELAKE_U0 },
	{ X86_VENDOR_INTEL, CPUID_COFFEELAKE_D0 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

/*
 * MP Init callback function to Find CPU Topology. This function is common
 * among all SOCs and thus its in Common CPU block.
 */
int get_cpu_count(void)
{
	unsigned int num_virt_cores, num_phys_cores;

	cpu_read_topology(&num_phys_cores, &num_virt_cores);

	printk(BIOS_DEBUG, "Detected %u core, %u thread CPU.\n",
	       num_phys_cores, num_virt_cores);

	return num_virt_cores;
}

/*
 * Function to get the microcode patch pointer. Use this function to avoid
 * reading the microcode patch from the boot media. init_cpus() would
 * initialize microcode_patch global variable to point to microcode patch
 * in boot media and this function can be used to access the pointer.
 */
const void *intel_mp_current_microcode(void)
{
	return microcode_patch;
}

/*
 * MP Init callback function(get_microcode_info) to find the Microcode at
 * Pre MP Init phase. This function is common among all SOCs and thus its in
 * Common CPU block.
 * This function also fills in the microcode patch (in *microcode), and also
 * sets the argument *parallel to 1, which allows microcode loading in all
 * APs to occur in parallel during MP Init.
 */
void get_microcode_info(const void **microcode, int *parallel)
{
	*microcode = intel_mp_current_microcode();
	*parallel = 1;
}

static void init_cpus(void *unused)
{
	struct device *dev = dev_find_path(NULL, DEVICE_PATH_CPU_CLUSTER);
	assert(dev != NULL);

	if (chip_get_fsp_mp_init())
		return;

	microcode_patch = intel_microcode_find();
	intel_microcode_load_unlocked(microcode_patch);

	soc_init_cpus(dev->link_list);
}

static void wrapper_x86_setup_mtrrs(void *unused)
{
	x86_setup_mtrrs_with_detect();
}

/* Ensure to re-program all MTRRs based on DRAM resource settings */
static void post_cpus_init(void *unused)
{
	if (chip_get_fsp_mp_init())
		return;

	if (mp_run_on_all_cpus(&wrapper_x86_setup_mtrrs, NULL, 1000) < 0)
		printk(BIOS_ERR, "MTRR programming failure\n");

	x86_mtrr_check();
}

/* Do CPU MP Init before FSP Silicon Init */
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT_CHIPS, BS_ON_ENTRY, init_cpus, NULL);
BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_EXIT, post_cpus_init, NULL);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME, BS_ON_ENTRY, post_cpus_init, NULL);
