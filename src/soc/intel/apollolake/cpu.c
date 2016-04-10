/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corp.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/cpu.h>

static struct device_operations cpu_dev_ops = {
	.init = DEVICE_NOOP,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, CPUID_APOLLOLAKE_A0 },
	{ X86_VENDOR_INTEL, CPUID_APOLLOLAKE_B0 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

static void read_cpu_topology(unsigned int *num_phys, unsigned int *num_virt)
{
	msr_t msr;
	msr = rdmsr(MSR_CORE_THREAD_COUNT);
	*num_virt = (msr.lo >> 0) & 0xffff;
	*num_phys = (msr.lo >> 16) & 0xffff;
}

/*
 * Do essential initialization tasks before APs can be fired up
 *
 * 1. Prevent race condition in MTRR solution. Enable MTRRs on the BSP. This
 * creates the MTRR solution that the APs will use. Otherwise APs will try to
 * apply the incomplete solution as the BSP is calculating it.
 */
static void bsp_pre_mp_setup(void)
{
	x86_setup_mtrrs_with_detect();
	x86_mtrr_check();
}

/*
 * CPU initialization recipe
 *
 * Note that no microcode update is passed to the init function. CSE updates
 * the microcode on all cores before releasing them from reset. That means that
 * the BSP and all APs will come up with the same microcode revision.
 */
static struct mp_flight_record flight_plan[] = {
	/* NOTE: MTRR solution must be calculated before firing up the APs */
	MP_FR_NOBLOCK_APS(mp_initialize_cpu, NULL, mp_initialize_cpu, NULL),
};

void apollolake_init_cpus(device_t dev)
{
	unsigned int num_virt_cores, num_phys_cores;

	/* Pre-flight check */
	bsp_pre_mp_setup();

	/* Find CPU topology */
	read_cpu_topology(&num_phys_cores, &num_virt_cores);
	printk(BIOS_DEBUG, "Detected %u core, %u thread CPU.\n",
	       num_phys_cores, num_virt_cores);

	/* Systems check */
	struct mp_params flight_data_recorder = {
		.num_cpus = num_virt_cores,
		.parallel_microcode_load = 0,
		.microcode_pointer = NULL,
		.adjust_apic_id = NULL,
		.flight_plan = flight_plan,
		.num_records = ARRAY_SIZE(flight_plan),
	};

	/* Clear for take-off */
	if (mp_init(dev->link_list, &flight_data_recorder) < 0)
		printk(BIOS_ERR, "MP initialization failure.\n");
}
