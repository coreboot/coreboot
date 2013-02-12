/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 *               2012 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <types.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/speedstep.h>
#include <device/device.h>

/**
 * @brief Returns c-state entries for this system
 *
 * This function is usually overwritten in mainboard code.
 *
 * @return Number of c-states *entries will point to.
 */
int __attribute__((weak)) get_cst_entries(acpi_cstate_t **entries
					  __attribute__((unused)))
{
	return 0;
}

static int determine_total_number_of_cores(void)
{
	device_t cpu;
	int count = 0;
	for(cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
			(cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled) {
			continue;
		}
		count++;
	}
	return count;
}

/**
 * @brief Returns three times the FSB clock in MHz
 *
 * The result of calculations with the returned value shall be divided by 3.
 * This helps to avoid rounding errors.
 */
static int get_fsb(void)
{
	const u32 fsbcode = rdmsr(MSR_FSB_FREQ).lo & 7;
	switch (fsbcode) {
		case 0: return  800; /*  / 3 == 266 */
		case 1: return  400; /*  / 3 == 133 */
		case 2: return  600; /*  / 3 == 200 */
		case 3: return  500; /*  / 3 == 166 */
		case 4: return 1000; /*  / 3 == 333 */
		case 5: return  300; /*  / 3 == 100 */
		case 6: return 1200; /*  / 3 == 400 */
	}
	printk(BIOS_WARNING,
	       "Warning: No supported FSB frequency. Assuming 200MHz\n");
	return 600;
}

static int gen_pstate_entries(const sst_table_t *const pstates,
			      const int cpuID, const int cores_per_package,
			      const uint8_t coordination)
{
	int i;
	int len, len_ps;
	int frequency;

	len = acpigen_write_empty_PCT();
	len += acpigen_write_PSD_package(
			cpuID, cores_per_package, coordination);
	len += acpigen_write_name("_PSS");

	const int fsb3 = get_fsb();
	const int min_ratio2 = SPEEDSTEP_DOUBLE_RATIO(
		pstates->states[pstates->num_states - 1]);
	const int max_ratio2 = SPEEDSTEP_DOUBLE_RATIO(pstates->states[0]);
	printk(BIOS_DEBUG, "clocks between %d and %d MHz.\n",
	       (min_ratio2 * fsb3)
		/ (pstates->states[pstates->num_states - 1].is_slfm ? 12 : 6),
	       (max_ratio2 * fsb3) / 6);

	printk(BIOS_DEBUG, "adding %x P-States between "
			   "busratio %x and %x, ""incl. P0\n",
	       pstates->num_states, min_ratio2 / 2, max_ratio2 / 2);
	len_ps = acpigen_write_package(pstates->num_states);
	for (i = 0; i < pstates->num_states; ++i) {
		const sst_state_t *const pstate = &pstates->states[i];
		/* Report frequency of turbo mode as that of HFM + 1. */
		if (pstate->is_turbo)
			frequency = (SPEEDSTEP_DOUBLE_RATIO(
					pstates->states[i + 1]) * fsb3) / 6 + 1;
		/* Super-LFM runs at half frequency. */
		else if (pstate->is_slfm)
			frequency = (SPEEDSTEP_DOUBLE_RATIO(*pstate)*fsb3)/12;
		else
			frequency = (SPEEDSTEP_DOUBLE_RATIO(*pstate)*fsb3)/6;
		len_ps += acpigen_write_PSS_package(
			frequency, pstate->power, 0, 0,
			SPEEDSTEP_ENCODE_STATE(*pstate),
			SPEEDSTEP_ENCODE_STATE(*pstate));
	}
	len_ps--;
	acpigen_patch_len(len_ps);

	len += acpigen_write_PPC(0);

	len += len_ps;

	return len;
}

/**
 * @brief Generate ACPI entries for Speedstep for each cpu
 */
void generate_cpu_entries(void)
{
	int len_pr;
	int coreID, cpuID, pcontrol_blk = PMB0_BASE, plen = 6;
	int totalcores = determine_total_number_of_cores();
	int cores_per_package = (cpuid_ebx(1)>>16) & 0xff;
	int numcpus = totalcores/cores_per_package; /* This assumes that all
						       CPUs share the same
						       layout. */
	int num_cstates;
	acpi_cstate_t *cstates;
	sst_table_t pstates;
	uint8_t coordination;

	printk(BIOS_DEBUG, "Found %d CPU(s) with %d core(s) each.\n",
	       numcpus, cores_per_package);

	num_cstates = get_cst_entries(&cstates);
	speedstep_gen_pstates(&pstates);
	if (((cpuid_eax(1) >> 4) & 0xffff) == 0x1067)
		/* For Penryn use HW_ALL. */
		coordination = HW_ALL;
	else
		/* Use SW_ANY as that was the default. */
		coordination = SW_ANY;

	for (cpuID = 0; cpuID < numcpus; ++cpuID) {
		for (coreID=1; coreID<=cores_per_package; coreID++) {
			if (coreID>1) {
				pcontrol_blk = 0;
				plen = 0;
			}

			/* Generate processor \_PR.CPUx. */
			len_pr = acpigen_write_processor(
					cpuID * cores_per_package + coreID - 1,
					pcontrol_blk, plen);

			/* Generate p-state entries. */
			len_pr += gen_pstate_entries(&pstates, cpuID,
					cores_per_package, coordination);

			/* Generate c-state entries. */
			if (num_cstates > 0)
				len_pr += acpigen_write_CST_package(
							cstates, num_cstates);

			len_pr--;
			acpigen_patch_len(len_pr);
		}
	}
}
