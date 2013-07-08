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

/*
 * List of supported C-states in this processor
 *
 * Latencies are typical worst-case package exit time in uS
 * taken from the SandyBridge BIOS specification.
 */
static acpi_cstate_t cstate_map[] = {
	{	/* 0: C0 */
	},{	/* 1: C1 */
		.latency = 1,
		.power = 1000,
		.resource = {
			.addrl = 0x00,	/* MWAIT State 0 */
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.resv = ACPI_FFIXEDHW_FLAG_HW_COORD,
		}
	},
	{	/* 2: C1E */
		.latency = 1,
		.power = 1000,
		.resource = {
			.addrl = 0x01,	/* MWAIT State 0 Sub-state 1 */
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.resv = ACPI_FFIXEDHW_FLAG_HW_COORD,
		}
	},
	{	/* 3: C3 */
		.latency = 63,
		.power = 500,
		.resource = {
			.addrl = 0x10,	/* MWAIT State 1 */
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.resv = ACPI_FFIXEDHW_FLAG_HW_COORD,
		}
	},
	{	/* 4: C6 */
		.latency = 87,
		.power = 350,
		.resource = {
			.addrl = 0x20,	/* MWAIT State 2 */
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.resv = ACPI_FFIXEDHW_FLAG_HW_COORD,
		}
	},
	{	/* 5: C7 */
		.latency = 90,
		.power = 200,
		.resource = {
			.addrl = 0x30,	/* MWAIT State 3 */
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.resv = ACPI_FFIXEDHW_FLAG_HW_COORD,
		}
	},
	{	/* 6: C7S */
		.latency = 90,
		.power = 200,
		.resource = {
			.addrl = 0x31,	/* MWAIT State 3 Sub-state 1 */
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.resv = ACPI_FFIXEDHW_FLAG_HW_COORD,
		}
	},
	{ 0 }
};

