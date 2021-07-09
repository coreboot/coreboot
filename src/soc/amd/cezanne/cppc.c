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
	acpi_addr_t msr = {
		.space_id   = ACPI_ADDRESS_SPACE_FIXED,
		.bit_width  = 8,
		.bit_offset = 0,
		.access_size = ACPI_ACCESS_SIZE_QWORD_ACCESS,
		.addrl      = 0,
		.addrh      = 0,
	};
	static const acpi_addr_t unsupported = {
		.space_id   = ACPI_ADDRESS_SPACE_MEMORY,
		.bit_width  = 0,
		.bit_offset = 0,
		.access_size = ACPI_ACCESS_SIZE_UNDEFINED,
		.addrl      = 0,
		.addrh      = 0,
	};

	config->version = version;

	/*
	 * Highest Performance:
	 */
	msr.addrl = MSR_CPPC_CAPABILITY_1;
	msr.bit_offset = SHIFT_CPPC_CAPABILITY_1_HIGHEST_PERF;
	config->regs[CPPC_HIGHEST_PERF] = msr;

	/*
	 * Lowest Nonlinear Performance -> Most Efficient Performance:
	 */
	msr.bit_offset = SHIFT_CPPC_CAPABILITY_1_LOW_NON_LIN_PERF;
	config->regs[CPPC_LOWEST_NONL_PERF] = msr;

	/*
	 * Lowest Performance:
	 */
	msr.bit_offset = SHIFT_CPPC_CAPABILITY_1_LOWEST_PERF;
	config->regs[CPPC_LOWEST_PERF] = msr;

	/*
	 * Guaranteed Performance Register:
	 */
	config->regs[CPPC_GUARANTEED_PERF] = unsupported;

	/*
	 * Nominal Performance -> Maximum Non-Turbo Ratio:
	 */
	msr.bit_offset = SHIFT_CPPC_CAPABILITY_1_NOMINAL_PERF;
	config->regs[CPPC_NOMINAL_PERF] = msr;

	/*
	 * Desired Performance Register:
	 */
	msr.addrl = MSR_CPPC_REQUEST;
	msr.bit_offset = SHIFT_CPPC_REQUEST_DES_PERF;
	config->regs[CPPC_DESIRED_PERF] = msr;

	/*
	 * Minimum Performance Register:
	 */
	msr.bit_offset = SHIFT_CPPC_REQUEST_MIN_PERF;
	config->regs[CPPC_MIN_PERF] = msr;

	/*
	 * Maximum Performance Register:
	 */
	msr.bit_offset = SHIFT_CPPC_REQUEST_MAX_PERF;
	config->regs[CPPC_MAX_PERF] = msr;

	/*
	 * Performance Reduction Tolerance Register:
	 */
	config->regs[CPPC_PERF_REDUCE_TOLERANCE] = unsupported;

	/*
	 * Time Window Register:
	 */
	config->regs[CPPC_TIME_WINDOW] = unsupported;

	/*
	 * Counter Wraparound Time:
	 */
	config->regs[CPPC_COUNTER_WRAP] = unsupported;

	/*
	 * Reference Performance Counter Register:
	 */
	msr.addrl = MSR_MAX_PERFORMANCE_FREQUENCY_CLOCK_COUNT;
	msr.bit_width = 64;
	msr.bit_offset = 0;
	config->regs[CPPC_REF_PERF_COUNTER] = msr;

	/*
	 * Delivered Performance Counter Register:
	 */
	msr.addrl = MSR_ACTUAL_PERFORMANCE_FREQUENCY_CLOCK_COUNT;
	config->regs[CPPC_DELIVERED_PERF_COUNTER] = msr;

	/*
	 * Performance Limited Register:
	 */
	msr.bit_width = 1;
	msr.addrl = MSR_CPPC_STATUS;
	msr.bit_offset = 1;
	config->regs[CPPC_PERF_LIMITED] = msr;

	/*
	 * CPPC Enable Register:
	 */
	msr.addrl = MSR_CPPC_ENABLE;
	msr.bit_offset = 0;
	config->regs[CPPC_ENABLE] = msr;

	if (version >= 2) {
		/* Autonomous Selection Enable is populated below */

		/*
		 * Autonomous Activity Window Register
		 */
		config->regs[CPPC_AUTO_ACTIVITY_WINDOW] = unsupported;

		/*
		 * Autonomous Energy Performance Preference Register
		 */
		msr.addrl = MSR_CPPC_REQUEST;
		msr.bit_width = 8;
		msr.bit_offset = SHIFT_CPPC_REQUEST_ENERGY_PERF_PREF;
		config->regs[CPPC_PERF_PREF] = msr;

		/* Reference Performance */
		config->regs[CPPC_REF_PERF] = unsupported;

		if (version >= 3) {
			/* Lowest Frequency */
			config->regs[CPPC_LOWEST_FREQ] = unsupported;
			/* Nominal Frequency */
			config->regs[CPPC_NOMINAL_FREQ] = unsupported;
		}

		/*
		 * Autonomous Selection Enable = 1
		 * This field is actually the first addition in version 2 but
		 * it's so unlike the others I'm populating it last.
		 */
		msr.space_id    = ACPI_ADDRESS_SPACE_MEMORY;
		msr.bit_width   = 32;
		msr.bit_offset  = 0;
		msr.access_size = ACPI_ACCESS_SIZE_UNDEFINED;
		msr.addrl       = 1;
		config->regs[CPPC_AUTO_SELECT] = unsupported;
	}
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
