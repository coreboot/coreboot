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
#include <cpu/x86/lapic.h>

#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/amdfam15.h>
#include <arch/acpi.h>

#include <amdlib.h>
#include <PspBaseLib.h>

void PSPProgBar3Msr(void *Buffer);

void PSPProgBar3Msr(void *Buffer)
{
	u32 Bar3Addr;
	u64 Tmp64;
	/* Get Bar3 Addr */
	Bar3Addr = PspLibPciReadPspConfig(0x20);
	Tmp64 = Bar3Addr;
	printk(BIOS_DEBUG, "Bar3=%llx\n", Tmp64);
	LibAmdMsrWrite(PSP_MSR_PRIVATE_BLOCK_BAR, &Tmp64, NULL);
	LibAmdMsrRead(PSP_MSR_PRIVATE_BLOCK_BAR, &Tmp64, NULL);
}

static void model_15_init(device_t dev)
{
	printk(BIOS_DEBUG, "Model 15 Init.\n");

	u8 i;
	msr_t msr;
	int msrno;
#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
	u32 siblings;
#endif

	disable_cache();
	/* Enable access to AMD RdDram and WrDram extension bits */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	msr.lo &= ~SYSCFG_MSR_MtrrFixDramEn;
	wrmsr(SYSCFG_MSR, msr);

	// BSP: make a0000-bffff UC, c0000-fffff WB
	msr.lo = msr.hi = 0;
	wrmsr(MTRR_FIX_16K_A0000, msr);
	msr.lo = msr.hi = 0x1e1e1e1e;
	wrmsr(MTRR_FIX_64K_00000, msr);
	wrmsr(MTRR_FIX_16K_80000, msr);
	for (msrno = MTRR_FIX_4K_C0000 ; msrno <= MTRR_FIX_4K_F8000 ; msrno++)
		wrmsr(msrno, msr);

	msr = rdmsr(SYSCFG_MSR);
	msr.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	msr.lo |= SYSCFG_MSR_MtrrFixDramEn;
	wrmsr(SYSCFG_MSR, msr);

	x86_mtrr_check();
	x86_enable_cache();

	/* zero the machine check error status registers */
	msr.lo = 0;
	msr.hi = 0;
	for (i = 0 ; i < 6 ; i++)
		wrmsr(MCI_STATUS + (i * 4), msr);


	/* Enable the local CPU APICs */
	setup_lapic();

#if IS_ENABLED(CONFIG_LOGICAL_CPUS)
	siblings = cpuid_ecx(0x80000008) & 0xff;

	if (siblings > 0) {
		msr = rdmsr_amd(CPU_ID_FEATURES_MSR);
		msr.lo |= 1 << 28;
		wrmsr_amd(CPU_ID_FEATURES_MSR, msr);

		msr = rdmsr_amd(CPU_ID_EXT_FEATURES_MSR);
		msr.hi |= 1 << (33 - 32);
		wrmsr_amd(CPU_ID_EXT_FEATURES_MSR, msr);
	}
	printk(BIOS_DEBUG, "siblings = %02d, ", siblings);
#endif
	PSPProgBar3Msr(NULL);

	/* DisableCf8ExtCfg */
	msr = rdmsr(NB_CFG_MSR);
	msr.hi &= ~(1 << (46 - 32));
	wrmsr(NB_CFG_MSR, msr);


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
