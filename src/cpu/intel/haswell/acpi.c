/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <console/console.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/speedstep.h>
#include <cpu/intel/turbo.h>
#include <device/device.h>
#include "haswell.h"
#include "chip.h"

#include <southbridge/intel/lynxpoint/pch.h>

static int cstate_set_s0ix[3] = {
	C_STATE_C1E,
	C_STATE_C7S_LONG_LAT,
	C_STATE_C10,
};

static int cstate_set_lp[3] = {
	C_STATE_C1E,
	C_STATE_C3,
	C_STATE_C7S_LONG_LAT,
};

static int cstate_set_trad[3] = {
	C_STATE_C1,
	C_STATE_C3,
	C_STATE_C6_LONG_LAT,
};

static int get_cores_per_package(void)
{
	struct cpuinfo_x86 c;
	struct cpuid_result result;
	int cores = 1;

	get_fms(&c, cpuid_eax(1));
	if (c.x86 != 6)
		return 1;

	result = cpuid_ext(0xb, 1);
	cores = result.ebx & 0xff;

	return cores;
}

static acpi_tstate_t tss_table_fine[] = {
	{ 100, 1000, 0, 0x00, 0 },
	{ 94, 940, 0, 0x1f, 0 },
	{ 88, 880, 0, 0x1e, 0 },
	{ 82, 820, 0, 0x1d, 0 },
	{ 75, 760, 0, 0x1c, 0 },
	{ 69, 700, 0, 0x1b, 0 },
	{ 63, 640, 0, 0x1a, 0 },
	{ 57, 580, 0, 0x19, 0 },
	{ 50, 520, 0, 0x18, 0 },
	{ 44, 460, 0, 0x17, 0 },
	{ 38, 400, 0, 0x16, 0 },
	{ 32, 340, 0, 0x15, 0 },
	{ 25, 280, 0, 0x14, 0 },
	{ 19, 220, 0, 0x13, 0 },
	{ 13, 160, 0, 0x12, 0 },
};

static acpi_tstate_t tss_table_coarse[] = {
	{ 100, 1000, 0, 0x00, 0 },
	{ 88, 875, 0, 0x1f, 0 },
	{ 75, 750, 0, 0x1e, 0 },
	{ 63, 625, 0, 0x1d, 0 },
	{ 50, 500, 0, 0x1c, 0 },
	{ 38, 375, 0, 0x1b, 0 },
	{ 25, 250, 0, 0x1a, 0 },
	{ 13, 125, 0, 0x19, 0 },
};

static void generate_T_state_entries(int core, int cores_per_package)
{
	/* Indicate SW_ALL coordination for T-states */
	acpigen_write_TSD_package(core, cores_per_package, SW_ALL);

	/* Indicate FFixedHW so OS will use MSR */
	acpigen_write_empty_PTC();

	/* Set a T-state limit that can be modified in NVS */
	acpigen_write_TPC("\\TLVL");

	/*
	 * CPUID.(EAX=6):EAX[5] indicates support
	 * for extended throttle levels.
	 */
	if (cpuid_eax(6) & (1 << 5))
		acpigen_write_TSS_package(
			ARRAY_SIZE(tss_table_fine), tss_table_fine);
	else
		acpigen_write_TSS_package(
			ARRAY_SIZE(tss_table_coarse), tss_table_coarse);
}

static bool is_s0ix_enabled(void)
{
	if (!haswell_is_ult())
		return false;

	const struct device *lapic = dev_find_lapic(SPEEDSTEP_APIC_MAGIC);

	if (!lapic || !lapic->chip_info)
		return false;

	const struct cpu_intel_haswell_config *conf = lapic->chip_info;

	return conf->s0ix_enable;
}

static void generate_C_state_entries(void)
{
	acpi_cstate_t map[3];
	int *set;
	int i;

	struct cpu_info *info;
	struct cpu_driver *cpu;

	/* Find CPU map of supported C-states */
	info = cpu_info();
	if (!info)
		return;
	cpu = find_cpu_driver(info->cpu);
	if (!cpu || !cpu->cstates)
		return;

	if (is_s0ix_enabled())
		set = cstate_set_s0ix;
	else if (haswell_is_ult())
		set = cstate_set_lp;
	else
		set = cstate_set_trad;

	for (i = 0; i < ARRAY_SIZE(map); i++) {
		map[i] = cpu->cstates[set[i]];
		map[i].ctype = i + 1;
	}

	/* Generate C-state tables */
	acpigen_write_CST_package(map, ARRAY_SIZE(map));
}

static int calculate_power(int tdp, int p1_ratio, int ratio)
{
	u32 m;
	u32 power;

	/*
	 * M = ((1.1 - ((p1_ratio - ratio) * 0.00625)) / 1.1) ^ 2
	 *
	 * Power = (ratio / p1_ratio) * m * tdp
	 */

	m = (110000 - ((p1_ratio - ratio) * 625)) / 11;
	m = (m * m) / 1000;

	power = ((ratio * 100000 / p1_ratio) / 100);
	power *= (m / 100) * (tdp / 1000);
	power /= 1000;

	return (int)power;
}

