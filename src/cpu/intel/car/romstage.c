/*
 * This file is part of the coreboot project.
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
#include <console/console.h>
#include <cpu/intel/romstage.h>
#include <program_loading.h>

#define DCACHE_RAM_ROMSTAGE_STACK_SIZE 0x800

void * asmlinkage romstage_main(unsigned long bist)
{
	int i;
	void *romstage_stack_after_car;
	const int num_guards = 4;
	const u32 stack_guard = 0xdeadbeef;
	u32 *stack_base = (void *)(CONFIG_DCACHE_RAM_BASE +
	                           CONFIG_DCACHE_RAM_SIZE -
	                           DCACHE_RAM_ROMSTAGE_STACK_SIZE);

	for (i = 0; i < num_guards; i++)
		stack_base[i] = stack_guard;

	mainboard_romstage_entry(bist);

	/* Check the stack. */
	for (i = 0; i < num_guards; i++) {
		if (stack_base[i] == stack_guard)
			continue;
		printk(BIOS_DEBUG, "Smashed stack detected in romstage!\n");
	}

	/* Get the stack to use after cache-as-ram is torn down. */
	if (IS_ENABLED(CONFIG_LATE_CBMEM_INIT))
		romstage_stack_after_car = (void*)CONFIG_RAMTOP;
	else
		romstage_stack_after_car = setup_stack_and_mtrrs();

	return romstage_stack_after_car;
}

void asmlinkage romstage_after_car(void)
{
	/* Load the ramstage. */
	run_ramstage();
}
