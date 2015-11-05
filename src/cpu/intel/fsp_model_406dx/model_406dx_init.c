/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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
#include <device/pci.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/name.h>
#include "model_406dx.h"
#include "chip.h"

static void enable_vmx(void)
{
	struct cpuid_result regs;
	msr_t msr;
	int enable = IS_ENABLED(CONFIG_ENABLE_VMX);

	regs = cpuid(1);
	/* Check that the VMX is supported before reading or writing the MSR. */
	if (!((regs.ecx & CPUID_VMX) || (regs.ecx & CPUID_SMX)))
		return;

	msr = rdmsr(IA32_FEATURE_CONTROL);

	if (msr.lo & (1 << 0)) {
		printk(BIOS_ERR, "VMX is locked, so %s will do nothing\n", __func__);
		/* VMX locked. If we set it again we get an illegal
		 * instruction
		 */
		return;
	}

	/* The IA32_FEATURE_CONTROL MSR may initialize with random values.
	 * It must be cleared regardless of VMX config setting.
	 */
	msr.hi = msr.lo = 0;

	printk(BIOS_DEBUG, "%s VMX\n", enable ? "Enabling" : "Disabling");

	if (enable) {
		msr.lo |= (1 << 2);
		if (regs.ecx & CPUID_SMX)
			msr.lo |= (1 << 1);
	}

	wrmsr(IA32_FEATURE_CONTROL, msr);
}

int cpu_config_tdp_levels(void)
{
	msr_t platform_info;

	/* Minimum CPU revision */
	if (cpuid_eax(1) < IVB_CONFIG_TDP_MIN_CPUID)
		return 0;

	/* Bits 34:33 indicate how many levels supported */
	platform_info = rdmsr(MSR_PLATFORM_INFO);
	return (platform_info.hi >> 1) & 3;
}

static void configure_misc(void)
{
	msr_t msr;

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= (1 << 0);	/* Fast String enable */
	msr.lo |= (1 << 3);	/* TM1/TM2/EMTTM enable */
	msr.lo |= (1 << 16);	/* Enhanced SpeedStep Enable */
	wrmsr(IA32_MISC_ENABLE, msr);

	/* Disable Thermal interrupts */
	msr.lo = 0;
	msr.hi = 0;
	wrmsr(IA32_THERM_INTERRUPT, msr);
}

static void configure_mca(void)
{
	msr_t msr;
	int i;

	msr.lo = msr.hi = 0;
	/* This should only be done on a cold boot */
	for (i = 0; i < 6; i++)
		wrmsr(IA32_MC0_STATUS + (i * 4), msr);
}

/*
 * Initialize any extra cores/threads in this package.
 */
static void intel_cores_init(struct device *cpu)
{
	struct cpuid_result result;
	unsigned threads_per_package, threads_per_core, i;

	/* Logical processors (threads) per core */
	result = cpuid_ext(0xb, 0);
	threads_per_core = result.ebx & 0xffff;

	/* Logical processors (threads) per package */
	result = cpuid_ext(0xb, 1);
	threads_per_package = result.ebx & 0xffff;

	/* Only initialize extra cores from BSP */
	if (cpu->path.apic.apic_id)
		return;

	printk(BIOS_DEBUG, "CPU: %u has %u cores, %u threads per core\n",
	       cpu->path.apic.apic_id, threads_per_package/threads_per_core,
	       threads_per_core);

	for (i = 1; i < threads_per_package; ++i) {
		struct device_path cpu_path;
		struct device *new;

		/* Build the cpu device path */
		cpu_path.type = DEVICE_PATH_APIC;
		cpu_path.apic.apic_id =
			cpu->path.apic.apic_id + i;

		/* Update APIC ID if no hyperthreading */
		if (threads_per_core == 1)
			cpu_path.apic.apic_id <<= 1;

		/* Allocate the new cpu device structure */
		new = alloc_dev(cpu->bus, &cpu_path);
		if (!new)
			continue;

		printk(BIOS_DEBUG, "CPU: %u has core %u\n",
		       cpu->path.apic.apic_id,
		       new->path.apic.apic_id);

#if IS_ENABLED(CONFIG_SMP) && CONFIG_MAX_CPUS > 1
		/* Start the new cpu */
		if (!start_cpu(new)) {
			/* Record the error in cpu? */
			printk(BIOS_ERR, "CPU %u would not start!\n",
			       new->path.apic.apic_id);
		}
#endif
	}
}

static void model_406dx_init(struct device *cpu)
{
	char processor_name[49];

	/* Turn on caching if we haven't already */
	x86_enable_cache();

	/* Load microcode */
	if (IS_ENABLED(CONFIG_SUPPORT_CPU_UCODE_IN_CBFS))
		intel_update_microcode_from_cbfs();

	/* Clear out pending MCEs */
	configure_mca();

	/* Print processor name */
	fill_processor_name(processor_name);
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);

	x86_mtrr_check();

	/* Enable the local cpu apics */
	setup_lapic();

	/* Enable virtualization */
	enable_vmx();

	/* Configure Enhanced SpeedStep and Thermal Sensors */
	configure_misc();

	/* Start up extra cores */
	intel_cores_init(cpu);
}

static struct device_operations cpu_dev_ops = {
	.init     = model_406dx_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x406d0 }, /* Intel Avoton/Rangeley A1 */
	{ X86_VENDOR_INTEL, 0x406d8 }, /* Intel Avoton/Rangeley B0 */
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
