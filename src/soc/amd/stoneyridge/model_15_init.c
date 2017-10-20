/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Advanced Micro Devices, Inc.
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

#include <console/console.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/pae.h>
#include <pc80/mc146818rtc.h>

#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/amd/amdfam15.h>
#include <arch/acpi.h>

static void model_15_init(device_t dev)
{
	printk(BIOS_DEBUG, "Model 15 Init.\n");

	int i;
	msr_t msr;

	/* zero the machine check error status registers */
	msr.lo = 0;
	msr.hi = 0;
	for (i = 0 ; i < 6 ; i++)
		wrmsr(MCI_STATUS + (i * 4), msr);

	setup_lapic();
}

static struct device_operations cpu_dev_ops = {
	.init = model_15_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, 0x670f00 },
	{ 0, 0 },
};

static const struct cpu_driver model_15 __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