static void enable_vmx(void)
{
	struct cpuid_result regs;
	msr_t msr;
	int enable = CONFIG_ENABLE_VMX;

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

	/* Even though the Intel manual says you must set the lock bit in addition
	 * to the VMX bit in order for VMX to work, it is incorrect.  Thus we leave
	 * it unlocked for the OS to manage things itself.  This is good for a few
	 * reasons:
	 * - No need to reflash the bios just to toggle the lock bit.
	 * - The VMX bits really really should match each other across cores, so
	 *   hard locking it on one while another has the opposite setting can
	 *   easily lead to crashes as code using VMX migrates between them.
	 * - Vendors that want to "upsell" from a bios that disables+locks to
	 *   one that doesn't is sleazy.
	 * By leaving this to the OS (e.g. Linux), people can do exactly what they
	 * want on the fly, and do it correctly (e.g. across multiple cores).
	 */
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

	if (power_limit_1_time > ARRAY_SIZE(power_limit_time_sec_to_msr))
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

static void configure_c_states(void)
{
	msr_t msr;

	msr = rdmsr(MSR_PMG_CST_CONFIG_CONTROL);
	msr.lo |= (1 << 28);	// C1 Auto Undemotion Enable
	msr.lo |= (1 << 27);	// C3 Auto Undemotion Enable
	msr.lo |= (1 << 26);	// C1 Auto Demotion Enable
	msr.lo |= (1 << 25);	// C3 Auto Demotion Enable
	msr.lo &= ~(1 << 10);	// Disable IO MWAIT redirection
	msr.lo |= 7;		// No package C-state limit
	wrmsr(MSR_PMG_CST_CONFIG_CONTROL, msr);

	msr = rdmsr(MSR_PMG_IO_CAPTURE_ADDR);
	msr.lo &= ~0x7ffff;
	msr.lo |= (PMB0_BASE + 4);	// LVL_2 base address
	msr.lo |= (2 << 16);		// CST Range: C7 is max C-state
	wrmsr(MSR_PMG_IO_CAPTURE_ADDR, msr);

	msr = rdmsr(MSR_MISC_PWR_MGMT);
	msr.lo &= ~(1 << 0);	// Enable P-state HW_ALL coordination
	wrmsr(MSR_MISC_PWR_MGMT, msr);

	msr = rdmsr(MSR_POWER_CTL);
	msr.lo |= (1 << 18);	// Enable Energy Perf Bias MSR 0x1b0
	msr.lo |= (1 << 1);	// C1E Enable
	msr.lo |= (1 << 0);	// Bi-directional PROCHOT#
	wrmsr(MSR_POWER_CTL, msr);

	/* C3 Interrupt Response Time Limit */
	msr.hi = 0;
	msr.lo = IRTL_VALID | IRTL_1024_NS | 0x50;
	wrmsr(MSR_PKGC3_IRTL, msr);

	/* C6 Interrupt Response Time Limit */
	msr.hi = 0;
	msr.lo = IRTL_VALID | IRTL_1024_NS | 0x68;
	wrmsr(MSR_PKGC6_IRTL, msr);

	/* C7 Interrupt Response Time Limit */
	msr.hi = 0;
	msr.lo = IRTL_VALID | IRTL_1024_NS | 0x6D;
	wrmsr(MSR_PKGC7_IRTL, msr);

	/* Primary Plane Current Limit */
	msr = rdmsr(MSR_PP0_CURRENT_CONFIG);
	msr.lo &= ~0x1fff;
	msr.lo |= PP0_CURRENT_LIMIT;
	wrmsr(MSR_PP0_CURRENT_CONFIG, msr);

	/* Secondary Plane Current Limit */
	msr = rdmsr(MSR_PP1_CURRENT_CONFIG);
	msr.lo &= ~0x1fff;
	if (cpuid_eax(1) >= 0x30600)
		msr.lo |= PP1_CURRENT_LIMIT_IVB;
	else
		msr.lo |= PP1_CURRENT_LIMIT_SNB;
	wrmsr(MSR_PP1_CURRENT_CONFIG, msr);
}

static void configure_thermal_target(void)
{
	struct cpu_intel_model_206ax_config *conf;
	device_t lapic;
	msr_t msr;

	/* Find pointer to CPU configuration */
	lapic = dev_find_lapic(SPEEDSTEP_APIC_MAGIC);
	if (!lapic || !lapic->chip_info)
		return;
	conf = lapic->chip_info;

	/* Set TCC activation offset if supported */
	msr = rdmsr(MSR_PLATFORM_INFO);
	if ((msr.lo & (1 << 30)) && conf->tcc_offset) {
		msr = rdmsr(MSR_TEMPERATURE_TARGET);
		msr.lo &= ~(0xf << 24); /* Bits 27:24 */
		msr.lo |= (conf->tcc_offset & 0xf) << 24;
		wrmsr(MSR_TEMPERATURE_TARGET, msr);
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

static void set_energy_perf_bias(u8 policy)
{
	msr_t msr;

	/* Energy Policy is bits 3:0 */
	msr = rdmsr(IA32_ENERGY_PERFORMANCE_BIAS);
	msr.lo &= ~0xf;
	msr.lo |= policy & 0xf;
	wrmsr(IA32_ENERGY_PERFORMANCE_BIAS, msr);

	printk(BIOS_DEBUG, "model_x06ax: energy policy set to %u\n",
	       policy);
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

/*
 * Initialize any extra cores/threads in this package.
 */
static void intel_cores_init(device_t cpu)
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
		device_t new;

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

static void model_206ax_init(device_t cpu)
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

	/* Enable virtualization if enabled in CMOS */
	enable_vmx();

	/* Configure C States */
	configure_c_states();

	/* Configure Enhanced SpeedStep and Thermal Sensors */
	configure_misc();

	/* Thermal throttle activation offset */
	configure_thermal_target();

	/* Enable Direct Cache Access */
	configure_dca_cap();

	/* Set energy policy */
	set_energy_perf_bias(ENERGY_POLICY_NORMAL);

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
	.cstates  = cstate_map,
};

