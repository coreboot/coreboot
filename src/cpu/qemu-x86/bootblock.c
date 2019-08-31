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
#include <console/console.h>
#include <cpu/x86/bist.h>

asmlinkage void bootblock_c_entry_bist(uint64_t base_timestamp, uint32_t bist)
{
	post_code(0x05);

	/* Halt if there was a built in self test failure */
	if (bist) {
		console_init();
		report_bist_failure(bist);
	}

	/* Call lib/bootblock.c main */
	bootblock_main_with_basetime(base_timestamp);
}
