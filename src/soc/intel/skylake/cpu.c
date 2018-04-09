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
#include <intelblocks/mp_init.h>
#include <intelblocks/sgx.h>
#include <intelblocks/smm.h>
#include <intelblocks/vmx.h>
#include <pc80/mc146818rtc.h>
#include <soc/cpu.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/smm.h>
#include <soc/systemagent.h>
#include <timer.h>

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
	unsigned int tdp, min_power, max_power, max_time, tdp_pl2, tdp_pl1;
	u8 power_limit_1_val;
	struct device *dev = SA_DEV_ROOT;
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
	tdp_pl1 = ((conf->tdp_pl1_override == 0) ?
		tdp : (conf->tdp_pl1_override * power_unit));
	limit.lo |= (tdp_pl1 & PKG_POWER_LIMIT_MASK);

	/* Set PL1 Pkg Power clamp bit */
	limit.lo |= PKG_POWER_LIMIT_CLAMP;

	limit.lo |= PKG_POWER_LIMIT_EN;
	limit.lo |= (power_limit_1_val & PKG_POWER_LIMIT_TIME_MASK) <<
		PKG_POWER_LIMIT_TIME_SHIFT;

	/* Set short term power limit to 1.25 * TDP if no config given */
	limit.hi = 0;
	tdp_pl2 = (conf->tdp_pl2_override == 0) ?
		(tdp * 125) / 100 : (conf->tdp_pl2_override * power_unit);
	printk(BIOS_DEBUG, "CPU PL2 = %u Watts\n", tdp_pl2 / power_unit);
	limit.hi |= (tdp_pl2) & PKG_POWER_LIMIT_MASK;
	limit.hi |= PKG_POWER_LIMIT_CLAMP;
	limit.hi |= PKG_POWER_LIMIT_EN;

	/* Power limit 2 time is only programmable on server SKU */
	wrmsr(MSR_PKG_POWER_LIMIT, limit);

	/* Set PL2 power limit values in MCHBAR and disable PL1 */
	MCHBAR32(MCH_PKG_POWER_LIMIT_LO) = limit.lo & (~(PKG_POWER_LIMIT_EN));
	MCHBAR32(MCH_PKG_POWER_LIMIT_HI) = limit.hi;

	/* Set PsysPl2 */
	if (conf->tdp_psyspl2) {
		limit = rdmsr(MSR_PLATFORM_POWER_LIMIT);
		limit.hi = 0;
		printk(BIOS_DEBUG, "CPU PsysPL2 = %u Watts\n",
			conf->tdp_psyspl2);
		limit.hi |= (conf->tdp_psyspl2 * power_unit) &
			PKG_POWER_LIMIT_MASK;
		limit.hi |= PKG_POWER_LIMIT_CLAMP;
		limit.hi |= PKG_POWER_LIMIT_EN;

		wrmsr(MSR_PLATFORM_POWER_LIMIT, limit);
	}

	/* Set PsysPl3 */
	if (conf->tdp_psyspl3) {
		limit = rdmsr(MSR_PL3_CONTROL);
		limit.lo = 0;
		printk(BIOS_DEBUG, "CPU PsysPL3 = %u Watts\n",
		       conf->tdp_psyspl3);
		limit.lo |= (conf->tdp_psyspl3 * power_unit) &
			PKG_POWER_LIMIT_MASK;
		/* Enable PsysPl3 */
		limit.lo |= PKG_POWER_LIMIT_EN;
		/* set PsysPl3 time window */
		limit.lo |= (conf->tdp_psyspl3_time &
			     PKG_POWER_LIMIT_TIME_MASK) <<
			PKG_POWER_LIMIT_TIME_SHIFT;
		/* set PsysPl3  duty cycle */
		limit.lo |= (conf->tdp_psyspl3_dutycycle &
			     PKG_POWER_LIMIT_DUTYCYCLE_MASK) <<
			PKG_POWER_LIMIT_DUTYCYCLE_SHIFT;
		wrmsr(MSR_PL3_CONTROL, limit);
	}

	/* Set Pl4 */
	if (conf->tdp_pl4) {
		limit = rdmsr(MSR_VR_CURRENT_CONFIG);
		limit.lo = 0;
		printk(BIOS_DEBUG, "CPU PL4 = %u Watts\n",
		       conf->tdp_pl4);
		limit.lo |= (conf->tdp_pl4 * power_unit) &
			PKG_POWER_LIMIT_MASK;
		wrmsr(MSR_VR_CURRENT_CONFIG, limit);
	}

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
	struct device *dev = SA_DEV_ROOT;
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
	struct device *dev = SA_DEV_ROOT;
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
	struct device *dev = SA_DEV_ROOT;
	config_t *conf = dev->chip_info;
	msr_t msr;

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= (1 << 0);	/* Fast String enable */
	msr.lo |= (1 << 3);	/* TM1/TM2/EMTTM enable */
	if (conf->eist_enable)
		msr.lo |= (1 << 16);	/* Enhanced SpeedStep Enable */
	else
		msr.lo &= ~(1 << 16);	/* Enhanced SpeedStep Disable */
	wrmsr(IA32_MISC_ENABLE, msr);

	/* Disable Thermal interrupts */
	msr.lo = 0;
	msr.hi = 0;
	wrmsr(IA32_THERM_INTERRUPT, msr);

	/* Enable package critical interrupt only */
	msr.lo = 1 << 4;
	msr.hi = 0;
	wrmsr(IA32_PACKAGE_THERM_INTERRUPT, msr);

	msr = rdmsr(MSR_POWER_CTL);
	msr.lo |= (1 << 0);	/* Enable Bi-directional PROCHOT as an input*/
	msr.lo &= ~POWER_CTL_C1E_MASK;	/* Disable C1E */
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
void soc_core_init(struct device *cpu)
{
	/* Clear out pending MCEs */
	/* TODO(adurbin): This should only be done on a cold boot. Also, some
	 * of these banks are core vs package scope. For now every CPU clears
	 * every bank. */
	mca_configure(NULL);

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

	/* Configure Core PRMRR for SGX. */
	prmrr_core_configure();
}

