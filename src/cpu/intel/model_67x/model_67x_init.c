/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2010 Keith Hui <buurin@gmail.com>
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

#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/intel/l2_cache.h>

static void model_67x_init(struct device *cpu)
{
	/* Update the microcode */
	intel_update_microcode_from_cbfs();

	/* Initialize L2 cache */
	p6_configure_l2_cache();

	/* Turn on caching if we haven't already */
	x86_enable_cache();

	/* Setup MTRRs */
	x86_setup_mtrrs();
	x86_mtrr_check();

	/* Enable the local CPU APICs */
	setup_lapic();
}

static struct device_operations cpu_dev_ops = {
	.init     = model_67x_init,
};

/*
 * Intel Pentium III Processor Identification and Package Information
 * http://www.intel.com/design/pentiumiii/qit/update.pdf
 *
 * Intel Pentium III Processor Specification Update
 * http://download.intel.com/design/intarch/specupdt/24445358.pdf
 */
static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x0671 },
	{ X86_VENDOR_INTEL, 0x0672 }, /* PIII, kB0 */
	{ X86_VENDOR_INTEL, 0x0673 }, /* PIII, kC0 */

	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
