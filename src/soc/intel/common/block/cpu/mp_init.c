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
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mp.h>
#include <cpu/intel/microcode.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/msr.h>
#include <soc/cpu.h>

static const void *microcode_patch;

/* SoC override function */
__attribute__((weak)) void soc_core_init(device_t dev, const void *microcode)
{
	/* no-op */
}

__attribute__((weak)) void soc_init_cpus(struct bus *cpu_bus,
				const void *microcode)
{
	/* no-op */
}

static  void init_one_cpu(device_t dev)
{
	soc_core_init(dev, microcode_patch);
	intel_microcode_load_unlocked(microcode_patch);
}

static struct device_operations cpu_dev_ops = {
	.init = init_one_cpu,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_C0 },
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_D0 },
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_HQ0 },
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_HR0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_G0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_H0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_Y0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_HA0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_HB0 },
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
 * MP Init callback function(get_microcode_info) to find the Microcode at
 * Pre MP Init phase. This function is common among all SOCs and thus its in
 * Common CPU block.
 * This function also fills in the microcode patch (in *microcode), and also
 * sets the argument *parallel to 1, which allows microcode loading in all
 * APs to occur in parallel during MP Init.
 */
void get_microcode_info(const void **microcode, int *parallel)
{
	*microcode =microcode_patch;
	*parallel = 1;
}

static void init_cpus(void *unused)
{
	device_t dev = dev_find_path(NULL, DEVICE_PATH_CPU_CLUSTER);
	assert(dev != NULL);

	microcode_patch = intel_microcode_find();
	intel_microcode_load_unlocked(microcode_patch);

	/*
	 * TODO: This parameter "microcode_patch" should be removed
	 * in this function call once the following two cases are resolved -
	 *
	 * 1) SGX enabling for the BSP issue gets solved, due to which
	 * configure_sgx() function is kept inside soc/cpu.c soc_init_cpus().
	 * 2) uCode loading after SMM relocation is deleted inside
	 * per_cpu_smm_trigger() mp_ops callback function in soc/cpu.c,
	 * since as per current BWG, uCode loading can be done after
	 * all feature programmings are done. There is no specific
	 * recommendation to do it after SMM Relocation.
	 */
	soc_init_cpus(dev->link_list, microcode_patch);
}

/* Ensure to re-program all MTRRs based on DRAM resource settings */
static void post_cpus_init(void *unused)
{
	if (mp_run_on_all_cpus(&x86_setup_mtrrs_with_detect, 1000) < 0)
		printk(BIOS_ERR, "MTRR programming failure\n");

	/* Temporarily cache the memory-mapped boot media. */
	if (IS_ENABLED(CONFIG_BOOT_DEVICE_MEMORY_MAPPED) &&
		IS_ENABLED(CONFIG_BOOT_DEVICE_SPI_FLASH))
		fast_spi_cache_bios_region();

	x86_mtrr_check();
}

/* Do CPU MP Init before FSP Silicon Init */
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT_CHIPS, BS_ON_ENTRY, init_cpus, NULL);
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT, post_cpus_init, NULL);
