/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_CPULIB_H
#define SOC_INTEL_COMMON_BLOCK_CPULIB_H

#include <types.h>

/*
 * Set PERF_CTL MSR (0x199) P_Req with
 * Turbo Ratio which is the Maximum Ratio.
 */
void cpu_set_max_ratio(void);

/* Get CPU bus frequency in MHz */
u32 cpu_get_bus_frequency(void);

/* Get CPU's max non-turbo ratio */
u8 cpu_get_max_non_turbo_ratio(void);

/* Check if CPU is hybrid CPU or not */
bool cpu_is_hybrid_supported(void);

/*
 * Returns type of CPU that executing the function. It returns 0x20
 * if CPU is atom, otherwise 0x40 if CPU is CORE. The API must be called
 * if CPU is hybrid.
 */
uint8_t cpu_get_cpu_type(void);
/*
 * Get the TDP Nominal Ratio from MSR 0x648 Bits 7:0.
 */
u8 cpu_get_tdp_nominal_ratio(void);

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
int cpu_config_tdp_levels(void);

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
void cpu_set_p_state_to_turbo_ratio(void);

/*
 * CONFIG_TDP_NOMINAL MSR (0x648) Bits 7:0 tells Nominal
 * TDP level ratio to be used for specific processor (in units
 * of 100MHz).
 *
 * Set PERF_CTL MSR (0x199) P_Req with that value.
 */
void cpu_set_p_state_to_nominal_tdp_ratio(void);

/*
 * PLATFORM_INFO MSR (0xCE) Bits 15:8 tells
 * MAX_NON_TURBO_LIM_RATIO.
 *
 * Set PERF_CTL MSR (0x199) P_Req with that value.
 */
void cpu_set_p_state_to_max_non_turbo_ratio(void);

/*
 * Set PERF_CTL MSR (0x199) P_Req with the value
 * for maximum efficiency. This value is reported in PLATFORM_INFO MSR (0xCE)
 * in Bits 47:40 and is extracted with cpu_get_min_ratio().
 */
void cpu_set_p_state_to_min_clock_ratio(void);

/*
 * Get the Burst/Turbo Mode State from MSR IA32_MISC_ENABLE 0x1A0
 * Bit 38 - TURBO_MODE_DISABLE Bit to get state ENABLED / DISABLED.
 * Also check for the cpuid 0x6 to check whether Burst mode unsupported.
 * Below are the possible cpu_get_burst_mode_state() return values-
 * These states are exposed to the User since user
 * need to know which is the current Burst Mode State.
 */
enum {
	BURST_MODE_UNKNOWN,
	BURST_MODE_UNAVAILABLE,
	BURST_MODE_DISABLED,
	BURST_MODE_ENABLED
};
int cpu_get_burst_mode_state(void);

/*
 * Program CPU Burst mode
 * true = Enable Burst mode.
 * false = Disable Burst mode.
 */
void cpu_burst_mode(bool burst_mode_status);

/*
 * Program Enhanced Intel Speed Step Technology
 * true = Enable EIST.
 * false = Disable EIST.
 */
void cpu_set_eist(bool eist_status);

/*
 * SoC specific implementation:
 *
 * Check CPU security level using ENABLE_IA_UNTRUSTED_MODE of CPU MSR.
 * If bit is set, meaning CPU has dropped its security level by entering
 * into `untrusted mode`. Otherwise, it's in `trusted mode`.
 */
bool cpu_soc_is_in_untrusted_mode(void);

/* SoC function to set the BIOS DONE MSR. */
void cpu_soc_bios_done(void);

/*
 * This function fills in the number of Cores(physical) and Threads(virtual)
 * of the CPU in the function arguments. It also returns if the number of cores
 * and number of threads are equal.
 */
int cpu_read_topology(unsigned int *num_phys, unsigned int *num_virt);

/*
 * cpu_get_bus_clock returns the bus clock frequency in KHz.
 * This is the value the clock ratio is multiplied with.
 */
uint32_t cpu_get_bus_clock(void);

/*
 * cpu_get_coord_type returns coordination type (SW_ANY or SW_ALL or HW_ALL)
 * which is used to populate _PSD object.
 */
int cpu_get_coord_type(void);

/*
 * cpu_get_min_ratio returns the minimum frequency ratio that is supported
 * by this processor
 */
uint32_t cpu_get_min_ratio(void);

/*
 * cpu_get_max_ratio returns the nominal TDP ratio if available or the
 * maximum non turbo frequency ratio for this processor
 */
uint32_t cpu_get_max_ratio(void);

/* Thermal throttle activation offset */
void configure_tcc_thermal_target(void);

/*
 * cpu_get_power_max calculates CPU TDP in mW
 */
uint32_t cpu_get_power_max(void);

/*
 * cpu_get_max_turbo_ratio returns the maximum turbo ratio limit for the
 * processor
 */
uint32_t cpu_get_max_turbo_ratio(void);

/* Configure Machine Check Architecture support */
void mca_configure(void);

/* Lock chipset memory registers to protect SMM */
void cpu_lt_lock_memory(void);

/* Get a supported PRMRR size in bytes with respect to users choice */
int get_valid_prmrr_size(void);

/*
 * Enable the emulated ACPI timer in case it's not available or to allow
 * disabling the PM ACPI timer (PM1_TMR) for power saving.
 */
void enable_pm_timer_emulation(void);

/* Derive core, package and thread information from lapic ID */
void get_cpu_topology_from_apicid(uint32_t apicid, uint8_t *package,
		uint8_t *core, uint8_t *thread);

#endif	/* SOC_INTEL_COMMON_BLOCK_CPULIB_H */
