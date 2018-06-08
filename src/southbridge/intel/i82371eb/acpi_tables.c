/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2005 Nick Barker <nick.barker9@btinternet.com>
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <arch/smp/mpspec.h>
#include <device/device.h>
#include <device/pci_ids.h>
#include "i82371eb.h"

static int determine_total_number_of_cores(void)
{
	struct device *cpu;
	int count = 0;
	for (cpu = all_devices; cpu; cpu = cpu->next) {
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

void generate_cpu_entries(struct device *device)
{
	int cpu, pcontrol_blk=DEFAULT_PMBASE+PCNTRL, plen=6;
	int numcpus = determine_total_number_of_cores();
	printk(BIOS_DEBUG, "Found %d CPU(s).\n", numcpus);

	/* without the outer scope, furhter ssdt addition will end up
	 * within the processor statement */
	acpigen_write_scope("\\_PR");
	for (cpu=0; cpu < numcpus; cpu++) {
		acpigen_write_processor(cpu, pcontrol_blk, plen);
		acpigen_pop_len();
	}
	acpigen_pop_len();
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	/* chipset doesn't have mmconfig */
	return current;
}
