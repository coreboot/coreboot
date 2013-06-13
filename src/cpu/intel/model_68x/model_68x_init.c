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
#include <usbdebug.h>

static const uint32_t microcode_updates[] = {
	#include "microcode-534-MU16810d.h"
	#include "microcode-535-MU16810e.h"
	#include "microcode-536-MU16810f.h"
	#include "microcode-537-MU268110.h"
	#include "microcode-538-MU168111.h"
	#include "microcode-550-MU168307.h"
	#include "microcode-551-MU168308.h"
	#include "microcode-727-MU168313.h"
	#include "microcode-728-MU168314.h"
	#include "microcode-729-MU268310.h"
	#include "microcode-611-MU168607.h"
	#include "microcode-612-MU168608.h"
	#include "microcode-615-MU16860a.h"
	#include "microcode-617-MU16860c.h"
	#include "microcode-618-MU268602.h"
	#include "microcode-662-MU168a01.h"
	#include "microcode-691-MU168a04.h"
	#include "microcode-692-MU168a05.h"
	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};

static void model_68x_init(device_t cpu)
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
static struct cpu_device_id cpu_table[] = {
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

