/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Kyösti Mälkki
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

#include <arch/acpi.h>
#include <arch/cpu.h>
#include <cpu/amd/car.h>
#include <cpu/x86/bist.h>
#include <console/console.h>
#include <smp/node.h>
#include <string.h>
#include <northbridge/amd/agesa/state_machine.h>

static void fill_sysinfo(struct sysinfo *cb)
{
	memset(cb, 0, sizeof(*cb));
	cb->s3resume = acpi_is_wakeup_s3();
}

void * asmlinkage romstage_main(unsigned long bist)
{
	struct sysinfo romstage_state;
	struct sysinfo *cb = &romstage_state;
	u8 initial_apic_id = (u8) (cpuid_ebx(1) >> 24);

	fill_sysinfo(cb);

	if ((initial_apic_id == 0) && boot_cpu()) {

		platform_once(cb);

		console_init();
	}

	printk(BIOS_DEBUG, "APIC %02d: CPU Family_Model = %08x\n",
		initial_apic_id, cpuid_eax(1));

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	agesa_main(cb);

	/* Not reached */
	return NULL;
}
