/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2018 Intel Corporation.
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

#include <arch/acpigen.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <delay.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/fast_spi.h>
#include <lib.h>
#include <reset.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/pm.h>
#include <intelblocks/msr.h>
#include <soc/pci_devs.h>
#include <stdint.h>

/*
 * Set PERF_CTL MSR (0x199) P_Req (14:8 bits) with
 * Turbo Ratio which is the Maximum Ratio.
 */
void cpu_set_max_ratio(void)
{
	/* Check for configurable TDP option */
	if (get_turbo_state() == TURBO_ENABLED)
		cpu_set_p_state_to_turbo_ratio();
}

/*
 * Get the TDP Nominal Ratio from MSR 0x648 Bits 7:0.
 */
u8 cpu_get_tdp_nominal_ratio(void)
{
	u8 nominal_ratio;
	msr_t msr;

	msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
	nominal_ratio = msr.lo & 0xff;
	return nominal_ratio;
}

/*
 * Read PLATFORM_INFO MSR (0xCE).
 * Return Value of Bit 34:33 (CONFIG_TDP_LEVELS).
 *
 * Possible values of Bit 34:33 are -
 * 00 : Config TDP not supported
 * 01 : One Additional TDP level supported
 * 10 : Two Additional TDP level supported
 * 11 : Reserved
 */
int cpu_config_tdp_levels(void)
{
	msr_t platform_info;

	/* Bits 34:33 indicate how many levels supported */
	platform_info = rdmsr(MSR_PLATFORM_INFO);
	return (platform_info.hi >> 1) & 3;
}

/*
 * TURBO_RATIO_LIMIT MSR (0x1AD) Bits 31:0 indicates the
 * factory configured values for of 1-core, 2-core, 3-core
 * and 4-core turbo ratio limits for all processors.
 *
 * 7:0 -	MAX_TURBO_1_CORE
 * 15:8 -	MAX_TURBO_2_CORES
 * 23:16 -	MAX_TURBO_3_CORES
 * 31:24 -	MAX_TURBO_4_CORES
 *
 * Set PERF_CTL MSR (0x199) P_Req (14:8 bits) with that value.
 */
void cpu_set_p_state_to_turbo_ratio(void)
{
	msr_t msr, perf_ctl;

	msr = rdmsr(MSR_TURBO_RATIO_LIMIT);
	perf_ctl.lo = (msr.lo & 0xff) << 8;
	perf_ctl.hi = 0;

	wrmsr(MSR_IA32_PERF_CTL, perf_ctl);
	printk(BIOS_DEBUG, "CPU: frequency set to %d MHz\n",
	       ((perf_ctl.lo >> 8) & 0xff) * CONFIG_CPU_BCLK_MHZ);
}

/*
 * CONFIG_TDP_NOMINAL MSR (0x648) Bits 7:0 tells Nominal
 * TDP level ratio to be used for specific processor (in units
 * of 100MHz).
 *
 * Set PERF_CTL MSR (0x199) P_Req (14:8 bits) with that value.
 */
void cpu_set_p_state_to_nominal_tdp_ratio(void)
{
	msr_t msr, perf_ctl;

	msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
	perf_ctl.lo = (msr.lo & 0xff) << 8;
	perf_ctl.hi = 0;

	wrmsr(MSR_IA32_PERF_CTL, perf_ctl);
	printk(BIOS_DEBUG, "CPU: frequency set to %d MHz\n",
		((perf_ctl.lo >> 8) & 0xff) * CONFIG_CPU_BCLK_MHZ);
}

/*
 * PLATFORM_INFO MSR (0xCE) Bits 15:8 tells
 * MAX_NON_TURBO_LIM_RATIO.
 *
 * Set PERF_CTL MSR (0x199) P_Req (14:8 bits) with that value.
 */
void cpu_set_p_state_to_max_non_turbo_ratio(void)
{
	msr_t msr, perf_ctl;

	/* Platform Info bits 15:8 give max ratio */
	msr = rdmsr(MSR_PLATFORM_INFO);
	perf_ctl.lo = msr.lo & 0xff00;
	perf_ctl.hi = 0;

	wrmsr(MSR_IA32_PERF_CTL, perf_ctl);
	printk(BIOS_DEBUG, "CPU: frequency set to %d MHz\n",
		((perf_ctl.lo >> 8) & 0xff) * CONFIG_CPU_BCLK_MHZ);
}

/*
 * Get the Burst/Turbo Mode State from MSR IA32_MISC_ENABLE 0x1A0
 * Bit 38 - TURBO_MODE_DISABLE Bit to get state ENABLED / DISABLED.
 * Also check for the cpuid 0x6 to check whether Burst mode unsupported.
 */
