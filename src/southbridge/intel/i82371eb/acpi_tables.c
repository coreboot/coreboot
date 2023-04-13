/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <device/device.h>
#include "i82371eb.h"

static void generate_cpu_entry(int cpu)
{
	acpigen_write_processor_device(cpu);

	/* bit 1:3 in PCNTRL reg (pmbase+0x10) */
	acpigen_write_PTC(3, 1, DEFAULT_PMBASE + PCNTRL);

	acpigen_write_processor_device_end();
}

void generate_cpu_entries(const struct device *device)
{
	int cpu;
	int numcpus = dev_count_cpu();

	printk(BIOS_DEBUG, "Found %d CPU(s).\n", numcpus);

	/* without the outer scope, further ssdt addition will end up
	 * within the processor statement */
	acpigen_write_scope("\\_SB");

	for (cpu = 0; cpu < numcpus; cpu++)
		generate_cpu_entry(cpu);

	acpigen_pop_len();
}
