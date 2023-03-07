/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <amdblocks/cppc.h>
#include <amdblocks/cpu.h>
#include <console/console.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/msr.h>
#include <types.h>

/*
 * Populate structure describing enabled p-states and return count of enabled p-states.
 */
static size_t get_pstate_info(struct acpi_sw_pstate *pstate_values,
			      struct acpi_xpss_sw_pstate *pstate_xpss_values)
{
	msr_t pstate_def;
	size_t pstate_count, pstate;
	uint32_t pstate_enable, max_pstate;

	pstate_count = 0;
	max_pstate = (rdmsr(PS_LIM_REG).lo & PS_LIM_MAX_VAL_MASK) >> PS_MAX_VAL_SHFT;

	for (pstate = 0; pstate <= max_pstate; pstate++) {
		pstate_def = rdmsr(PSTATE_MSR(pstate));

		pstate_enable = (pstate_def.hi & PSTATE_DEF_HI_ENABLE_MASK)
				>> PSTATE_DEF_HI_ENABLE_SHIFT;
		if (!pstate_enable)
			continue;

		pstate_values[pstate_count].core_freq = get_pstate_core_freq(pstate_def);
		pstate_values[pstate_count].power = get_pstate_core_power(pstate_def);
		pstate_values[pstate_count].transition_latency = 0;
		pstate_values[pstate_count].bus_master_latency = 0;
		pstate_values[pstate_count].control_value = pstate;
		pstate_values[pstate_count].status_value = pstate;

		pstate_xpss_values[pstate_count].core_freq =
			(uint64_t)pstate_values[pstate_count].core_freq;
		pstate_xpss_values[pstate_count].power =
			(uint64_t)pstate_values[pstate_count].power;
		pstate_xpss_values[pstate_count].transition_latency = 0;
		pstate_xpss_values[pstate_count].bus_master_latency = 0;
		pstate_xpss_values[pstate_count].control_value = (uint64_t)pstate;
		pstate_xpss_values[pstate_count].status_value = (uint64_t)pstate;
		pstate_count++;
	}

	return pstate_count;
}

static void write_cstate_entry(acpi_cstate_t *entry, const acpi_cstate_t *data,
			       uint32_t cstate_io_base_address)
{
	if (!data->ctype) {
		printk(BIOS_WARNING, "Invalid C-state data; skipping entry.\n");
		return;
	}

	entry->ctype = data->ctype;
	entry->latency = data->latency;
	entry->power = data->power;

	if (data->ctype == 1) {
		entry->resource = (acpi_addr_t){
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = 2,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.addrl = 0,
			.addrh = 0,
		};
	} else {
		entry->resource = (acpi_addr_t){
			.space_id = ACPI_ADDRESS_SPACE_IO,
			.bit_width = 8,
			.bit_offset = 0,
			/* ctype is 1-indexed while the offset into cstate_io_base_address is
			   0-indexed */
			.addrl = cstate_io_base_address + data->ctype - 1,
			.addrh = 0,
			.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS,
		};
	}
}

static size_t get_cstate_info(acpi_cstate_t *cstate_values)
{
	size_t i;
	size_t cstate_count;
	uint32_t cstate_io_base_address =
		rdmsr(MSR_CSTATE_ADDRESS).lo & MSR_CSTATE_ADDRESS_MASK;
	const acpi_cstate_t *cstate_config = get_cstate_config_data(&cstate_count);

	if (cstate_count > MAX_CSTATE_COUNT) {
		printk(BIOS_WARNING, "cstate_info array has too many entries. "
			"Skipping last %zu entries.\n",
			cstate_count - MAX_CSTATE_COUNT);
		cstate_count = MAX_CSTATE_COUNT;
	}

	for (i = 0; i < cstate_count; i++) {
		write_cstate_entry(&cstate_values[i], &cstate_config[i], cstate_io_base_address);
	}

	return i;
}

void generate_cpu_entries(const struct device *device)
{
	int logical_cores;
	size_t cstate_count, pstate_count, cpu;
	acpi_cstate_t cstate_values[MAX_CSTATE_COUNT] = { {0} };
	struct acpi_sw_pstate pstate_values[MAX_PSTATES] = { {0} };
	struct acpi_xpss_sw_pstate pstate_xpss_values[MAX_PSTATES] = { {0} };
	uint32_t threads_per_core;

	const acpi_addr_t perf_ctrl = {
		.space_id = ACPI_ADDRESS_SPACE_FIXED,
		.bit_width = 64,
		.addrl = PS_CTL_REG,
	};
	const acpi_addr_t perf_sts = {
		.space_id = ACPI_ADDRESS_SPACE_FIXED,
		.bit_width = 64,
		.addrl = PS_STS_REG,
	};

	threads_per_core = get_threads_per_core();
	cstate_count = get_cstate_info(cstate_values);
	pstate_count = get_pstate_info(pstate_values, pstate_xpss_values);
	logical_cores = get_cpu_count();

	for (cpu = 0; cpu < logical_cores; cpu++) {
		acpigen_write_processor_device(cpu);

		acpigen_write_pct_package(&perf_ctrl, &perf_sts);

		acpigen_write_pss_object(pstate_values, pstate_count);

		acpigen_write_xpss_object(pstate_xpss_values, pstate_count);

		if (CONFIG(ACPI_SSDT_PSD_INDEPENDENT))
			acpigen_write_PSD_package(cpu / threads_per_core, threads_per_core,
						  HW_ALL);
		else
			acpigen_write_PSD_package(0, logical_cores, SW_ALL);

		acpigen_write_PPC(0);

		acpigen_write_CST_package(cstate_values, cstate_count);

		acpigen_write_CSD_package(cpu / threads_per_core, threads_per_core,
					  CSD_HW_ALL, 0);

		if (CONFIG(SOC_AMD_COMMON_BLOCK_ACPI_CPPC))
			generate_cppc_entries(cpu);

		acpigen_write_processor_device_end();
	}

	acpigen_write_processor_package("PPKG", 0, logical_cores);
}
