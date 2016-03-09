/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

static void configure_mca(void);

static struct mp_flight_record mp_steps[] = {
	MP_FR_BLOCK_APS(mp_initialize_cpu, NULL, mp_initialize_cpu, NULL),
};

static int adjust_apic_id(int index, int apic_id)
{
	return index;
}

static void configure_mca(void)
{
	msr_t msr;
	const unsigned int mcg_cap_msr = 0x179;
	int i;
	int num_banks;

	msr = rdmsr(mcg_cap_msr);
	num_banks = msr.lo & 0xff;

	/* TODO(adurbin): This should only be done on a cold boot. Also, some
	 * of these banks are core vs package scope. For now every CPU clears
	 * every bank. */
	msr.lo = msr.hi = 0;
	for (i = 0; i < num_banks; i++) {
		wrmsr(MSR_IA32_MC0_STATUS + (i * 4) + 1, msr);
		wrmsr(MSR_IA32_MC0_STATUS + (i * 4) + 2, msr);
		wrmsr(MSR_IA32_MC0_STATUS + (i * 4) + 3, msr);
	}

	msr.lo = msr.hi = 0xffffffff;
	for (i = 0; i < num_banks; i++)
		wrmsr(MSR_IA32_MC0_STATUS + (i * 4), msr);
}

void broadwell_de_init_cpus(device_t dev)
{
	struct bus *cpu_bus = dev->link_list;
	const struct pattrs *pattrs = pattrs_get();
	struct mp_params mp_params;

	x86_mtrr_check();

	/* Enable the local cpu apics */
	setup_lapic();

	mp_params.num_cpus = pattrs->num_cpus,
	mp_params.parallel_microcode_load = 1,
	mp_params.adjust_apic_id = adjust_apic_id;
	mp_params.flight_plan = &mp_steps[0];
	mp_params.num_records = ARRAY_SIZE(mp_steps);
	mp_params.microcode_pointer = pattrs->microcode_patch;

	if (mp_init(cpu_bus, &mp_params)) {
		printk(BIOS_ERR, "MP initialization failure.\n");
	}
}

static void broadwell_de_core_init(device_t cpu)
{
	printk(BIOS_DEBUG, "Init Broadwell-DE core.\n");
	configure_mca();
}

static struct device_operations cpu_dev_ops = {
	.init = broadwell_de_core_init,
};

static struct cpu_device_id cpu_table[] = {
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