static void generate_P_state_entries(int core, int cores_per_package)
{
	int ratio_min, ratio_max, ratio_turbo, ratio_step;
	int coord_type, power_max, power_unit, num_entries;
	int ratio, power, clock, clock_max;
	msr_t msr;

	/* Determine P-state coordination type from MISC_PWR_MGMT[0] */
	msr = rdmsr(MSR_MISC_PWR_MGMT);
	if (msr.lo & MISC_PWR_MGMT_EIST_HW_DIS)
		coord_type = SW_ANY;
	else
		coord_type = HW_ALL;

	/* Get bus ratio limits and calculate clock speeds */
	msr = rdmsr(MSR_PLATFORM_INFO);
	ratio_min = (msr.hi >> (40-32)) & 0xff; /* Max Efficiency Ratio */

	/* Determine if this CPU has configurable TDP */
	if (cpu_config_tdp_levels()) {
		/* Set max ratio to nominal TDP ratio */
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		ratio_max = msr.lo & 0xff;
	} else {
		/* Max Non-Turbo Ratio */
		ratio_max = (msr.lo >> 8) & 0xff;
	}
	clock_max = ratio_max * CPU_BCLK;

	/* Calculate CPU TDP in mW */
	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 2 << ((msr.lo & 0xf) - 1);
	msr = rdmsr(MSR_PKG_POWER_SKU);
	power_max = ((msr.lo & 0x7fff) / power_unit) * 1000;

	/* Write _PCT indicating use of FFixedHW */
	acpigen_write_empty_PCT();

	/* Write _PPC with no limit on supported P-state */
	acpigen_write_PPC_NVS();

	/* Write PSD indicating configured coordination type */
	acpigen_write_PSD_package(core, 1, coord_type);

	/* Add P-state entries in _PSS table */
	acpigen_write_name("_PSS");

	/* Determine ratio points */
	ratio_step = PSS_RATIO_STEP;
	num_entries = (ratio_max - ratio_min) / ratio_step;
	while (num_entries > PSS_MAX_ENTRIES-1) {
		ratio_step <<= 1;
		num_entries >>= 1;
	}

	/* P[T] is Turbo state if enabled */
	if (get_turbo_state() == TURBO_ENABLED) {
		/* _PSS package count including Turbo */
		acpigen_write_package(num_entries + 2);

		msr = rdmsr(MSR_TURBO_RATIO_LIMIT);
		ratio_turbo = msr.lo & 0xff;

		/* Add entry for Turbo ratio */
		acpigen_write_PSS_package(
			clock_max + 1,		/*MHz*/
			power_max,		/*mW*/
			PSS_LATENCY_TRANSITION,	/*lat1*/
			PSS_LATENCY_BUSMASTER,	/*lat2*/
			ratio_turbo << 8,	/*control*/
			ratio_turbo << 8);	/*status*/
	} else {
		/* _PSS package count without Turbo */
		acpigen_write_package(num_entries + 1);
	}

	/* First regular entry is max non-turbo ratio */
	acpigen_write_PSS_package(
		clock_max,		/*MHz*/
		power_max,		/*mW*/
		PSS_LATENCY_TRANSITION,	/*lat1*/
		PSS_LATENCY_BUSMASTER,	/*lat2*/
		ratio_max << 8,		/*control*/
		ratio_max << 8);	/*status*/

	/* Generate the remaining entries */
	for (ratio = ratio_min + ((num_entries - 1) * ratio_step);
	     ratio >= ratio_min; ratio -= ratio_step) {

		/* Calculate power at this ratio */
		power = calculate_power(power_max, ratio_max, ratio);
		clock = ratio * CPU_BCLK;

		acpigen_write_PSS_package(
			clock,			/*MHz*/
			power,			/*mW*/
			PSS_LATENCY_TRANSITION,	/*lat1*/
			PSS_LATENCY_BUSMASTER,	/*lat2*/
			ratio << 8,		/*control*/
			ratio << 8);		/*status*/
	}

	/* Fix package length */
	acpigen_pop_len();
}

void generate_cpu_entries(const struct device *device)
{
	int coreID, cpuID, pcontrol_blk = get_pmbase(), plen = 6;
	int totalcores = dev_count_cpu();
	int cores_per_package = get_cores_per_package();
	int numcpus = totalcores/cores_per_package;

	printk(BIOS_DEBUG, "Found %d CPU(s) with %d core(s) each.\n",
	       numcpus, cores_per_package);

	for (cpuID = 1; cpuID <= numcpus; cpuID++) {
		for (coreID = 1; coreID <= cores_per_package; coreID++) {
			if (coreID > 1) {
				pcontrol_blk = 0;
				plen = 0;
			}

			/* Generate processor \_SB.CPUx */
			acpigen_write_processor(
				(cpuID - 1) * cores_per_package+coreID - 1,
				pcontrol_blk, plen);

			/* Generate P-state tables */
			generate_P_state_entries(
				coreID - 1, cores_per_package);

			/* Generate C-state tables */
			generate_C_state_entries();

			/* Generate T-state tables */
			generate_T_state_entries(
				cpuID - 1, cores_per_package);

			acpigen_pop_len();
		}
	}

	/* PPKG is usually used for thermal management
	   of the first and only package. */
	acpigen_write_processor_package("PPKG", 0, cores_per_package);

	/* Add a method to notify processor nodes */
	acpigen_write_processor_cnot(cores_per_package);
}

struct chip_operations cpu_intel_haswell_ops = {
	CHIP_NAME("Intel Haswell CPU")
};
