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
#include <cbmem.h>
#include <cpu/amd/car.h>
#include <cpu/amd/agesa/s3_resume.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/mtrr.h>
#include <console/console.h>
#include <halt.h>
#include <program_loading.h>
#include <smp/node.h>
#include <string.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/agesa/state_machine.h>

void asmlinkage early_all_cores(void)
{
	amd_initmmio();
}

void __attribute__((weak)) platform_once(struct sysinfo *cb)
{
	board_BeforeAgesa(cb);
}

static void fill_sysinfo(struct sysinfo *cb)
{
	memset(cb, 0, sizeof(*cb));
	cb->s3resume = acpi_is_wakeup_s3();

	if (!HAS_LEGACY_WRAPPER)
		agesa_set_interface(cb);
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

	if (!HAS_LEGACY_WRAPPER) {

		agesa_execute_state(cb, AMD_INIT_RESET);

		agesa_execute_state(cb, AMD_INIT_EARLY);

		if (!cb->s3resume)
			agesa_execute_state(cb, AMD_INIT_POST);
		else
			agesa_execute_state(cb, AMD_INIT_RESUME);

	} else {

		agesa_main(cb);

	}

	uintptr_t stack_top = CACHE_TMP_RAMTOP;
	if (cb->s3resume) {
		if (cbmem_recovery(1)) {
			printk(BIOS_EMERG, "Unable to recover CBMEM\n");
			halt();
		}
		stack_top = romstage_ram_stack_base(HIGH_ROMSTAGE_STACK_SIZE,
			ROMSTAGE_STACK_CBMEM);
		stack_top += HIGH_ROMSTAGE_STACK_SIZE;
	}

	printk(BIOS_DEBUG, "Move CAR stack.\n");
	return (void*)stack_top;
}

void asmlinkage romstage_after_car(void)
{
	struct sysinfo romstage_state;
	struct sysinfo *cb = &romstage_state;

	printk(BIOS_DEBUG, "CAR disabled.\n");

	fill_sysinfo(cb);

	if (HAS_LEGACY_WRAPPER)
		agesa_postcar(cb);

	if (cb->s3resume)
		set_resume_cache();

	run_ramstage();
}
