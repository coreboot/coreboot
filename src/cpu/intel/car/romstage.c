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
#include <arch/romstage.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <arch/symbols.h>
#include <commonlib/helpers.h>
#include <program_loading.h>
#include <timestamp.h>
#include <security/vboot/vboot_common.h>

/* If we do not have a constrained _car_stack region size, use the
   following as a guideline for acceptable stack usage. */
#define DCACHE_RAM_ROMSTAGE_STACK_SIZE 0x2000

static struct postcar_frame early_mtrrs;

static void romstage_main(unsigned long bist)
{
	int i;
	const int num_guards = 64;
	const u32 stack_guard = 0xdeadbeef;
	u32 *stack_base;
	u32 size;
	const size_t stack_size = MAX(CONFIG_DCACHE_BSP_STACK_SIZE,
				      DCACHE_RAM_ROMSTAGE_STACK_SIZE);

	/* Size of unallocated CAR. */
	size = ALIGN_DOWN(_car_stack_size, 16);

	size = MIN(size, stack_size);
	if (size < stack_size)
		printk(BIOS_DEBUG, "Romstage stack size limited to 0x%x!\n",
			size);

	stack_base = (u32 *) (_ecar_stack - size);

	for (i = 0; i < num_guards; i++)
		stack_base[i] = stack_guard;

	if (CONFIG(VBOOT_EARLY_EC_SYNC))
		vboot_sync_ec();

	mainboard_romstage_entry();

	/* Check the stack. */
	for (i = 0; i < num_guards; i++) {
		if (stack_base[i] == stack_guard)
			continue;
		printk(BIOS_DEBUG, "Smashed stack detected in romstage!\n");
	}

	if (CONFIG(SMM_TSEG))
		smm_list_regions();

	prepare_and_run_postcar(&early_mtrrs);
	/* We do not return here. */
}

#if CONFIG(ROMCC_BOOTBLOCK)
/* This wrapper enables easy transition away from ROMCC_BOOTBLOCK
 * keeping changes in cache_as_ram.S easy to manage.
 */
asmlinkage void bootblock_c_entry_bist(uint64_t base_timestamp, uint32_t bist)
{
	timestamp_init(base_timestamp);
	timestamp_add_now(TS_START_ROMSTAGE);
	romstage_main(bist);
}
#endif


/* We don't carry BIST from bootblock in a good location to read from.
 * Any error should have been reported in bootblock already.
 */
#define NO_BIST 0

asmlinkage void car_stage_entry(void)
{
	timestamp_add_now(TS_START_ROMSTAGE);

	/* Assumes the hardware was set up during the bootblock */
	console_init();

	romstage_main(NO_BIST);
}
