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

#include <arch/cpu.h>
#include <cbmem.h>
#include <cpu/amd/car.h>
#include <cpu/x86/mtrr.h>
#include <console/console.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <northbridge/amd/agesa/agesa_helper.h>
#include <northbridge/amd/agesa/state_machine.h>

void asmlinkage early_all_cores(void)
{
	amd_initmmio();
}

void *asmlinkage romstage_main(unsigned long bist)
{
	int s3resume = 0;
	u8 initial_apic_id = cpuid_ebx(1) >> 24;

	/* Only BSP returns from here. */
	cache_as_ram_main(bist, initial_apic_id);

	cbmem_recovery(s3resume);

	romstage_handoff_init(s3resume);

	uintptr_t stack_top = romstage_ram_stack_base(HIGH_ROMSTAGE_STACK_SIZE,
		ROMSTAGE_STACK_CBMEM);
	stack_top += HIGH_ROMSTAGE_STACK_SIZE;

	printk(BIOS_DEBUG, "Move CAR stack.\n");
	return (void *)stack_top;
}

void asmlinkage romstage_after_car(void)
{
	printk(BIOS_DEBUG, "CAR disabled.\n");

	agesa_postcar(NULL);

	run_ramstage();
}
