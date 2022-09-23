/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <console/console.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <arch/cpu.h>
#include <cpu/intel/fsb.h>
#include <cpu/intel/speedstep.h>
#include <device/device.h>

static int determine_total_number_of_cores(void)
{
	struct device *cpu;
	int count = 0;
	for (cpu = all_devices; cpu; cpu = cpu->next) {
		if (!is_enabled_cpu(cpu))
			continue;
		count++;
	}
	return count;
}

static void gen_pstate_entries(const sst_table_t *const pstates,
			      const int cpuID, const int cores_per_package,
			      const uint8_t coordination)
{
	int i;
	int frequency;

	acpigen_write_empty_PCT();
	acpigen_write_PSD_package(
			cpuID, cores_per_package, coordination);
	acpigen_write_name("_PSS");

	int fsb3 = get_ia32_fsb_x3();
	if (fsb3 <= 0) {
		printk(BIOS_ERR, "CPU or FSB not supported. Assuming 200MHz\n");
		fsb3 = 600;
	}

	const int min_ratio2 = SPEEDSTEP_DOUBLE_RATIO(
		pstates->states[pstates->num_states - 1]);
	const int max_ratio2 = SPEEDSTEP_DOUBLE_RATIO(pstates->states[0]);
	printk(BIOS_DEBUG, "clocks between %d and %d MHz.\n",
	       (min_ratio2 * fsb3)
		/ (pstates->states[pstates->num_states - 1].is_slfm ? 12 : 6),
	       (max_ratio2 * fsb3) / 6);

	printk(BIOS_DEBUG,
		"adding %x P-States between busratio %x and %x, incl. P0\n",
	       pstates->num_states, min_ratio2 / 2, max_ratio2 / 2);
	acpigen_write_package(pstates->num_states);
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
		acpigen_write_PSS_package(
			frequency, pstate->power, 0, 0,
			SPEEDSTEP_ENCODE_STATE(*pstate),
			SPEEDSTEP_ENCODE_STATE(*pstate));
	}
	acpigen_pop_len();

	acpigen_write_PPC(0);
}

/**
 * @brief Generate ACPI entries for Speedstep for each cpu
 */
void generate_cpu_entries(const struct device *device)
{
	int coreID, cpuID, pcontrol_blk = PMB0_BASE, plen = 6;
	int totalcores = determine_total_number_of_cores();
	int cores_per_package = (cpuid_ebx(1)>>16) & 0xff;
	int numcpus = totalcores/cores_per_package; /* This assumes that all
						       CPUs share the same
						       layout. */
	int num_cstates;
	const acpi_cstate_t *cstates;
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
		for (coreID = 1; coreID <= cores_per_package; coreID++) {
			if (coreID > 1) {
				pcontrol_blk = 0;
				plen = 0;
			}

			/* Generate processor \_SB.CPUx. */
			acpigen_write_processor(
					cpuID * cores_per_package + coreID - 1,
					pcontrol_blk, plen);

			/* Generate p-state entries. */
			gen_pstate_entries(&pstates, cpuID,
					cores_per_package, coordination);

			/* Generate c-state entries. */
			if (num_cstates > 0)
				acpigen_write_CST_package(
							cstates, num_cstates);

			acpigen_pop_len();
		}
	}
	/* PPKG is usually used for thermal management
	   of the first and only package. */
	acpigen_write_processor_package("PPKG", 0, cores_per_package);

	acpigen_write_processor_cnot(cores_per_package);

	acpigen_write_scope("\\");
	acpigen_write_name_integer("MPEN", numcpus > 1);
	acpigen_pop_len();
}
