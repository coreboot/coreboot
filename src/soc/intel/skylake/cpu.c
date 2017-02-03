/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2017 Intel Corporation.
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

#include <assert.h>
#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <chip.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/speedstep.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/name.h>
#include <cpu/x86/smm.h>
#include <delay.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/fast_spi.h>
#include <pc80/mc146818rtc.h>
#include <soc/cpu.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/smm.h>
#include <soc/systemagent.h>

/* MP initialization support. */
static const void *microcode_patch;
static int ht_disabled;

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

/*
 * Configure processor power limits if possible
 * This must be done AFTER set of BIOS_RESET_CPL
 */
void set_power_limits(u8 power_limit_1_time)
{
	msr_t msr = rdmsr(MSR_PLATFORM_INFO);
	msr_t limit;
	unsigned int power_unit;
	unsigned int tdp, min_power, max_power, max_time, tdp_pl2;
	u8 power_limit_1_val;
	device_t dev = SA_DEV_ROOT;
	config_t *conf = dev->chip_info;

	if (power_limit_1_time > ARRAY_SIZE(power_limit_time_sec_to_msr))
		power_limit_1_time = 28;

	if (!(msr.lo & PLATFORM_INFO_SET_TDP))
		return;

	/* Get units */
	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 1 << (msr.lo & 0xf);

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

	/* Set PL1 Pkg Power clamp bit */
	limit.lo |= PKG_POWER_LIMIT_CLAMP;

	limit.lo |= PKG_POWER_LIMIT_EN;
	limit.lo |= (power_limit_1_val & PKG_POWER_LIMIT_TIME_MASK) <<
		PKG_POWER_LIMIT_TIME_SHIFT;

	/* Set short term power limit to 1.25 * TDP */
	limit.hi = 0;
	tdp_pl2 = (conf->tdp_pl2_override == 0) ?
		(tdp * 125) / 100 : (conf->tdp_pl2_override * power_unit);
	limit.hi |= (tdp_pl2) & PKG_POWER_LIMIT_MASK;
	limit.hi |= PKG_POWER_LIMIT_CLAMP;
	limit.hi |= PKG_POWER_LIMIT_EN;

	/* Power limit 2 time is only programmable on server SKU */
	wrmsr(MSR_PKG_POWER_LIMIT, limit);

	/* Set PL2 power limit values in MCHBAR and disable PL1 */
	MCHBAR32(MCH_PKG_POWER_LIMIT_LO) = limit.lo & (~(PKG_POWER_LIMIT_EN));
	MCHBAR32(MCH_PKG_POWER_LIMIT_HI) = limit.hi;

	/* Set DDR RAPL power limit by copying from MMIO to MSR */
	msr.lo = MCHBAR32(MCH_DDR_POWER_LIMIT_LO);
	msr.hi = MCHBAR32(MCH_DDR_POWER_LIMIT_HI);
	wrmsr(MSR_DDR_RAPL_LIMIT, msr);

	/* Use nominal TDP values for CPUs with configurable TDP */
	if (cpu_config_tdp_levels()) {
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		limit.hi = 0;
		limit.lo = cpu_get_tdp_nominal_ratio();
		wrmsr(MSR_TURBO_ACTIVATION_RATIO, limit);
	}
}

static void configure_thermal_target(void)
{
	device_t dev = SA_DEV_ROOT;
	config_t *conf = dev->chip_info;
	msr_t msr;

	/* Set TCC activation offset if supported */
	msr = rdmsr(MSR_PLATFORM_INFO);
	if ((msr.lo & (1 << 30)) && conf->tcc_offset) {
		msr = rdmsr(MSR_TEMPERATURE_TARGET);
		msr.lo &= ~(0xf << 24); /* Bits 27:24 */
		msr.lo |= (conf->tcc_offset & 0xf) << 24;
		wrmsr(MSR_TEMPERATURE_TARGET, msr);
	}
	msr = rdmsr(MSR_TEMPERATURE_TARGET);
	msr.lo &= ~0x7f; /* Bits 6:0 */
	msr.lo |= 0xe6; /* setting 100ms thermal time window */
	wrmsr(MSR_TEMPERATURE_TARGET, msr);
}

