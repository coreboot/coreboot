/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>

static void ep80579_init(struct device *dev)
{
	/* Turn on caching if we haven't already */
	x86_enable_cache();
	x86_setup_mtrrs();
	x86_mtrr_check();

	/* Update the microcode */
	intel_update_microcode_from_cbfs();

	/* Enable the local cpu apics */
	setup_lapic();
};

static struct device_operations cpu_dev_ops = {
	.init = ep80579_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x10650 }, /* EP80579 */
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops = &cpu_dev_ops,
	.id_table = cpu_table,
};