static void per_cpu_smm_trigger(void)
{
	/* Relocate the SMM handler. */
	smm_relocate();
}

static void post_mp_init(void)
{
	/* Set Max Ratio */
	cpu_set_max_ratio();

	/*
	 * Now that all APs have been relocated as well as the BSP let SMIs
	 * start flowing.
	 */
	smm_southbridge_enable(GBL_EN);

	/* Lock down the SMRAM space. */
#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
	smm_lock();
#endif

	mp_run_on_all_cpus(vmx_configure, NULL, 2 * USECS_PER_MSEC);

	mp_run_on_all_cpus(sgx_configure, NULL, 14 * USECS_PER_MSEC);
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
	.pre_mp_smm_init = smm_initialize,
	.per_cpu_smm_trigger = per_cpu_smm_trigger,
	.relocation_handler = smm_relocation_handler,
	.post_mp_init = post_mp_init,
};

void soc_init_cpus(struct bus *cpu_bus)
{
	if (mp_init_with_smm(cpu_bus, &mp_ops))
		printk(BIOS_ERR, "MP initialization failure.\n");

	/* Thermal throttle activation offset */
	configure_thermal_target();
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

void cpu_lock_sgx_memory(void)
{
	msr_t msr;

	msr = rdmsr(MSR_LT_LOCK_MEMORY);
	if ((msr.lo & 1) == 0) {
		msr.lo |= 1; /* Lock it */
		wrmsr(MSR_LT_LOCK_MEMORY, msr);
	}
}

int soc_fill_sgx_param(struct sgx_param *sgx_param)
{
	struct device *dev = SA_DEV_ROOT;
	assert(dev != NULL);
	config_t *conf = dev->chip_info;

	if (!conf) {
		printk(BIOS_ERR, "Failed to get chip_info for SGX param\n");
		return -1;
	}

	sgx_param->enable = conf->sgx_enable;
	return 0;
}
int soc_fill_vmx_param(struct vmx_param *vmx_param)
{
	struct device *dev = SA_DEV_ROOT;
	assert(dev != NULL);
	config_t *conf = dev->chip_info;

	if (!conf) {
		printk(BIOS_ERR, "Failed to get chip_info for VMX param\n");
		return -1;
	}

	vmx_param->enable = conf->VmxEnable;
	return 0;
}
