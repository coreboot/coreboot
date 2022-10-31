/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_pm.h>
#include <acpi/acpigen.h>
#include <amdblocks/cppc.h>
#include <cpu/cpu.h>
#include <soc/msr.h>
#include <types.h>

/*
 * version 2 is expected to be the typical use case.
 * For now this function 'punts' on version 3 and just
 * populates the additional fields with 'unsupported'.
 */
static void cpu_init_cppc_config(struct cppc_config *config, u32 version)
{
	uint32_t lowest_freq_mhz;
	uint32_t nominal_freq_mhz;

	config->version = version;

	config->entries[CPPC_HIGHEST_PERF]		= CPPC_REG_MSR(MSR_CPPC_CAPABILITY_1, SHIFT_CPPC_CAPABILITY_1_HIGHEST_PERF, 8);
	config->entries[CPPC_NOMINAL_PERF]		= CPPC_REG_MSR(MSR_CPPC_CAPABILITY_1, SHIFT_CPPC_CAPABILITY_1_NOMINAL_PERF, 8);
	config->entries[CPPC_LOWEST_NONL_PERF]		= CPPC_REG_MSR(MSR_CPPC_CAPABILITY_1, SHIFT_CPPC_CAPABILITY_1_LOW_NON_LIN_PERF, 8);
	config->entries[CPPC_LOWEST_PERF]		= CPPC_REG_MSR(MSR_CPPC_CAPABILITY_1, SHIFT_CPPC_CAPABILITY_1_LOWEST_PERF, 8);
	config->entries[CPPC_GUARANTEED_PERF]		= CPPC_UNSUPPORTED;
	config->entries[CPPC_DESIRED_PERF]		= CPPC_REG_MSR(MSR_CPPC_REQUEST, SHIFT_CPPC_REQUEST_DES_PERF, 8);
	config->entries[CPPC_MIN_PERF]			= CPPC_REG_MSR(MSR_CPPC_REQUEST, SHIFT_CPPC_REQUEST_MIN_PERF, 8);
	config->entries[CPPC_MAX_PERF]			= CPPC_REG_MSR(MSR_CPPC_REQUEST, SHIFT_CPPC_REQUEST_MAX_PERF, 8);
	config->entries[CPPC_PERF_REDUCE_TOLERANCE]	= CPPC_UNSUPPORTED;
	config->entries[CPPC_TIME_WINDOW]		= CPPC_UNSUPPORTED;
	config->entries[CPPC_COUNTER_WRAP]		= CPPC_UNSUPPORTED;
	config->entries[CPPC_REF_PERF_COUNTER]		= CPPC_REG_MSR(MSR_MAX_PERFORMANCE_FREQUENCY_CLOCK_COUNT, 0, 64);
	config->entries[CPPC_DELIVERED_PERF_COUNTER]	= CPPC_REG_MSR(MSR_ACTUAL_PERFORMANCE_FREQUENCY_CLOCK_COUNT, 0, 64);
	config->entries[CPPC_PERF_LIMITED]		= CPPC_REG_MSR(MSR_CPPC_STATUS, 0, 2);
	config->entries[CPPC_ENABLE]			= CPPC_REG_MSR(MSR_CPPC_ENABLE, 0, 1);

	if (version < 2)
		return;

	config->entries[CPPC_AUTO_SELECT]		= CPPC_DWORD(1);
	config->entries[CPPC_AUTO_ACTIVITY_WINDOW]	= CPPC_UNSUPPORTED;
	config->entries[CPPC_PERF_PREF]			= CPPC_REG_MSR(MSR_CPPC_REQUEST, SHIFT_CPPC_REQUEST_ENERGY_PERF_PREF, 8);
	config->entries[CPPC_REF_PERF]			= CPPC_UNSUPPORTED;

	if (version < 3)
		return;

	config->entries[CPPC_LOWEST_FREQ]	=
				get_ccx_cppc_min_frequency(&lowest_freq_mhz) == CB_SUCCESS ?
				CPPC_DWORD(lowest_freq_mhz) : CPPC_UNSUPPORTED;
	config->entries[CPPC_NOMINAL_FREQ]	=
				get_ccx_cppc_nom_frequency(&nominal_freq_mhz) == CB_SUCCESS ?
				CPPC_DWORD(nominal_freq_mhz) : CPPC_UNSUPPORTED;
}

void generate_cppc_entries(unsigned int core_id)
{
	/* Generate GCPC package in first logical core */
	if (core_id == 0) {
		struct cppc_config cppc_config;
		cpu_init_cppc_config(&cppc_config, CPPC_VERSION_3);
		acpigen_write_CPPC_package(&cppc_config);
	}

	/* Write _CPC entry for each logical core */
	acpigen_write_CPPC_method();
}
