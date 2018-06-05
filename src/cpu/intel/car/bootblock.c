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
#include <cpu/intel/car/bootblock.h>

asmlinkage void bootblock_c_entry_bist(uint64_t base_timestamp, uint32_t bist)
{
	/* Call lib/bootblock.c main */
	bootblock_main_with_timestamp(base_timestamp, NULL, 0);
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
}
