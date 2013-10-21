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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/mp.h>

#include <baytrail/pattrs.h>
#include <baytrail/ramstage.h>

static struct mp_flight_record mp_steps[] = {
	MP_FR_BLOCK_APS(mp_initialize_cpu, NULL, mp_initialize_cpu, NULL),
};

/* The APIC id space on Bay Trail is sparse. Each id is separated by 2. */
static int adjust_apic_id(int index, int apic_id)
{
	return 2 * index;
}

void baytrail_init_cpus(device_t dev)
{
	struct bus *cpu_bus = dev->link_list;
	const struct pattrs *pattrs = pattrs_get();
	struct mp_params mp_params;

	/* Set up MTRRs based on physical address size. */
	x86_setup_fixed_mtrrs();
	x86_setup_var_mtrrs(pattrs->address_bits, 2);
	x86_mtrr_check();

	mp_params.num_cpus = pattrs->num_cpus,
	mp_params.parallel_microcode_load = 1,
	mp_params.adjust_apic_id = adjust_apic_id;
	mp_params.flight_plan = &mp_steps[0];
	mp_params.num_records = ARRAY_SIZE(mp_steps);
	mp_params.microcode_pointer = intel_microcode_find();
	mp_params.microcode_pointer = NULL;

	if (mp_init(cpu_bus, &mp_params)) {
		printk(BIOS_ERR, "MP initialization failure.\n");
	}
}

static void baytrail_core_init(device_t cpu)
{
	printk(BIOS_DEBUG, "Init BayTrail core.\n");
}

static struct device_operations cpu_dev_ops = {
	.init = baytrail_core_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x30673 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