int cpu_get_burst_mode_state(void)
{

	msr_t msr;
	unsigned int eax;
	int burst_en, burst_cap, burst_state = BURST_MODE_UNKNOWN;

	eax = cpuid_eax(0x6);
	burst_cap = eax & 0x2;
	msr = rdmsr(IA32_MISC_ENABLE);
	burst_en = !(msr.hi & BURST_MODE_DISABLE);

	if (!burst_cap && burst_en) {
		burst_state = BURST_MODE_UNAVAILABLE;
	} else if (burst_cap && !burst_en) {
		burst_state = BURST_MODE_DISABLED;
	} else if (burst_cap && burst_en) {
		burst_state = BURST_MODE_ENABLED;
	}
	return burst_state;
}

/*
 * Enable Burst mode.
 */
void cpu_enable_burst_mode(void)
{
	msr_t msr;

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.hi &= ~BURST_MODE_DISABLE;
	wrmsr(IA32_MISC_ENABLE, msr);
}

/*
 * Disable Burst mode.
 */
void cpu_disable_burst_mode(void)
{
	msr_t msr;

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.hi |= BURST_MODE_DISABLE;
	wrmsr(IA32_MISC_ENABLE, msr);
}

/*
 * Enable Intel Enhanced Speed Step Technology.
 */
void cpu_enable_eist(void)
{
	msr_t msr;

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= (1 << 16);	/* Enhanced SpeedStep Enable */
	wrmsr(IA32_MISC_ENABLE, msr);
}

/*
 * Disable Intel Enhanced Speed Step Technology.
 */
void cpu_disable_eist(void)
{
	msr_t msr;

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo &= ~(1 << 16);	/* Enhanced SpeedStep Disable */
	wrmsr(IA32_MISC_ENABLE, msr);
}

/*
 * Set Bit 6 (ENABLE_IA_UNTRUSTED_MODE) of MSR 0x120
 * UCODE_PCR_POWER_MISC MSR to enter IA Untrusted Mode.
 */
void cpu_enable_untrusted_mode(void *unused)
{
	msr_t msr;

	msr = rdmsr(MSR_POWER_MISC);
	msr.lo |= ENABLE_IA_UNTRUSTED;
	wrmsr(MSR_POWER_MISC, msr);
}

/*
 * This function fills in the number of Cores(physical) and Threads(virtual)
 * of the CPU in the function arguments. It also returns if the number of cores
 * and number of threads are equal.
 */
int cpu_read_topology(unsigned int *num_phys, unsigned int *num_virt)
{
	msr_t msr;
	msr = rdmsr(MSR_CORE_THREAD_COUNT);
	*num_virt = (msr.lo >> 0) & 0xffff;
	*num_phys = (msr.lo >> 16) & 0xffff;
	return (*num_virt == *num_phys);
}

int cpu_get_coord_type(void)
{
	return HW_ALL;
}

uint32_t cpu_get_min_ratio(void)
{
	msr_t msr;
	/* Get bus ratio limits and calculate clock speeds */
	msr = rdmsr(MSR_PLATFORM_INFO);
	return ((msr.hi >> 8) & 0xff);	/* Max Efficiency Ratio */
}

uint32_t cpu_get_max_ratio(void)
{
	msr_t msr;
	uint32_t ratio_max;
	if (cpu_config_tdp_levels()) {
		/* Set max ratio to nominal TDP ratio */
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		ratio_max = msr.lo & 0xff;
	} else {
		msr = rdmsr(MSR_PLATFORM_INFO);
		/* Max Non-Turbo Ratio */
		ratio_max = (msr.lo >> 8) & 0xff;
	}
	return ratio_max;
}

uint32_t cpu_get_bus_clock(void)
{
	/* CPU bus clock is set by default here to 100MHz.
	 * This function returns the bus clock in KHz.
	 */
	return CONFIG_CPU_BCLK_MHZ * KHz;
}

uint32_t cpu_get_power_max(void)
{
	msr_t msr;
	int power_unit;

	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 2 << ((msr.lo & 0xf) - 1);
	msr = rdmsr(MSR_PKG_POWER_SKU);
	return (msr.lo & 0x7fff) * 1000 / power_unit;
}

uint32_t cpu_get_max_turbo_ratio(void)
{
	msr_t msr;
	msr = rdmsr(MSR_TURBO_RATIO_LIMIT);
	return msr.lo & 0xff;
}

void mca_configure(void *unused)
{
	msr_t msr;
	int i;
	int num_banks;

	msr = rdmsr(IA32_MCG_CAP);
	num_banks = msr.lo & 0xff;
	msr.lo = msr.hi = 0;

	for (i = 0; i < num_banks; i++) {
		/* Clear the machine check status */
		wrmsr(IA32_MC0_STATUS + (i * 4), msr);
		/* Initialize machine checks */
		wrmsr(IA32_MC0_CTL + i * 4,
			(msr_t) {.lo = 0xffffffff, .hi = 0xffffffff});
	}
}
