/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2010 Joseph Smith <joe@settoplinux.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/name.h>

static void model_68x_init(struct device *cpu)
{
	char processor_name[49];

	/* Turn on caching if we haven't already */
	x86_enable_cache();

	/* Update the microcode */
	intel_update_microcode_from_cbfs();

	/* Print processor name */
	fill_processor_name(processor_name);
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);

	/* Setup MTRRs */
	x86_setup_mtrrs();
	x86_mtrr_check();

	/* Enable the local CPU APICs */
	setup_lapic();
}

static struct device_operations cpu_dev_ops = {
	.init     = model_68x_init,
};

/*
 * Intel Celeron Processor Identification Information
 * http://www.intel.com/design/celeron/qit/update.pdf
 *
 * Intel Pentium III Processor Identification and Package Information
 * http://www.intel.com/design/pentiumiii/qit/update.pdf
 *
 * Intel Pentium III Processor Specification Update
 * http://download.intel.com/design/intarch/specupdt/24445358.pdf
 *
 * Mobile Intel Pentium III/III-M Processor Specification Update
 * http://download.intel.com/design/intarch/specupdt/24530663.pdf
 */
static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x0680 },
	{ X86_VENDOR_INTEL, 0x0681 }, /* PIII, cA2/cA2c/A2/BA2/PA2/MA2 */
	{ X86_VENDOR_INTEL, 0x0683 }, /* PIII/Celeron, cB0/cB0c/B0/BB0/PB0/MB0*/
	{ X86_VENDOR_INTEL, 0x0686 }, /* PIII/Celeron, cC0/C0/BC0/PC0/MC0 */
	{ X86_VENDOR_INTEL, 0x068a }, /* PIII/Celeron, cD0/D0/BD0/PD0 */

	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
