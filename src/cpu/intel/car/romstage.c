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
#include <cpu/x86/mtrr.h>
#include <arch/symbols.h>
#include <program_loading.h>

#define DCACHE_RAM_ROMSTAGE_STACK_SIZE 0x2000

asmlinkage void *romstage_main(unsigned long bist)
{
	int i;
	const int num_guards = 4;
	const u32 stack_guard = 0xdeadbeef;
	u32 *stack_base;
	u32 size;

	/* Size of unallocated CAR. */
	size = _car_region_end - _car_relocatable_data_end;
	size = ALIGN_DOWN(size, 16);

	size = MIN(size, DCACHE_RAM_ROMSTAGE_STACK_SIZE);
	if (size < DCACHE_RAM_ROMSTAGE_STACK_SIZE)
		printk(BIOS_DEBUG, "Romstage stack size limited to 0x%x!\n",
			size);

	stack_base = (u32 *) (_car_region_end - size);

	for (i = 0; i < num_guards; i++)
		stack_base[i] = stack_guard;

	mainboard_romstage_entry(bist);

	/* Check the stack. */
	for (i = 0; i < num_guards; i++) {
		if (stack_base[i] == stack_guard)
			continue;
		printk(BIOS_DEBUG, "Smashed stack detected in romstage!\n");
	}

	if (!IS_ENABLED(CONFIG_POSTCAR_STAGE))
		return setup_stack_and_mtrrs();

	platform_enter_postcar();

	/* We do not return. */
	return NULL;
}

asmlinkage void romstage_after_car(void)
{
	/* Load the ramstage. */
	run_ramstage();
}
