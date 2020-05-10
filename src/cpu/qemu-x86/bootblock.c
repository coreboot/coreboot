/* SPDX-License-Identifier: GPL-2.0-only */

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
