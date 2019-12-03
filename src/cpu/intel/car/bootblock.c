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

#include <bootblock_common.h>
#include <arch/bootblock.h>
#include <cpu/x86/bist.h>

static uint32_t saved_bist;

asmlinkage void bootblock_c_entry_bist(uint64_t base_timestamp, uint32_t bist)
{
	saved_bist = bist;
	/* Call lib/bootblock.c main */
	bootblock_main_with_basetime(base_timestamp);
}

void __weak bootblock_early_northbridge_init(void) { }
void __weak bootblock_early_southbridge_init(void) { }
void __weak bootblock_early_cpu_init(void) { }

void bootblock_soc_early_init(void)
{
	bootblock_early_northbridge_init();
	bootblock_early_southbridge_init();
	bootblock_early_cpu_init();
}

void bootblock_soc_init(void)
{
	/* Halt if there was a built in self test failure */
	report_bist_failure(saved_bist);
}
