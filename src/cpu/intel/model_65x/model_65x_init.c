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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/intel/l2_cache.h>

static u32 microcode_updates[] = {
	#include "microcode-410-MU16522d.h"
	#include "microcode-422-MU26530b.h"
	#include "microcode-412-MU16530d.h"
	#include "microcode-423-MU26522b.h"
	#include "microcode-407-MU16522a.h"
	#include "microcode-146-MU16502e.h"
	#include "microcode-409-MU16522c.h"
	#include "microcode-147-MU16502f.h"
	#include "microcode-94-MU265019.h"
	#include "microcode-430-MU165041.h"
	#include "microcode-452-MU165310.h"
	#include "microcode-434-MU165140.h"
	#include "microcode-435-MU165141.h"
	#include "microcode-433-MU165045.h"
	#include "microcode-429-MU165040.h"
	#include "microcode-436-MU165142.h"
	#include "microcode-411-MU16530c.h"

	/* Dummy terminator */
	0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0,
};

static void model_65x_init(device_t dev)
{
	/* Update the microcode */
	intel_update_microcode(microcode_updates);

	/* Initialize L2 cache */
	p6_configure_l2_cache();

	/* Turn on caching if we haven't already */
	x86_enable_cache();
	x86_setup_mtrrs();
	x86_mtrr_check();

	/* Enable the local cpu apics */
	setup_lapic();
};

static struct device_operations cpu_dev_ops = {
	.init     = model_65x_init,
};

/*
 * Intel Pentium II Processor Specification Update
 * http://download.intel.com/design/PentiumII/specupdt/24333749.pdf
 *
 * Mobile Intel Pentium II Processor Specification Update
 * http://download.intel.com/design/intarch/specupdt/24388757.pdf
 *
 * Intel Pentium II Xeon Processor Specification Update
 * http://download.intel.com/support/processors/pentiumii/xeon/24377632.pdf
 */
static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x0650 }, /* PII/Celeron, dA0/mdA0/A0 */
	{ X86_VENDOR_INTEL, 0x0651 }, /* PII/Celeron, dA1/A1 */
	{ X86_VENDOR_INTEL, 0x0652 }, /* PII/Celeron/Xeon, dB0/mdB0/B0 */
	{ X86_VENDOR_INTEL, 0x0653 }, /* PII/Xeon, dB1/B1 */
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
