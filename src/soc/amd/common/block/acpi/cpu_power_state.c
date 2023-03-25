/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <amdblocks/cppc.h>
#include <amdblocks/cpu.h>
#include <console/console.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/msr.h>
#include <soc/msr.h>
#include <types.h>

static uint32_t get_pstate_core_power(union pstate_msr pstate_reg)
{
	uint32_t voltage_in_uvolts, current_value_amps, current_divisor, power_in_mw;

	/* Get Voltage from core voltage ID */
	voltage_in_uvolts = get_pstate_core_uvolts(pstate_reg);

	/* Current value in amps */
	current_value_amps = pstate_reg.idd_value;

	/* Current divisor */
	current_divisor = pstate_reg.idd_div;

	/* Power in mW */
	power_in_mw = (voltage_in_uvolts) / 10 * current_value_amps;

	switch (current_divisor) {
	case 0:
		power_in_mw = power_in_mw / 100L;
		break;
	case 1:
		power_in_mw = power_in_mw / 1000L;
		break;
	case 2:
		power_in_mw = power_in_mw / 10000L;
		break;
	case 3:
		/* current_divisor is set to an undefined value.*/
		printk(BIOS_WARNING, "Undefined current_divisor set for enabled P-state .\n");
		power_in_mw = 0;
		break;
	}

	return power_in_mw;
}

static uint32_t get_visible_pstate_count(void)
{
	return (rdmsr(PS_LIM_REG).lo & PS_LIM_MAX_VAL_MASK) >> PS_MAX_VAL_SHFT;
}

/*
 * Populate structure describing enabled p-states and return count of enabled p-states.
 */
static size_t get_pstate_info(struct acpi_sw_pstate *pstate_values,
			      struct acpi_xpss_sw_pstate *pstate_xpss_values)
{
	union pstate_msr pstate_reg;
	size_t pstate_count, pstate;
	uint32_t pstate_0_reg, max_pstate, latency;

	pstate_count = 0;
	pstate_0_reg = get_pstate_0_reg();
	max_pstate = get_visible_pstate_count();
	latency = get_pstate_latency();

	for (pstate = 0; pstate <= max_pstate; pstate++) {
		pstate_reg.raw = rdmsr(PSTATE_MSR(pstate_0_reg + pstate)).raw;

		if (!pstate_reg.pstate_en)
			continue;

		pstate_values[pstate_count].core_freq = get_pstate_core_freq(pstate_reg);
		pstate_values[pstate_count].power = get_pstate_core_power(pstate_reg);
		pstate_values[pstate_count].transition_latency = latency;
		pstate_values[pstate_count].bus_master_latency = latency;
		pstate_values[pstate_count].control_value = pstate;
		pstate_values[pstate_count].status_value = pstate;

		pstate_xpss_values[pstate_count].core_freq =
			(uint64_t)pstate_values[pstate_count].core_freq;
		pstate_xpss_values[pstate_count].power =
			(uint64_t)pstate_values[pstate_count].power;
		pstate_xpss_values[pstate_count].transition_latency = latency;
		pstate_xpss_values[pstate_count].bus_master_latency = latency;
		pstate_xpss_values[pstate_count].control_value = (uint64_t)pstate;
		pstate_xpss_values[pstate_count].status_value = (uint64_t)pstate;
		pstate_count++;
	}

	return pstate_count;
}

static uint16_t get_cstate_io_base_address(void)
{
	static uint16_t cstate_io_base;

	if (cstate_io_base)
		return cstate_io_base;

	cstate_io_base = rdmsr(MSR_CSTATE_ADDRESS).lo & MSR_CSTATE_ADDRESS_MASK;

	return cstate_io_base;
}

static void write_cstate_entry(acpi_cstate_t *entry, const acpi_cstate_t *data)
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
			/* ctype is 1-indexed while the offset into the IO address returned by
			   get_cstate_io_base_address() is 0-indexed */
			.addrl = get_cstate_io_base_address() + data->ctype - 1,
			.addrh = 0,
			.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS,
		};
	}
}

static size_t get_cstate_info(acpi_cstate_t *cstate_values)
{
	size_t i;
	size_t cstate_count;
	const acpi_cstate_t *cstate_config = get_cstate_config_data(&cstate_count);

	if (cstate_count > MAX_CSTATE_COUNT) {
		printk(BIOS_WARNING, "cstate_info array has too many entries. "
			"Skipping last %zu entries.\n",
			cstate_count - MAX_CSTATE_COUNT);
		cstate_count = MAX_CSTATE_COUNT;
	}

	for (i = 0; i < cstate_count; i++) {
		write_cstate_entry(&cstate_values[i], &cstate_config[i]);
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