static void configure_isst(void)
{
	device_t dev = SA_DEV_ROOT;
	config_t *conf = dev->chip_info;
	msr_t msr;

	if (conf->speed_shift_enable) {
		/*
		* Kernel driver checks CPUID.06h:EAX[Bit 7] to determine if HWP
		  is supported or not. coreboot needs to configure MSR 0x1AA
		  which is then reflected in the CPUID register.
		*/
		msr = rdmsr(MSR_MISC_PWR_MGMT);
		msr.lo |= MISC_PWR_MGMT_ISST_EN; /* Enable Speed Shift */
		msr.lo |= MISC_PWR_MGMT_ISST_EN_INT; /* Enable Interrupt */
		msr.lo |= MISC_PWR_MGMT_ISST_EN_EPP; /* Enable EPP */
		wrmsr(MSR_MISC_PWR_MGMT, msr);
	} else {
		msr = rdmsr(MSR_MISC_PWR_MGMT);
		msr.lo &= ~MISC_PWR_MGMT_ISST_EN; /* Disable Speed Shift */
		msr.lo &= ~MISC_PWR_MGMT_ISST_EN_INT; /* Disable Interrupt */
		msr.lo &= ~MISC_PWR_MGMT_ISST_EN_EPP; /* Disable EPP */
		wrmsr(MSR_MISC_PWR_MGMT, msr);
	}
}

static void configure_misc(void)
{
	device_t dev = SA_DEV_ROOT;
	config_t *conf = dev->chip_info;
	msr_t msr;

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= (1 << 0);	/* Fast String enable */
	msr.lo |= (1 << 3);	/* TM1/TM2/EMTTM enable */
	if (conf->eist_enable)
		cpu_enable_eist();
	else
		cpu_disable_eist();
	wrmsr(IA32_MISC_ENABLE, msr);

	/* Disable Thermal interrupts */
	msr.lo = 0;
	msr.hi = 0;
	wrmsr(IA32_THERM_INTERRUPT, msr);

	/* Enable package critical interrupt only */
	msr.lo = 1 << 4;
	msr.hi = 0;
	wrmsr(IA32_PACKAGE_THERM_INTERRUPT, msr);

	/* Enable PROCHOT */
	msr = rdmsr(MSR_POWER_CTL);
	msr.lo |= (1 << 0);	/* Enable Bi-directional PROCHOT as an input*/
	msr.lo |= (1 << 23);	/* Lock it */
	wrmsr(MSR_POWER_CTL, msr);
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

	printk(BIOS_DEBUG, "cpu: energy policy set to %u\n", policy);
}

static void configure_mca(void)
{
	msr_t msr;
	int i;
	int num_banks;

	msr = rdmsr(IA32_MCG_CAP);
	num_banks = msr.lo & 0xff;
	msr.lo = msr.hi = 0;
	/*
	 * TODO(adurbin): This should only be done on a cold boot. Also, some
	 * of these banks are core vs package scope. For now every CPU clears
	 * every bank.
	 */
	for (i = 0; i < num_banks; i++) {
		/* Clear the machine check status */
		wrmsr(IA32_MC0_STATUS + (i * 4), msr);
		/* Initialize machine checks */
		wrmsr(IA32_MC0_CTL + i * 4,
			(msr_t) {.lo = 0xffffffff, .hi = 0xffffffff});
	}
}

