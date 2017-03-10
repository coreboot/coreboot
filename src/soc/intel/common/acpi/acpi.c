/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <arch/cpu.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/msr.h>
#include <soc/intel/common/acpi.h>
#include <soc/pm.h>

#define MSR_PLATFORM_INFO		0xce
#define MSR_TURBO_RATIO_LIMIT		0x1ad
#define MSR_CONFIG_TDP_NOMINAL		0x648
#define MSR_RAPL_POWER_UNIT		0x606
#define MSR_PKG_POWER_INFO		0x614

/* P-state configuration */
#define PSS_MAX_ENTRIES			8
#define PSS_RATIO_STEP			2
#define PSS_LATENCY_TRANSITION		10
#define PSS_LATENCY_BUSMASTER		10


__attribute__((weak)) int cpu_get_coord_type(void)
{
	return HW_ALL;
}

__attribute__((weak)) int cpu_config_tdp_levels(void)
{
	return 0;
}

__attribute__((weak)) uint32_t cpu_get_min_ratio(void)
{
	msr_t msr;
	/* Get bus ratio limits and calculate clock speeds */
	msr = rdmsr(MSR_PLATFORM_INFO);
	return ((msr.hi >> 8) & 0xff); /* Max Efficiency Ratio */
}

__attribute__((weak)) uint32_t cpu_get_max_ratio(void)
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

__attribute__((weak)) uint32_t cpu_get_bus_clock(void)
{
	/* CPU bus clock is set by default here to 100MHz.
           This function returns the bus clock in KHz.
	*/
	return 100 * KHz;
}

__attribute__((weak)) uint32_t cpu_get_power_max(void)
{
	msr_t msr;
	int power_unit;

	msr = rdmsr(MSR_RAPL_POWER_UNIT);
	power_unit = 2 << ((msr.lo & 0xf) - 1);
	msr = rdmsr(MSR_PKG_POWER_INFO);
	return ((msr.lo & 0x7fff) / power_unit) * 1000;
}

__attribute__((weak)) uint32_t cpu_get_max_turbo_ratio(void)
{
	msr_t msr;
	msr = rdmsr(MSR_TURBO_RATIO_LIMIT);
	return msr.lo & 0xff;
}

__attribute__((weak)) acpi_cstate_t *soc_get_cstate_map(int *entries)
{
	*entries = 0;
	return NULL;
}

__attribute__((weak)) acpi_tstate_t *soc_get_tss_table(int *entries)
{
	*entries = 0;
	return NULL;
}

__attribute__((weak)) uint16_t soc_get_acpi_base_address(void)
{
#define ACPI_BASE_ADDR 0x400
	return (ACPI_BASE_ADDR);
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
	return power;
}

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

static void generate_p_state_entries(int core, int cores_per_package)
{
	int ratio_min, ratio_max, ratio_turbo, ratio_step;
	int coord_type, power_max, num_entries;
	int ratio, power, clock, clock_max;

	coord_type = cpu_get_coord_type();
	ratio_min = cpu_get_min_ratio();
	ratio_max = cpu_get_max_ratio();
	clock_max = (ratio_max * cpu_get_bus_clock()) / KHz;

	/* Calculate CPU TDP in mW */
	power_max = cpu_get_power_max();

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
	num_entries = ((ratio_max - ratio_min) / ratio_step) + 1;
	if (num_entries > PSS_MAX_ENTRIES) {
		ratio_step += 1;
		num_entries = ((ratio_max - ratio_min) / ratio_step) + 1;
	}

	/* P[T] is Turbo state if enabled */
	if (get_turbo_state() == TURBO_ENABLED) {
	/* _PSS package count including Turbo */
		acpigen_write_package(num_entries + 2);
		ratio_turbo = cpu_get_max_turbo_ratio();

		/* Add entry for Turbo ratio */
		acpigen_write_PSS_package(
			clock_max + 1,          /* MHz */
			power_max,              /* mW */
			PSS_LATENCY_TRANSITION, /* lat1 */
			PSS_LATENCY_BUSMASTER,  /* lat2 */
			ratio_turbo << 8,       /* control */
			ratio_turbo << 8);      /* status */
	} else {
		/* _PSS package count without Turbo */
		acpigen_write_package(num_entries + 1);
	}

	/* First regular entry is max non-turbo ratio */
	acpigen_write_PSS_package(
		clock_max,              /* MHz */
		power_max,              /* mW */
		PSS_LATENCY_TRANSITION, /* lat1 */
		PSS_LATENCY_BUSMASTER,  /* lat2 */
		ratio_max << 8,         /* control */
		ratio_max << 8);        /* status */

	/* Generate the remaining entries */
	for (ratio = ratio_min + ((num_entries - 1) * ratio_step);
		ratio >= ratio_min; ratio -= ratio_step) {

	/* Calculate power at this ratio */
		power = calculate_power(power_max, ratio_max, ratio);
		clock = (ratio * cpu_get_bus_clock()) / KHz;

		acpigen_write_PSS_package(
			clock,                  /* MHz */
			power,                  /* mW */
			PSS_LATENCY_TRANSITION, /* lat1 */
			PSS_LATENCY_BUSMASTER,  /* lat2 */
			ratio << 8,             /* control */
			ratio << 8);            /* status */
	}
	/* Fix package length */
	acpigen_pop_len();
}

static void generate_c_state_entries(void)
{
	acpi_cstate_t *c_state_map;
	int entries;

	c_state_map = soc_get_cstate_map(&entries);

	/* Generate C-state tables */
	acpigen_write_CST_package(c_state_map, entries);
}

static void generate_t_state_entries(int core, int cores_per_package)
{
	acpi_tstate_t *soc_tss_table;
	int entries;

	soc_tss_table = soc_get_tss_table(&entries);
	if (entries == 0)
		return;

	/* Indicate SW_ALL coordination for T-states */
	acpigen_write_TSD_package(core, cores_per_package, SW_ALL);

	/* Indicate FFixedHW so OS will use MSR */
	acpigen_write_empty_PTC();

	/* Set NVS controlled T-state limit */
	acpigen_write_TPC("\\TLVL");

	/* Write TSS table for MSR access */
	acpigen_write_TSS_package(entries, soc_tss_table);
}

void generate_cpu_entries(device_t device)
{
	int core_id, cpu_id, pcontrol_blk = soc_get_acpi_base_address();
	int plen = 6;
	int totalcores = dev_count_cpu();
	int cores_per_package = get_cores_per_package();
	int numcpus = totalcores / cores_per_package;

	printk(BIOS_DEBUG, "Found %d CPU(s) with %d core(s) each.\n",
		numcpus, cores_per_package);

	for (cpu_id = 0; cpu_id < numcpus; cpu_id++) {
		for (core_id = 0; core_id < cores_per_package; core_id++) {
			if (core_id > 0) {
				pcontrol_blk = 0;
				plen = 0;
			}

			/* Generate processor \_PR.CPUx */
			acpigen_write_processor(
				(cpu_id)*cores_per_package + core_id,
				pcontrol_blk, plen);

			/* Generate P-state tables */
			generate_p_state_entries(
				core_id, cores_per_package);

			/* Generate C-state tables */
			generate_c_state_entries();

			/* Generate T-state tables */
			generate_t_state_entries(
				core_id, cores_per_package);

			acpigen_pop_len();
		}
	}
}
