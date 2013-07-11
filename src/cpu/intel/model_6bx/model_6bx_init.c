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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/name.h>

static const uint32_t microcode_updates[] = {
	#include "microcode-737-MU16b11c.h"
	#include "microcode-738-MU16b11d.h"
	#include "microcode-875-MU16b401.h"
	#include "microcode-885-MU16b402.h"
	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};

static void model_6bx_init(device_t cpu)
{
	char processor_name[49];

	/* Turn on caching if we haven't already */
	x86_enable_cache();

	/* Update the microcode */
	intel_update_microcode(microcode_updates);

	/* Print processor name */
	fill_processor_name(processor_name);
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);

	/* Setup MTRRs */
	x86_setup_mtrrs();
	x86_mtrr_check();

	/* Enable the local cpu apics */
	setup_lapic();
}

static struct device_operations cpu_dev_ops = {
	.init     = model_6bx_init,
};

/*
 * Pentium III Processor Identification and Package Information.
 * http://www.intel.com/design/pentiumiii/qit/update.pdf
 *
 * Intel Pentium III Processor Specification Update
 * http://download.intel.com/design/intarch/specupdt/24445358.pdf
 */
static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x06b1 }, /* Pentium III/Celeron, tA1/A1/FPA1 */
	{ X86_VENDOR_INTEL, 0x06b4 }, /* Pentium III, tB1/FPB1 */
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

