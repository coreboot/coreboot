/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <console/console.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/msr.h>
#include <arch/cpu.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/fast_spi.h>
#include <intelblocks/msr.h>
#include <soc/soc_chip.h>
#include <types.h>

/*
 * Set PERF_CTL MSR (0x199) P_Req with
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

static void set_perf_control_msr(msr_t msr)
{
	wrmsr(IA32_PERF_CTL, msr);
	printk(BIOS_DEBUG, "CPU: frequency set to %d MHz\n",
	       ((msr.lo >> 8) & 0xff) * CONFIG_CPU_BCLK_MHZ);
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
 * Set PERF_CTL MSR (0x199) P_Req with that value.
 */
void cpu_set_p_state_to_turbo_ratio(void)
{
	msr_t msr, perf_ctl;

	msr = rdmsr(MSR_TURBO_RATIO_LIMIT);
	perf_ctl.lo = (msr.lo & 0xff) << 8;
	perf_ctl.hi = 0;

	set_perf_control_msr(perf_ctl);
}

/*
 * CONFIG_TDP_NOMINAL MSR (0x648) Bits 7:0 tells Nominal
 * TDP level ratio to be used for specific processor (in units
 * of 100MHz).
 *
 * Set PERF_CTL MSR (0x199) P_Req with that value.
 */
void cpu_set_p_state_to_nominal_tdp_ratio(void)
{
	msr_t msr, perf_ctl;

	msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
	perf_ctl.lo = (msr.lo & 0xff) << 8;
	perf_ctl.hi = 0;

	set_perf_control_msr(perf_ctl);
}

/*
 * PLATFORM_INFO MSR (0xCE) Bits 15:8 tells
 * MAX_NON_TURBO_LIM_RATIO.
 *
 * Set PERF_CTL MSR (0x199) P_Req with that value.
 */
void cpu_set_p_state_to_max_non_turbo_ratio(void)
{
	msr_t msr, perf_ctl;

	/* Platform Info bits 15:8 give max ratio */
	msr = rdmsr(MSR_PLATFORM_INFO);
	perf_ctl.lo = msr.lo & 0xff00;
	perf_ctl.hi = 0;

	set_perf_control_msr(perf_ctl);
}

/*
 * Set PERF_CTL MSR (0x199) P_Req with the value
 * for maximum efficiency. This value is reported in PLATFORM_INFO MSR (0xCE)
 * in Bits 47:40 and is extracted with cpu_get_min_ratio().
 */
void cpu_set_p_state_to_min_clock_ratio(void)
{
	uint32_t min_ratio;
	msr_t perf_ctl;

	/* Read the minimum ratio for the best efficiency. */
	min_ratio = cpu_get_min_ratio();
	perf_ctl.lo = (min_ratio << 8) & 0xff00;
	perf_ctl.hi = 0;

	set_perf_control_msr(perf_ctl);
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
 * Program CPU Burst mode
 * true = Enable Burst mode.
 * false = Disable Burst mode.
 */
void cpu_burst_mode(bool burst_mode_status)
{
	msr_t msr;

	msr = rdmsr(IA32_MISC_ENABLE);
	if (burst_mode_status)
		msr.hi &= ~BURST_MODE_DISABLE;
	else
		msr.hi |= BURST_MODE_DISABLE;
	wrmsr(IA32_MISC_ENABLE, msr);
}

/*
 * Program Enhanced Intel Speed Step Technology
 * true = Enable EIST.
 * false = Disable EIST.
 */
void cpu_set_eist(bool eist_status)
{
	msr_t msr;

	msr = rdmsr(IA32_MISC_ENABLE);
	if (eist_status)
		msr.lo |= (1 << 16);
	else
		msr.lo &= ~(1 << 16);
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

void configure_tcc_thermal_target(void)
{
	const config_t *conf = config_of_soc();
	msr_t msr;

	if (!conf->tcc_offset)
		return;

	/* Set TCC activation offset */
	msr = rdmsr(MSR_PLATFORM_INFO);
	if ((msr.lo & BIT(30))) {
		msr = rdmsr(MSR_TEMPERATURE_TARGET);
		msr.lo &= ~(0xf << 24);
		msr.lo |= (conf->tcc_offset & 0xf) << 24;
		wrmsr(MSR_TEMPERATURE_TARGET, msr);
	}

	/*
	 * SoCs prior to Comet Lake/Cannon Lake do not support the time window
	 * bits, so return early.
	 */
	if (CONFIG(SOC_INTEL_APOLLOLAKE) || CONFIG(SOC_INTEL_SKYLAKE) ||
	    CONFIG(SOC_INTEL_KABYLAKE) || CONFIG(SOC_INTEL_BRASWELL) ||
	    CONFIG(SOC_INTEL_BROADWELL))
		return;

	/* Time Window Tau Bits [6:0] */
	msr = rdmsr(MSR_TEMPERATURE_TARGET);
	msr.lo &= ~0x7f;
	msr.lo |= 0xe6; /* setting 100ms thermal time window */
	wrmsr(MSR_TEMPERATURE_TARGET, msr);
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

void mca_configure(void)
{
	msr_t msr;
	int i;
	int num_banks;

	printk(BIOS_DEBUG, "Clearing out pending MCEs\n");

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

void cpu_lt_lock_memory(void *unused)
{
	msr_set(MSR_LT_CONTROL, LT_CONTROL_LOCK);
}

int get_valid_prmrr_size(void)
{
	msr_t msr;
	int i;
	int valid_size;

	if (!CONFIG(SOC_INTEL_COMMON_BLOCK_SGX_ENABLE))
		return 0;

	msr = rdmsr(MSR_PRMRR_VALID_CONFIG);
	if (!msr.lo) {
		printk(BIOS_WARNING, "PRMRR not supported.\n");
		return 0;
	}

	printk(BIOS_DEBUG, "MSR_PRMRR_VALID_CONFIG = 0x%08x\n", msr.lo);

	/* find the first (greatest) value that is lower than or equal to the selected size */
	for (i = 8; i >= 0; i--) {
		valid_size = msr.lo & (1 << i);

		if (valid_size && valid_size <= CONFIG_SOC_INTEL_COMMON_BLOCK_SGX_PRMRR_SIZE)
			break;
		else if (i == 0)
			valid_size = 0;
	}

	if (!valid_size) {
		printk(BIOS_WARNING, "Unsupported PRMRR size of %i MiB, check your config!\n",
			CONFIG_SOC_INTEL_COMMON_BLOCK_SGX_PRMRR_SIZE);
		return 0;
	}

	printk(BIOS_DEBUG, "PRMRR size set to %i MiB\n", valid_size);

	valid_size *= MiB;

	return valid_size;
}