static void configure_c_states(void)
{
	msr_t msr;

	/* C-state Interrupt Response Latency Control 0 - package C3 latency */
	msr.hi = 0;
	msr.lo = IRTL_VALID | IRTL_1024_NS | C_STATE_LATENCY_CONTROL_0_LIMIT;
	wrmsr(MSR_C_STATE_LATENCY_CONTROL_0, msr);

	/* C-state Interrupt Response Latency Control 1 - package C6/C7 short */
	msr.hi = 0;
	msr.lo = IRTL_VALID | IRTL_1024_NS | C_STATE_LATENCY_CONTROL_1_LIMIT;
	wrmsr(MSR_C_STATE_LATENCY_CONTROL_1, msr);

	/* C-state Interrupt Response Latency Control 2 - package C6/C7 long */
	msr.hi = 0;
	msr.lo = IRTL_VALID | IRTL_1024_NS | C_STATE_LATENCY_CONTROL_2_LIMIT;
	wrmsr(MSR_C_STATE_LATENCY_CONTROL_2, msr);

	/* C-state Interrupt Response Latency Control 3 - package C8 */
	msr.hi = 0;
	msr.lo = IRTL_VALID | IRTL_1024_NS |
		C_STATE_LATENCY_CONTROL_3_LIMIT;
	wrmsr(MSR_C_STATE_LATENCY_CONTROL_3, msr);

	/* C-state Interrupt Response Latency Control 4 - package C9 */
	msr.hi = 0;
	msr.lo = IRTL_VALID | IRTL_1024_NS |
		C_STATE_LATENCY_CONTROL_4_LIMIT;
	wrmsr(MSR_C_STATE_LATENCY_CONTROL_4, msr);

	/* C-state Interrupt Response Latency Control 5 - package C10 */
	msr.hi = 0;
	msr.lo = IRTL_VALID | IRTL_1024_NS |
		C_STATE_LATENCY_CONTROL_5_LIMIT;
	wrmsr(MSR_C_STATE_LATENCY_CONTROL_5, msr);
}

/*
 * The emulated ACPI timer allows disabling of the ACPI timer
 * (PM1_TMR) to have no impart on the system.
 */
static void enable_pm_timer_emulation(void)
{
	/* ACPI PM timer emulation */
	msr_t msr;
	/*
	 * The derived frequency is calculated as follows:
	 *    (CTC_FREQ * msr[63:32]) >> 32 = target frequency.
	 * Back solve the multiplier so the 3.579545MHz ACPI timer
	 * frequency is used.
	 */
	msr.hi = (3579545ULL << 32) / CTC_FREQ;
	/* Set PM1 timer IO port and enable*/
	msr.lo = (EMULATE_DELAY_VALUE << EMULATE_DELAY_OFFSET_VALUE) |
			EMULATE_PM_TMR_EN | (ACPI_BASE_ADDRESS + PM1_TMR);
	wrmsr(MSR_EMULATE_PM_TMR, msr);
}

/* All CPUs including BSP will run the following function. */
static void cpu_core_init(device_t cpu)
{
	/* Clear out pending MCEs */
	configure_mca();

	/* Enable the local CPU apics */
	enable_lapic_tpr();
	setup_lapic();

	/* Configure c-state interrupt response time */
	configure_c_states();

	/* Configure Enhanced SpeedStep and Thermal Sensors */
	configure_misc();

	/* Configure Intel Speed Shift */
	configure_isst();

	/* Enable ACPI Timer Emulation via MSR 0x121 */
	enable_pm_timer_emulation();

	/* Enable Direct Cache Access */
	configure_dca_cap();

	/* Set energy policy */
	set_energy_perf_bias(ENERGY_POLICY_NORMAL);

	/* Enable Turbo */
	enable_turbo();

	/* Configure SGX */
	configure_sgx(microcode_patch);
}

