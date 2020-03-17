/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/exception.h>
#include <console/console.h>
#include <delay.h>
#include <program_loading.h>
#include <symbols.h>
#include <timestamp.h>
#include <soc/bootblock.h>

DECLARE_OPTIONAL_REGION(timestamp);

__attribute__((weak)) void bootblock_mainboard_early_init(void) { /* no-op */ }
__attribute__((weak)) void bootblock_soc_early_init(void) { /* do nothing */ }
__attribute__((weak)) void bootblock_soc_init(void) { /* do nothing */ }
__attribute__((weak)) void bootblock_mainboard_init(void) { /* do nothing */ }


/* C code entry point for the boot block */
void bootblock_main(const uint64_t reg_x0,
		    const uint64_t reg_pc)
{

	init_timer();

	/* Initialize timestamps if we have TIMESTAMP region in memlayout.ld. */
	if (CONFIG(COLLECT_TIMESTAMPS) && REGION_SIZE(timestamp) > 0)
		timestamp_init(timestamp_get());

	bootblock_soc_early_init();
	bootblock_mainboard_early_init();

	if (CONFIG(BOOTBLOCK_CONSOLE)) {
		console_init();
		exception_init();

		if (reg_x0)
			printk(BIOS_ERR,
			       "BOOTBLOCK: RST Boot Failure Code %lld\n",
			       reg_x0);
	}

	bootblock_soc_init();
	bootblock_mainboard_init();

	run_romstage();
}
