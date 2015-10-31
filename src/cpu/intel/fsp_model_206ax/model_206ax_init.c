/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
#include <string.h>
#include <arch/acpi.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/speedstep.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/name.h>
#include <pc80/mc146818rtc.h>
#include "model_206ax.h"
#include "chip.h"
#include <cpu/intel/smm/gen1/smi.h>

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

/* Convert time in seconds to POWER_LIMIT_1_TIME MSR value */
static const u8 power_limit_time_sec_to_msr[] = {
	[0]   = 0x00,
	[1]   = 0x0a,
	[2]   = 0x0b,
	[3]   = 0x4b,
	[4]   = 0x0c,
	[5]   = 0x2c,
	[6]   = 0x4c,
	[7]   = 0x6c,
	[8]   = 0x0d,
	[10]  = 0x2d,
	[12]  = 0x4d,
	[14]  = 0x6d,
	[16]  = 0x0e,
	[20]  = 0x2e,
	[24]  = 0x4e,
	[28]  = 0x6e,
	[32]  = 0x0f,
	[40]  = 0x2f,
	[48]  = 0x4f,
	[56]  = 0x6f,
	[64]  = 0x10,
	[80]  = 0x30,
	[96]  = 0x50,
	[112] = 0x70,
	[128] = 0x11,
};

/* Convert POWER_LIMIT_1_TIME MSR value to seconds */
static const u8 power_limit_time_msr_to_sec[] = {
	[0x00] = 0,
	[0x0a] = 1,
	[0x0b] = 2,
	[0x4b] = 3,
	[0x0c] = 4,
	[0x2c] = 5,
	[0x4c] = 6,
	[0x6c] = 7,
	[0x0d] = 8,
	[0x2d] = 10,
	[0x4d] = 12,
	[0x6d] = 14,
	[0x0e] = 16,
	[0x2e] = 20,
	[0x4e] = 24,
	[0x6e] = 28,
	[0x0f] = 32,
	[0x2f] = 40,
	[0x4f] = 48,
	[0x6f] = 56,
	[0x10] = 64,
	[0x30] = 80,
	[0x50] = 96,
	[0x70] = 112,
	[0x11] = 128,
};

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

/*
 * Configure processor power limits if possible
 * This must be done AFTER set of BIOS_RESET_CPL
 */
void set_power_limits(u8 power_limit_1_time)
{
	msr_t msr = rdmsr(MSR_PLATFORM_INFO);
	msr_t limit;
	unsigned power_unit;
	unsigned tdp, min_power, max_power, max_time;
	u8 power_limit_1_val;

	if (power_limit_1_time >= ARRAY_SIZE(power_limit_time_sec_to_msr))
		return;

	if (!(msr.lo & PLATFORM_INFO_SET_TDP))
		return;

	/* Get units */
	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 2 << ((msr.lo & 0xf) - 1);

	/* Get power defaults for this SKU */
	msr = rdmsr(MSR_PKG_POWER_SKU);
	tdp = msr.lo & 0x7fff;
	min_power = (msr.lo >> 16) & 0x7fff;
	max_power = msr.hi & 0x7fff;
	max_time = (msr.hi >> 16) & 0x7f;

	printk(BIOS_DEBUG, "CPU TDP: %u Watts\n", tdp / power_unit);

	if (power_limit_time_msr_to_sec[max_time] > power_limit_1_time)
		power_limit_1_time = power_limit_time_msr_to_sec[max_time];

	if (min_power > 0 && tdp < min_power)
		tdp = min_power;

	if (max_power > 0 && tdp > max_power)
		tdp = max_power;

	power_limit_1_val = power_limit_time_sec_to_msr[power_limit_1_time];

	/* Set long term power limit to TDP */
	limit.lo = 0;
	limit.lo |= tdp & PKG_POWER_LIMIT_MASK;
	limit.lo |= PKG_POWER_LIMIT_EN;
	limit.lo |= (power_limit_1_val & PKG_POWER_LIMIT_TIME_MASK) <<
		PKG_POWER_LIMIT_TIME_SHIFT;

	/* Set short term power limit to 1.25 * TDP */
	limit.hi = 0;
	limit.hi |= ((tdp * 125) / 100) & PKG_POWER_LIMIT_MASK;
	limit.hi |= PKG_POWER_LIMIT_EN;
	/* Power limit 2 time is only programmable on SNB EP/EX */

	wrmsr(MSR_PKG_POWER_LIMIT, limit);

	/* Use nominal TDP values for CPUs with configurable TDP */
	if (cpu_config_tdp_levels()) {
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		limit.hi = 0;
		limit.lo = msr.lo & 0xff;
		wrmsr(MSR_TURBO_ACTIVATION_RATIO, limit);
	}
}

static void configure_misc(void)
{
	msr_t msr;

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= (1 << 0);	  /* Fast String enable */
	msr.lo |= (1 << 3); 	  /* TM1/TM2/EMTTM enable */
	msr.lo |= (1 << 16);	  /* Enhanced SpeedStep Enable */
	wrmsr(IA32_MISC_ENABLE, msr);

	/* Disable Thermal interrupts */
	msr.lo = 0;
	msr.hi = 0;
	wrmsr(IA32_THERM_INTERRUPT, msr);

	/* Enable package critical interrupt only */
	msr.lo = 1 << 4;
	msr.hi = 0;
	wrmsr(IA32_PACKAGE_THERM_INTERRUPT, msr);
}

