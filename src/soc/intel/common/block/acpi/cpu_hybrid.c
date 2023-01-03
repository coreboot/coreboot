/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <acpi/acpigen.h>
#include <intelblocks/acpi.h>
#include <soc/cpu.h>
#include <smp/spinlock.h>
#include <device/device.h>
#include <device/path.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>

#define CPPC_NOM_FREQ_IDX	22
#define CPPC_NOM_PERF_IDX	3

enum cpu_perf_eff_type {
	CPU_TYPE_EFF,
	CPU_TYPE_PERF,
};

DECLARE_SPIN_LOCK(cpu_lock);
static u8 global_cpu_type[CONFIG_MAX_CPUS];

static bool is_perf_core(void)
{
	return get_soc_cpu_type() == CPUID_CORE_TYPE_INTEL_CORE;
}

static u32 get_cpu_index(void)
{
	u32 cpu_index = 0;
	struct device *dev;
	u32 my_apic_id = lapicid();

	for (dev = dev_find_lapic(0); dev; dev = dev->next) {
		if (my_apic_id > dev->path.apic.apic_id)
			cpu_index++;
	}

	return cpu_index;
}

/*
 * This function determines the type (performance or efficient) of the CPU that
 * is executing it and stores the information (in a thread-safe manner) in an
 * global_cpu_type array.
 * It requires the SoC to implement a function `get_soc_cpu_type()` which will be
 * called in a critical section to determine the type of the executing CPU.
 */
static void set_cpu_type(void *unused)
{
	spin_lock(&cpu_lock);
	u8 cpu_index = get_cpu_index();

	if (is_perf_core())
		global_cpu_type[cpu_index] = CPU_TYPE_PERF;

	spin_unlock(&cpu_lock);
}

static void run_set_cpu_type(void *unused)
{
	if (mp_run_on_all_cpus(set_cpu_type, NULL) != CB_SUCCESS) {
		printk(BIOS_ERR, "cpu_hybrid: Failed to set global_cpu_type with CPU type info\n");
		return;
	}
}

static void acpi_get_cpu_nomi_perf(u16 *eff_core_nom_perf, u16 *perf_core_nom_perf)
{
	u16 perf_core_scal_factor, eff_core_scal_factor;
	u8 max_non_turbo_ratio = cpu_get_max_non_turbo_ratio();

	soc_get_scaling_factor(&perf_core_scal_factor, &eff_core_scal_factor);

	*perf_core_nom_perf = (u16)((max_non_turbo_ratio * perf_core_scal_factor) / 100);

	*eff_core_nom_perf = (u16)((max_non_turbo_ratio * eff_core_scal_factor) / 100);
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

	if (global_cpu_type[core_id] == CPU_TYPE_PERF)
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
			 CONFIG_ACPI_CPU_STRING "." CPPC_PACKAGE_NAME, 0);

	acpigen_write_method("_CPC", 0);

	/* Update nominal performance and nominal frequency */
	acpigen_cppc_update_nominal_freq_perf(pkg_path, core_id);
	acpigen_emit_byte(RETURN_OP);
	acpigen_emit_namestring(pkg_path);
	acpigen_pop_len();
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT_CHIPS, BS_ON_EXIT, run_set_cpu_type, NULL);