static struct device_operations cpu_dev_ops = {
	.init = cpu_core_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_C0 },
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_D0 },
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_HQ0 },
	{ X86_VENDOR_INTEL, CPUID_SKYLAKE_HR0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_G0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_H0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_Y0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_HA0 },
	{ X86_VENDOR_INTEL, CPUID_KABYLAKE_HB0 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

static int get_cpu_count(void)
{
	msr_t msr;
	int num_threads;
	int num_cores;

	msr = rdmsr(MSR_CORE_THREAD_COUNT);
	num_threads = (msr.lo >> 0) & 0xffff;
	num_cores = (msr.lo >> 16) & 0xffff;
	printk(BIOS_DEBUG, "CPU has %u cores, %u threads enabled.\n",
	       num_cores, num_threads);

	ht_disabled = num_threads == num_cores;

	return num_threads;
}

static void get_microcode_info(const void **microcode, int *parallel)
{
	microcode_patch = intel_microcode_find();
	*microcode = microcode_patch;
	*parallel = 1;
	intel_microcode_load_unlocked(microcode_patch);
}

static int adjust_apic_id(int index, int apic_id)
{
	if (ht_disabled)
		return 2 * index;
	else
		return index;
}

/* Check whether the current CPU is the sibling hyperthread. */
int is_secondary_thread(void)
{
	int apic_id;
	apic_id = lapicid();

	if (!ht_disabled && (apic_id & 1))
		return 1;
	return 0;
}

static void per_cpu_smm_trigger(void)
{
	/* Relocate the SMM handler. */
	smm_relocate();

	/* After SMM relocation a 2nd microcode load is required. */
	intel_microcode_load_unlocked(microcode_patch);
}

static void post_mp_init(void)
{
	/* Set Max Ratio */
	cpu_set_max_ratio();

	/*
	 * Now that all APs have been relocated as well as the BSP let SMIs
	 * start flowing.
	 */
	southbridge_smm_enable_smi();

	/* Lock down the SMRAM space. */
#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
	smm_lock();
#endif
}

static const struct mp_ops mp_ops = {
	/*
	 * Skip Pre MP init MTRR programming as MTRRs are mirrored from BSP,
	 * that are set prior to ramstage.
	 * Real MTRRs programming are being done after resource allocation.
	 */
	.pre_mp_init = soc_fsp_load,
	.get_cpu_count = get_cpu_count,
	.get_smm_info = smm_info,
	.get_microcode_info = get_microcode_info,
	.adjust_cpu_apic_entry = adjust_apic_id,
	.pre_mp_smm_init = smm_initialize,
	.per_cpu_smm_trigger = per_cpu_smm_trigger,
	.relocation_handler = smm_relocation_handler,
	.post_mp_init = post_mp_init,
};

static void soc_init_cpus(void *unused)
{
	device_t dev = dev_find_path(NULL, DEVICE_PATH_CPU_CLUSTER);
	assert(dev != NULL);
	struct bus *cpu_bus = dev->link_list;

	if (mp_init_with_smm(cpu_bus, &mp_ops))
		printk(BIOS_ERR, "MP initialization failure.\n");

	/* Thermal throttle activation offset */
	configure_thermal_target();

	/*
	 * TODO: somehow calling configure_sgx() in cpu_core_init() is not
	 * successful on the BSP (other threads are fine). Have to run it again
	 * here to get SGX enabled on BSP. This behavior needs to root-caused
	 * and we shall not have this redundant call.
	 */
	configure_sgx(microcode_patch);
}

/* Ensure to re-program all MTRRs based on DRAM resource settings */
static void soc_post_cpus_init(void *unused)
{
	if (mp_run_on_all_cpus(&x86_setup_mtrrs_with_detect, 1000) < 0)
		printk(BIOS_ERR, "MTRR programming failure\n");

	/* Temporarily cache the memory-mapped boot media. */
	if (IS_ENABLED(CONFIG_BOOT_DEVICE_MEMORY_MAPPED) &&
		IS_ENABLED(CONFIG_BOOT_DEVICE_SPI_FLASH))
		fast_spi_cache_bios_region();

	x86_mtrr_check();
}

int soc_skip_ucode_update(u32 current_patch_id, u32 new_patch_id)
{
	msr_t msr1;
	msr_t msr2;

	/*
	 * If PRMRR/SGX is supported the FIT microcode load will set the msr
	 * 0x08b with the Patch revision id one less than the id in the
	 * microcode binary. The PRMRR support is indicated in the MSR
	 * MTRRCAP[12]. If SGX is not enabled, check and avoid reloading the
	 * same microcode during CPU initialization. If SGX is enabled, as
	 * part of SGX BIOS initialization steps, the same microcode needs to
	 * be reloaded after the core PRMRR MSRs are programmed.
	 */
	msr1 = rdmsr(MTRR_CAP_MSR);
	msr2 = rdmsr(PRMRR_PHYS_BASE_MSR);
	if (msr2.lo && (current_patch_id == new_patch_id - 1))
		return 0;
	else
		return (msr1.lo & PRMRR_SUPPORTED) &&
			(current_patch_id == new_patch_id - 1);
}

/* Do CPU MP Init before FSP Silicon Init */
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT_CHIPS, BS_ON_ENTRY, soc_init_cpus, NULL);
BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT, soc_post_cpus_init, NULL);
