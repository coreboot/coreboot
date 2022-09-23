/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include "i82371eb.h"

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

void generate_cpu_entries(const struct device *device)
{
	int cpu, pcontrol_blk=DEFAULT_PMBASE+PCNTRL, plen=6;
	int numcpus = determine_total_number_of_cores();
	printk(BIOS_DEBUG, "Found %d CPU(s).\n", numcpus);

	/* without the outer scope, further ssdt addition will end up
	 * within the processor statement */
	acpigen_write_scope("\\_SB");
	for (cpu=0; cpu < numcpus; cpu++) {
		acpigen_write_processor(cpu, pcontrol_blk, plen);
		acpigen_pop_len();
	}
	acpigen_pop_len();
}
