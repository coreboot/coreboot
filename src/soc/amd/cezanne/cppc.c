/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_pm.h>
#include <acpi/acpigen.h>
#include <arch/cpu.h>
#include <soc/cppc.h>
#include <soc/msr.h>

/*
 * version 2 is expected to be the typical use case.
 * For now this function 'punts' on version 3 and just
 * populates the additional fields with 'unsupported'.
 */
void cpu_init_cppc_config(struct cppc_config *config, u32 version)
{
	config->version = version;

	config->regs[CPPC_HIGHEST_PERF]			= ACPI_REG_MSR(MSR_CPPC_CAPABILITY_1, SHIFT_CPPC_CAPABILITY_1_HIGHEST_PERF, 8);
	config->regs[CPPC_NOMINAL_PERF]			= ACPI_REG_MSR(MSR_CPPC_CAPABILITY_1, SHIFT_CPPC_CAPABILITY_1_NOMINAL_PERF, 8);
	config->regs[CPPC_LOWEST_NONL_PERF]		= ACPI_REG_MSR(MSR_CPPC_CAPABILITY_1, SHIFT_CPPC_CAPABILITY_1_LOW_NON_LIN_PERF, 8);
	config->regs[CPPC_LOWEST_PERF]			= ACPI_REG_MSR(MSR_CPPC_CAPABILITY_1, SHIFT_CPPC_CAPABILITY_1_LOWEST_PERF, 8);
	config->regs[CPPC_GUARANTEED_PERF]		= ACPI_REG_UNSUPPORTED;
	config->regs[CPPC_DESIRED_PERF]			= ACPI_REG_MSR(MSR_CPPC_REQUEST, SHIFT_CPPC_REQUEST_DES_PERF, 8);
	config->regs[CPPC_MIN_PERF]			= ACPI_REG_MSR(MSR_CPPC_REQUEST, SHIFT_CPPC_REQUEST_MIN_PERF, 8);
	config->regs[CPPC_MAX_PERF]			= ACPI_REG_MSR(MSR_CPPC_REQUEST, SHIFT_CPPC_REQUEST_MAX_PERF, 8);
	config->regs[CPPC_PERF_REDUCE_TOLERANCE]	= ACPI_REG_UNSUPPORTED;
	config->regs[CPPC_TIME_WINDOW]			= ACPI_REG_UNSUPPORTED;
	config->regs[CPPC_COUNTER_WRAP]			= ACPI_REG_UNSUPPORTED;
	config->regs[CPPC_REF_PERF_COUNTER]		= ACPI_REG_MSR(MSR_MAX_PERFORMANCE_FREQUENCY_CLOCK_COUNT, 0, 64);
	config->regs[CPPC_DELIVERED_PERF_COUNTER]	= ACPI_REG_MSR(MSR_ACTUAL_PERFORMANCE_FREQUENCY_CLOCK_COUNT, 0, 64);
	config->regs[CPPC_PERF_LIMITED]			= ACPI_REG_MSR(MSR_CPPC_STATUS, 1, 1);
	config->regs[CPPC_ENABLE]			= ACPI_REG_MSR(MSR_CPPC_ENABLE, 0, 1);

	if (version < 2)
		return;

	config->regs[CPPC_AUTO_SELECT]			= ACPI_REG_UNSUPPORTED;
	config->regs[CPPC_AUTO_ACTIVITY_WINDOW]		= ACPI_REG_UNSUPPORTED;
	config->regs[CPPC_PERF_PREF]			= ACPI_REG_MSR(MSR_CPPC_REQUEST, SHIFT_CPPC_REQUEST_ENERGY_PERF_PREF, 8);
	config->regs[CPPC_REF_PERF]			= ACPI_REG_UNSUPPORTED;

	if (version < 3)
		return;

	config->regs[CPPC_LOWEST_FREQ]			= ACPI_REG_UNSUPPORTED;
	config->regs[CPPC_NOMINAL_FREQ]			= ACPI_REG_UNSUPPORTED;
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
