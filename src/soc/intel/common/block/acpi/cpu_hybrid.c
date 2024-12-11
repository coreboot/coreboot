/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <bootstate.h>
#include <commonlib/sort.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>
#include <cpu/cpu.h>
#include <device/path.h>
#include <intelblocks/acpi.h>
#include <soc/cpu.h>
#include <stdio.h>
#include <types.h>

#define CPPC_NOM_FREQ_IDX	22
#define CPPC_NOM_PERF_IDX	3

struct cpu_apic_info_type {
	/*
	 * Ordered APIC IDs based on core type.
	 * Array begins with Performance Cores' APIC IDs,
	 * then followed by Efficeint Cores's APIC IDs.
	 */
	int32_t apic_ids[CONFIG_MAX_CPUS];

	/* Total CPU count */
	uint16_t total_cpu_cnt;

	/*
	 * Total Performance core count. This will be used
	 * to identify the start of Efficient Cores's
	 * APIC ID list
	 */
	uint16_t perf_cpu_cnt;
};

static struct cpu_apic_info_type cpu_apic_info;

/*
 * The function orders APIC IDs such that orders first Performance cores and then
 * Efficient cores' APIC IDs in ascending order. Also calculates total number of
 * Performance cores and all cores count in the system and populates the information
 * in the cpu_apic_info sturct.
 */
static void acpi_set_hybrid_cpu_apicid_order(void *unused)
{
	size_t perf_core_cnt = 0, eff_core_cnt = 0;
	int32_t eff_apic_ids[CONFIG_MAX_CPUS] = {0};
	extern struct cpu_info cpu_infos[];
	uint32_t i, j = 0;

	for (i = 0; i < ARRAY_SIZE(cpu_apic_info.apic_ids); i++) {
		if (!cpu_infos[i].cpu)
			continue;
		if (cpu_infos[i].cpu->path.apic.core_type == CPU_TYPE_PERF)
			cpu_apic_info.apic_ids[perf_core_cnt++] =
				cpu_infos[i].cpu->path.apic.apic_id;
		else
			eff_apic_ids[eff_core_cnt++] =
				cpu_infos[i].cpu->path.apic.apic_id;
	}

	if (perf_core_cnt > 1)
		bubblesort(cpu_apic_info.apic_ids, perf_core_cnt, NUM_ASCENDING);

	for (i = perf_core_cnt; j < eff_core_cnt; i++, j++)
		cpu_apic_info.apic_ids[i] = eff_apic_ids[j];

	if (eff_core_cnt > 1)
		bubblesort(&cpu_apic_info.apic_ids[perf_core_cnt], eff_core_cnt, NUM_ASCENDING);

	/* Populate total core count */
	cpu_apic_info.total_cpu_cnt = perf_core_cnt + eff_core_cnt;

	cpu_apic_info.perf_cpu_cnt = perf_core_cnt;
}

static unsigned long acpi_create_madt_lapics_hybrid(unsigned long current)
{
	size_t index;

	for (index = 0; index < cpu_apic_info.total_cpu_cnt; index++)
		current = acpi_create_madt_one_lapic(current, index,
						     cpu_apic_info.apic_ids[index]);

	return current;
}

unsigned long acpi_create_madt_lapics_with_nmis_hybrid(unsigned long current)
{
	current = acpi_create_madt_lapics_hybrid(current);
	current = acpi_create_madt_lapic_nmis(current);
	return current;
}

static enum cpu_perf_eff_type get_core_type(void)
{
	return (get_soc_cpu_type() == CPUID_CORE_TYPE_INTEL_CORE) ?
		CPU_TYPE_PERF : CPU_TYPE_EFF;
}

void set_dev_core_type(void)
{
	struct cpu_info *info = cpu_info();
	info->cpu->path.apic.core_type = get_core_type();
}

static void acpi_get_cpu_nomi_perf(u16 *eff_core_nom_perf, u16 *perf_core_nom_perf)
{
	u8 max_non_turbo_ratio = cpu_get_max_non_turbo_ratio();
	static u16 performance, efficient;

	_Static_assert(CONFIG(SOC_INTEL_COMMON_BLOCK_RUNTIME_CORE_SCALING_FACTORS) ||
		       CONFIG_SOC_INTEL_PERFORMANCE_CORE_SCALE_FACTOR != 0,
		       "CONFIG_SOC_INTEL_PERFORMANCE_CORE_SCALE_FACTOR must not be zero");

	_Static_assert(CONFIG(SOC_INTEL_COMMON_BLOCK_RUNTIME_CORE_SCALING_FACTORS) ||
		       CONFIG_SOC_INTEL_EFFICIENT_CORE_SCALE_FACTOR != 0,
		       "CONFIG_SOC_INTEL_EFFICIENT_CORE_SCALE_FACTOR must not be zero");

	if (!performance) {
		if (CONFIG(SOC_INTEL_COMMON_BLOCK_RUNTIME_CORE_SCALING_FACTORS)) {
			soc_read_core_scaling_factors(&performance, &efficient);
		} else {
			performance = CONFIG_SOC_INTEL_PERFORMANCE_CORE_SCALE_FACTOR;
			efficient = CONFIG_SOC_INTEL_EFFICIENT_CORE_SCALE_FACTOR;
		}
	}

	*perf_core_nom_perf = (u16)((max_non_turbo_ratio * performance) / 100);
	*eff_core_nom_perf = (u16)((max_non_turbo_ratio * efficient) / 100);
}

static u16 acpi_get_cpu_nominal_freq(void)
{
	return cpu_get_max_non_turbo_ratio() * cpu_get_bus_frequency();
}

/* Updates Nominal Frequency and Nominal Performance */
static void acpigen_cppc_update_nominal_freq_perf(const char *pkg_path, s32 core_id)
{
	u16 eff_core_nom_perf, perf_core_nom_perf;

	if (!soc_is_nominal_freq_supported())
		return;

	acpi_get_cpu_nomi_perf(&eff_core_nom_perf, &perf_core_nom_perf);

	if (core_id < cpu_apic_info.perf_cpu_cnt)
		acpigen_set_package_element_int(pkg_path, CPPC_NOM_PERF_IDX, perf_core_nom_perf);
	else
		acpigen_set_package_element_int(pkg_path, CPPC_NOM_PERF_IDX,
						eff_core_nom_perf);

	/* Update CPU's nominal frequency */
	acpigen_set_package_element_int(pkg_path, CPPC_NOM_FREQ_IDX,
					acpi_get_cpu_nominal_freq());
}

void acpigen_write_CPPC_hybrid_method(s32 core_id)
{
	char pkg_path[16];

	if (core_id == 0)
		snprintf(pkg_path, sizeof(pkg_path), CPPC_PACKAGE_NAME, 0);
	else
		snprintf(pkg_path, sizeof(pkg_path),
			 "\\_SB." CONFIG_ACPI_CPU_STRING "." CPPC_PACKAGE_NAME, 0);

	acpigen_write_method("_CPC", 0);

	/* Update nominal performance and nominal frequency */
	acpigen_cppc_update_nominal_freq_perf(pkg_path, core_id);
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring(pkg_path);
	acpigen_pop_len();
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT_CHIPS, BS_ON_EXIT, acpi_set_hybrid_cpu_apicid_order, NULL);