static void enable_lapic_tpr(void)
{
	msr_t msr;

	msr = rdmsr(MSR_PIC_MSG_CONTROL);
	msr.lo &= ~(1 << 10);	/* Enable APIC TPR updates */
	wrmsr(MSR_PIC_MSG_CONTROL, msr);
}

static void configure_dca_cap(void)
{
	struct cpuid_result cpuid_regs;
	msr_t msr;

	/* Check feature flag in CPUID.(EAX=1):ECX[18]==1 */
	cpuid_regs = cpuid(1);
	if (cpuid_regs.ecx & (1 << 18)) {
		msr = rdmsr(IA32_PLATFORM_DCA_CAP);
		msr.lo |= 1;
		wrmsr(IA32_PLATFORM_DCA_CAP, msr);
	}
}

static void set_max_ratio(void)
{
	msr_t msr, perf_ctl;

	perf_ctl.hi = 0;

	/* Check for configurable TDP option */
	if (cpu_config_tdp_levels()) {
		/* Set to nominal TDP ratio */
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		perf_ctl.lo = (msr.lo & 0xff) << 8;
	} else {
		/* Platform Info bits 15:8 give max ratio */
		msr = rdmsr(MSR_PLATFORM_INFO);
		perf_ctl.lo = msr.lo & 0xff00;
	}
	wrmsr(IA32_PERF_CTL, perf_ctl);

	printk(BIOS_DEBUG, "model_x06ax: frequency set to %d\n",
	       ((perf_ctl.lo >> 8) & 0xff) * SANDYBRIDGE_BCLK);
}

static void configure_mca(void)
{
	msr_t msr;
	int i;

	msr.lo = msr.hi = 0;
	/* This should only be done on a cold boot */
	for (i = 0; i < 7; i++)
		wrmsr(IA32_MC0_STATUS + (i * 4), msr);
}

int cpu_get_apic_id_map(int *apic_id_map)
{
	struct cpuid_result result;
	unsigned threads_per_package, threads_per_core, i, shift = 0;

	/* Logical processors (threads) per core */
	result = cpuid_ext(0xb, 0);
	threads_per_core = result.ebx & 0xffff;

	/* Logical processors (threads) per package */
	result = cpuid_ext(0xb, 1);
	threads_per_package = result.ebx & 0xffff;

	if (threads_per_core == 1)
		shift++;

	for (i = 0; i < threads_per_package && i < CONFIG_MAX_CPUS; i++)
		apic_id_map[i] = i << shift;

	return threads_per_package;
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

#if CONFIG_SMP && CONFIG_MAX_CPUS > 1
		/* Start the new cpu */
		if (!start_cpu(new)) {
			/* Record the error in cpu? */
			printk(BIOS_ERR, "CPU %u would not start!\n",
			       new->path.apic.apic_id);
		}
#endif
	}
}

static void model_206ax_init(struct device *cpu)
{
	char processor_name[49];
	struct cpuid_result cpuid_regs;

	/* Turn on caching if we haven't already */
	x86_enable_cache();

	intel_update_microcode_from_cbfs();

	/* Clear out pending MCEs */
	configure_mca();

	/* Print processor name */
	fill_processor_name(processor_name);
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);

	/* Setup MTRRs based on physical address size */
	cpuid_regs = cpuid(0x80000008);
	x86_setup_fixed_mtrrs();
	x86_setup_var_mtrrs(cpuid_regs.eax & 0xff, 2);
	x86_mtrr_check();

	/* Setup Page Attribute Tables (PAT) */
	// TODO set up PAT

	/* Enable the local cpu apics */
	enable_lapic_tpr();
	setup_lapic();

	/* Enable virtualization if Kconfig option is set */
	enable_vmx();

	/* Configure Enhanced SpeedStep and Thermal Sensors */
	configure_misc();

	/* Enable Direct Cache Access */
	configure_dca_cap();

	/* Set Max Ratio */
	set_max_ratio();

	/* Enable Turbo */
	enable_turbo();

	/* Start up extra cores */
	intel_cores_init(cpu);
}

static struct device_operations cpu_dev_ops = {
	.init     = model_206ax_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x206a0 }, /* Intel Sandybridge */
	{ X86_VENDOR_INTEL, 0x206a6 }, /* Intel Sandybridge D1 */
	{ X86_VENDOR_INTEL, 0x206a7 }, /* Intel Sandybridge D2/J1 */
	{ X86_VENDOR_INTEL, 0x306a0 }, /* Intel IvyBridge */
	{ X86_VENDOR_INTEL, 0x306a2 }, /* Intel IvyBridge */
	{ X86_VENDOR_INTEL, 0x306a4 }, /* Intel IvyBridge */
	{ X86_VENDOR_INTEL, 0x306a5 }, /* Intel IvyBridge */
	{ X86_VENDOR_INTEL, 0x306a6 }, /* Intel IvyBridge */
	{ X86_VENDOR_INTEL, 0x306a8 }, /* Intel IvyBridge */
	{ X86_VENDOR_INTEL, 0x306a9 }, /* Intel IvyBridge */
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
