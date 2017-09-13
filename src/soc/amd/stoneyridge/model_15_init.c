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
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/pae.h>
#include <pc80/mc146818rtc.h>

#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/amdfam15.h>
#include <arch/acpi.h>

static void msr_rw_dram(unsigned int reg)
{
#define RW_DRAM (MTRR_READ_MEM | MTRR_WRITE_MEM)
#define ALL_RW_DRAM ((RW_DRAM << 24) | (RW_DRAM << 16) | \
		     (RW_DRAM << 8)  | (RW_DRAM))

	msr_t mtrr = rdmsr(reg);
	mtrr.hi |= ALL_RW_DRAM;
	mtrr.lo |= ALL_RW_DRAM;
	wrmsr(reg, mtrr);
}

static void model_15_init(device_t dev)
{
	printk(BIOS_DEBUG, "Model 15 Init.\n");

	int i;
	msr_t msr;

	disable_cache();

	/* Enable access to AMD RdDram and WrDram extension bits */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	msr.lo &= ~SYSCFG_MSR_MtrrFixDramEn;
	wrmsr(SYSCFG_MSR, msr);

	/* Send all but A0000-BFFFF to DRAM */
	msr_rw_dram(MTRR_FIX_64K_00000);
	msr_rw_dram(MTRR_FIX_16K_80000);
	for (i = MTRR_FIX_4K_C0000 ; i <= MTRR_FIX_4K_F8000 ; i++)
		msr_rw_dram(i);

	/* Hide RdDram and WrDram bits, and clear Tom2ForceMemTypeWB */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo &= ~SYSCFG_MSR_TOM2WB;
	msr.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	msr.lo |= SYSCFG_MSR_MtrrFixDramEn;
	wrmsr(SYSCFG_MSR, msr);

	x86_enable_cache();

	/* zero the machine check error status registers */
	msr.lo = 0;
	msr.hi = 0;
	for (i = 0 ; i < 6 ; i++)
		wrmsr(MCI_STATUS + (i * 4), msr);

	/* Write protect SMM space with SMMLOCK. */
	msr = rdmsr(HWCR_MSR);
	msr.lo |= (1 << 0);
	wrmsr(HWCR_MSR, msr);
}

static struct device_operations cpu_dev_ops = {
	.init = model_15_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_AMD, 0x670f00 },
	{ 0, 0 },
};

static const struct cpu_driver model_15 __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
