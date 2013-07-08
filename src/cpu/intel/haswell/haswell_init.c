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
#include "haswell.h"
#include "chip.h"

/*
 * List of supported C-states in this processor
 *
 * Latencies are typical worst-case package exit time in uS
 * taken from the SandyBridge BIOS specification.
 */
#if 0
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
#endif

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

#if 0
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

	msr = rdmsr(MSR_PMG_IO_CAPTURE_BASE);
	msr.lo &= ~0x7ffff;
	msr.lo |= (get_pmbase() + 4);	// LVL_2 base address
	msr.lo |= (2 << 16);		// CST Range: C7 is max C-state
	wrmsr(MSR_PMG_IO_CAPTURE_BASE, msr);

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
#endif

static void configure_thermal_target(void)
{
	struct cpu_intel_haswell_config *conf;
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

	printk(BIOS_DEBUG, "haswell: frequency set to %d\n",
	       ((perf_ctl.lo >> 8) & 0xff) * HASWELL_BCLK);
}

static void set_energy_perf_bias(u8 policy)
{
	msr_t msr;
	int ecx;

	/* Determine if energy efficient policy is supported. */
	ecx = cpuid_ecx(0x6);
	if (!(ecx & (1 << 3)))
		return;

	/* Energy Policy is bits 3:0 */
	msr = rdmsr(IA32_ENERGY_PERFORMANCE_BIAS);
	msr.lo &= ~0xf;
	msr.lo |= policy & 0xf;
	wrmsr(IA32_ENERGY_PERFORMANCE_BIAS, msr);

	printk(BIOS_DEBUG, "haswell: energy policy set to %u\n",
	       policy);
}

static void configure_mca(void)
{
	msr_t msr;
	const unsigned int mcg_cap_msr = 0x179;
	int i;
	int num_banks;

	msr = rdmsr(mcg_cap_msr);
	num_banks = msr.lo & 0xff;
	msr.lo = msr.hi = 0;
	/* TODO(adurbin): This should only be done on a cold boot. Also, some
	 * of these banks are core vs package scope. For now every CPU clears
	 * every bank. */
	for (i = 0; i < num_banks; i++)
		wrmsr(IA32_MC0_STATUS + (i * 4), msr);
}

static void bsp_init_before_ap_bringup(struct bus *cpu_bus)
{
	struct device_path cpu_path;
	struct cpu_info *info;
	char processor_name[49];

	/* Print processor name */
	fill_processor_name(processor_name);
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);

	/* Ensure the local apic is enabled */
	enable_lapic();

	/* Set the device path of the boot cpu. */
	cpu_path.type = DEVICE_PATH_APIC;
	cpu_path.apic.apic_id = lapicid();

	/* Find the device structure for the boot cpu. */
	info = cpu_info();
	info->cpu = alloc_find_dev(cpu_bus, &cpu_path);

	if (info->index != 0)
		printk(BIOS_CRIT, "BSP index(%d) != 0!\n", info->index);

	/* Setup MTRRs based on physical address size. */
	x86_setup_fixed_mtrrs();
	x86_setup_var_mtrrs(cpuid_eax(0x80000008) & 0xff, 2);
	x86_mtrr_check();

	/* Call through the cpu driver's initialization. */
	cpu_initialize(0);
}

/* All CPUs including BSP will run the following function. */
static void haswell_init(device_t cpu)
{
	/* Clear out pending MCEs */
	configure_mca();

	/* Enable the local cpu apics */
	enable_lapic_tpr();
	setup_lapic();

	/* Configure C States */
	//configure_c_states();

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
}

void bsp_init_and_start_aps(struct bus *cpu_bus)
{
	int max_cpus;
	int num_aps;
	const void *microcode_patch;

	/* Perform any necessary BSP initialization before APs are brought up.
	 * This call also allows the BSP to prepare for any secondary effects
	 * from calling cpu_initialize() such as smm_init(). */
	bsp_init_before_ap_bringup(cpu_bus);

	microcode_patch = intel_microcode_find();

	/* This needs to be called after the mtrr setup so the BSP mtrrs
	 * can be mirrored by the APs. */
	if (setup_ap_init(cpu_bus, &max_cpus, microcode_patch)) {
		printk(BIOS_CRIT, "AP setup initialization failed. "
		       "No APs will be brought up.\n");
		return;
	}

	num_aps = max_cpus - 1;
	if (start_aps(cpu_bus, num_aps)) {
		printk(BIOS_CRIT, "AP startup failed. Trying to continue.\n");
	}

	if (smm_initialize()) {
		printk(BIOS_CRIT, "SMM Initialization failed...\n");
		return;
	}

	/* After SMM relocation a 2nd microcode load is required. */
	intel_microcode_load_unlocked(microcode_patch);

	/* Enable ROM caching if option was selected. */
	x86_mtrr_enable_rom_caching();
}

static struct device_operations cpu_dev_ops = {
	.init     = haswell_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x306c1 }, /* Intel Haswell 4+2 A0 */
	{ X86_VENDOR_INTEL, 0x306c2 }, /* Intel Haswell 4+2 B0 */
	{ X86_VENDOR_INTEL, 0x40650 }, /* Intel Haswell ULT B0 */
	{ X86_VENDOR_INTEL, 0x40651 }, /* Intel Haswell ULT B1 */
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
	/* .cstates  = cstate_map, */
};

